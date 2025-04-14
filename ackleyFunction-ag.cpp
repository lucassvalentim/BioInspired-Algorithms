#include <bits/stdc++.h>
using std::cout;
using std::endl;

std::mt19937 rng(std::random_device{}());

namespace algorithm
{
    namespace detail
    {   
        double bitTointeger(std::string bit){
            double integer = 0;
            double position = 1;
            for(int i = bit.size() - 1; i >= 0; i--){
                if(bit[i] == '1') integer += position;
                position *= 2;
            }

            return integer;
        }

        auto get_random_bit = []() -> char
        {
            const char charset[] = "01";
            const size_t max_index = (sizeof(charset) - 2);
            std::uniform_int_distribution<> dist(0, max_index);
            return charset[dist(rng)];
        };

        auto get_random_string(const size_t length){
            std::string str(length, 0);
            std::generate_n(str.begin(), length, get_random_bit);
            return str;
        }

        std::vector<double> decode(std::vector<std::pair<int, int>> &bounds, std::size_t n_bits, std::string bitstring){
            std::vector<double> decoded(bounds.size());
            uint64_t largest = pow(2, n_bits);
            for(int i = 0; i < bounds.size(); i++){ 
                std::string substring = "";
                int start = i * n_bits;
                int end = (i * n_bits) + n_bits;
                for(int j = start; j < end; j++){
                    substring += bitstring[j];
                }

                double integer = bitTointeger(substring);
                double value = bounds[i].first + (integer/largest) * (bounds[i].second - bounds[i].first);

                decoded[i] = value;
            }

            return decoded;
        }
    } // namespace detail

    class individual {
        public:
            individual() : m_value(""), m_fitness(0.0) {}

            individual(std::vector<std::pair<int, int>> &bound, std::size_t n_bits)
                        : m_value(detail::get_random_string(n_bits*bound.size())), m_fitness(0.0){
                std::vector<double> values = detail::decode(bound, n_bits, m_value);
                calculate_fitness(values);
            }

            individual(const std::string value, std::vector<std::pair<int, int>> &bound, ::size_t n_bits) 
                        : m_value(value), m_fitness(0.0){
                std::vector<double> values = detail::decode(bound, n_bits, m_value);
                calculate_fitness(values);
            }

            std::string get_value() const{
                return this->m_value;
            }

            double get_fitness() const{
                return this->m_fitness;
            }

            auto operator [](const int i) const {
                return this->m_value[i];
            }

            auto operator > (const individual &rhs) const {
                return (this->m_fitness > rhs.m_fitness);
            }

            auto operator < (const individual &rhs) const {
                return (this->m_fitness < rhs.m_fitness);
            }

            auto operator == (const individual &rhs) const {
                return (this->m_fitness == rhs.m_fitness);
            }
            
        private:
            void calculate_fitness(std::vector<double>& x){
                int n = x.size();
            
                double somatorio1 = 0;
                for(int i = 0; i < n; i++){
                    somatorio1 += pow(x[i], 2);
                }
            
                double somatorio2 = 0;
                for(int i = 0; i < n; i++){
                    somatorio2 += cos(2 * M_PI * x[i]);
                }
                
                // ackley function
                this->m_fitness = -20 * exp(-0.2 * sqrt(somatorio1 / n)) 
                        - exp(somatorio2 / n) 
                        + 20 + exp(1);
            }
        
        
        private:
            std::string m_value;
            double m_fitness;
    };

    individual create_child(const individual &p1, const individual &p2, std::vector<std::pair<int, int>> &bound, std::size_t n_bits, int parent_ratio, int mutate_probability){
        std::uniform_int_distribution<std::size_t> distCem(0, 100);
        if(parent_ratio < distCem(rng)) return p1;
        
        std::string c1{""};

        std::size_t length = p2.get_value().size();
        std::uniform_int_distribution<std::size_t> dist(1, length - 2);
        std::size_t index = dist(rng);

        for(int i = 0; i < index; i++){
            c1 += p1.get_value()[i];
        }

        for(int i = index; i < length; i++){
            c1 += p2.get_value()[i];
        }

        for(int i = 0; i < c1.size(); i++){
            if(mutate_probability > distCem(rng))
                c1[i] = (c1[i] == '1') ? '0' : '1';
        }

        return individual{c1, bound, n_bits};
    }

    std::vector<individual> selection_parents(std::vector<individual> m_population, double ratio_selection){
        std::set<individual> possibles_parents;
        std::uniform_int_distribution<std::size_t> dist(0, m_population.size() - 1);
        
        std::size_t count = 0;
        while(count < ratio_selection*m_population.size()){
            std::size_t random_index = dist(rng);
            if(possibles_parents.count(m_population[random_index]) == 0){
                possibles_parents.insert(m_population[random_index]);
                count++;
            }
        }
        
        std::vector<individual> parents(2);
        parents[0] = *(possibles_parents.begin());
        parents[1] = *(next(possibles_parents.begin()));

        return parents;
    }

    class population{
        public:
            population(const std::size_t length_population, const std::size_t parent_ratio, std::size_t mutate_probability, std::size_t transfer_elite_ratio, std::vector<std::pair<int, int>> &bound, ::size_t n_bits)
            : m_generation{1}, m_parent_ratio{parent_ratio}, m_mutate_probability{mutate_probability}{

                m_transfer_count = (transfer_elite_ratio*length_population);
                m_new_individuals_per_generation = length_population - m_transfer_count;
                m_population.reserve(length_population);

                std::generate_n(std::back_inserter(m_population), length_population, [&](){return individual(bound, n_bits);});
                this->sort();
            }

            std::size_t get_generation() const{
                return m_generation;
            }

            std::vector<individual> get_pop(){
                return m_population;
            }

            void sort(){
                std::sort(std::begin(m_population), std::end(m_population), [](const auto&left, const auto&right){return left < right;});
            }

            void create_next_generation(std::vector<std::pair<int, int>> &bound, ::size_t n_bits){

                m_generation++;

                std::vector<individual>next_generation;
                next_generation.reserve(m_population.size());
                for(std::size_t i = 0; i < m_transfer_count; i++){
                    next_generation.push_back(m_population[i]);
                }

                for(int i = 0; i < m_new_individuals_per_generation; i++){
                    std::vector<individual> parents = selection_parents(m_population, 0.2);
                    individual child = create_child(parents[0], parents[1], bound, n_bits, m_parent_ratio, m_mutate_probability);
                    next_generation.push_back(child);
                }

                m_population = next_generation;
            }

            const individual& front() const {
                return m_population.front();
            }

        private:
            std::vector<individual> m_population;
            std::size_t m_generation;
            std::size_t m_parent_ratio;
            std::size_t m_mutate_probability;
            std::size_t m_transfer_count; // quantidade da elite
            std::size_t m_crossover_threshold; // quantidade de pais
            std::size_t m_new_individuals_per_generation;
    };
    
}


int main(){

    int n_bits = 32;
    std::vector<std::pair<int, int>> bounds = {{-2, 2}, {-2, 2}};
    const std::size_t length_population = 100;
    const std::size_t parent_ratio = 90; 
    std::size_t mutate_probability = 10;
    std::size_t transfer_elite_ratio = 0.02;
    
    algorithm::population pop(length_population, parent_ratio, mutate_probability, transfer_elite_ratio, bounds, n_bits);
    int max_iteration = 100;
    while(max_iteration--){
        cout << pop.get_generation() << ". Generations best match: " << pop.front().get_fitness() << endl;
        pop.create_next_generation(bounds, n_bits);
        pop.sort();
    }
  
    return 0;
}
