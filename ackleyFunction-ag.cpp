#include <bits/stdc++.h>
using std::cout;
using std::endl;

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
            const size_t max_index = (sizeof(charset) - 1);
            return charset[rand() % max_index];
        };

        auto get_random_string(const size_t length){
            std::string str(length, 0);
            std::generate_n(str.begin(), length, get_random_bit);
            return str;
        }

        std::vector<double> decode(std::vector<std::pair<int, int>> &bounds, std::size_t n_bits, std::string bitstring){
            std::vector<double> decoded(bounds.size());
            double largest = pow(2, n_bits);
            for(int i = 0; i < bounds.size(); i++){ 
                std::string substring = "";
                int start = i * n_bits;
                int end = (i * n_bits) + n_bits;
                for(int j = start; j < end; j++){
                    substring += bitstring[j];
                }
                cout << "substring: " << substring << endl;

                double integer = bitTointeger(substring);
                cout << "largest: " << largest << endl;
                cout << "integer: " << integer << endl;
                cout << "inter/largest: " << (integer/largest) << endl;
                double value = bounds[i].first + (integer/largest) * (bounds[i].second - bounds[i].first);

                decoded[i] = value;
            }

            return decoded;
        }
    } // namespace detail

    class individual {
        public:
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
            
                this->m_fitness = -20 * exp(-0.2 * sqrt(somatorio1 / n)) 
                        - exp(somatorio2 / n) 
                        + 20 + exp(1);
            }
        
        
        private:
            std::string m_value;
            double m_fitness;
    };

    individual create_child(const individual &p1, const individual &p2, std::vector<std::pair<int, int>> &bound, ::size_t n_bits){
        std::string c1{""};

        std::size_t length = p2.get_value().size();
        std::size_t index = (rand() % (length - 2)) + 1;

        cout << "index: " << index << endl;

        for(int i = 0; i < index; i++){
            c1 += p1.get_value()[i];
        }

        for(int i = index; i < length; i++){
            c1 += p2.get_value()[i];
        }

        return individual{c1, bound, n_bits};
    }

    class population{
        public:
            population(const std::size_t length_population, const std::size_t parent_ratio, std::size_t mutate_probability, std::size_t transfer_elite_ratio, std::vector<std::pair<int, int>> &bound, ::size_t n_bits)
            : m_generation{1}, m_parent_ratio{parent_ratio}, m_mutate_probability{mutate_probability}{

                m_transfer_count = (transfer_elite_ratio*length_population);
                m_new_individuals_per_generation = length_population - m_transfer_count;
                m_population.reserve(length_population);

                std::generate_n(std::back_inserter(m_population), length_population, [&](){return individual(bound, n_bits);});
                    
            }

            std::vector<individual> get_pop(){
                return m_population;
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

    srand(time(NULL));

    int n_bits = 32;
    std::vector<std::pair<int, int>> bounds = {{-10, 10}, {-10, 10}};
    const std::size_t length_population = 100;
    const std::size_t parent_ratio = 0.9; 
    std::size_t mutate_probability = 0.5;
    std::size_t transfer_elite_ratio = 0.02;
    algorithm::population pop(length_population, parent_ratio, mutate_probability, transfer_elite_ratio, bounds, n_bits);

    for(algorithm::individual inv : pop.get_pop())
        cout << inv.get_value() << " " << inv.get_fitness() << endl;
    return 0;
}