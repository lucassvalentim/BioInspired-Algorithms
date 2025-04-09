#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include <chrono>

const std::string target{"coding_with_thomas"};

namespace cwt{
    namespace details{
        auto get_random_char = []() -> char
        {
            const char charset[] = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOP"
            "QRSTUVWXYZ 1234567890, .-;:_!\"#%&/()=?@${[]}";
            const size_t max_index = (sizeof(charset) - 1);
            return charset[rand() % max_index];
        };

        auto get_random_string(const size_t length){
            std::string str(length, 0);
            std::generate_n(str.begin(), length, get_random_char);
            return str;
        }

        auto get_random_number(const std::size_t min, const std::size_t max){
            const std::size_t values_count = max - min + 1;
            return rand() % values_count + min;
        }
    }

    class individual{
        public:
            individual() : m_value(details::get_random_string(target.size())), m_fitness(0){
                calculate_fitness();
            }

            individual(const std::string &value) : m_value(value), m_fitness(0){
                calculate_fitness();
            }

            std::string get_value() const{
                return this->m_value;
            }

            std::size_t get_fitness() const{
                return this->m_fitness;
            }

            auto operator[](const int i) const{
                return this->m_value[i];
            }
            
            auto operator > (const individual &rhs) const {
                return (this->m_fitness > rhs.m_fitness);
            }
        private:
            // Função objetivo
            void calculate_fitness(){
                for (int i = 0 ; i < target.size() ; i++){
                    if (target[i] == m_value[i]) {
                        this->m_fitness++;
                    }
                }
            }
        
        private:
            std::string m_value;
            std::size_t m_fitness;
        
    };

    individual create_child(const individual& mother, const individual &father, const std::size_t parent_ratio, const std::size_t mutate_probability){
        std::string child_value{""};

        for(int i = 0; i < target.size(); i++){
            if(details::get_random_number(0, 100) < mutate_probability){
                child_value += details::get_random_char();
            }else if(details::get_random_number(0, 100) < parent_ratio){
                child_value += mother[i];
            }else{
                child_value += father[i];
            }
        }

        return individual{child_value};
    }

    class population{
        public:
            population(const std::size_t max_population, const std::size_t parent_ratio, std::size_t mutate_probability, const std::size_t transfer_ratio, const std::size_t crossover)
            : m_generation{1}, m_parent_ratio{parent_ratio}, m_mutate_probability{mutate_probability}{
                m_transfer_count = (transfer_ratio*max_population)/100;
                m_crossover_threshold = (crossover*max_population)/100;
                m_new_individuals_per_generation = max_population - m_transfer_count;
                m_population.reserve(max_population);

                std::generate_n(std::back_inserter(m_population), max_population, [](){return individual{};});
                this->sort();
            }
            
            std::size_t get_generation() const{
                return m_generation;
            }

            void sort(){
                std::sort(std::begin(m_population), std::end(m_population), [](const auto&left, const auto& right){return left > right;});
            }

            void create_next_generation(){
                m_generation++;
                std::vector<individual>next_generation;
                next_generation.reserve(m_population.size());
                for (std::size_t i = 0 ; i < m_transfer_count ; i++) {
                    next_generation.push_back(m_population[i]);
                }

                for (std::size_t i = 0 ; i < m_new_individuals_per_generation ; i++) {
                    individual& mother = this->m_population[details::get_random_number(0,m_crossover_threshold)];
                    individual& father = this->m_population[details::get_random_number(0,m_crossover_threshold)];
                    next_generation.push_back(create_child(mother, father, m_parent_ratio, m_mutate_probability));
                }

                m_population = next_generation;
            }

            const individual& front() const 
            {
                return m_population.front();
            }

        private:
            std::vector<individual> m_population;
            std::size_t m_generation;
            std::size_t m_parent_ratio;
            std::size_t m_mutate_probability;
            std::size_t m_transfer_count;
            std::size_t m_crossover_threshold;
            std::size_t m_new_individuals_per_generation;
    };
}

int main(){

    srand (time(NULL));
    
    const std::size_t population_size = 500;
    const std::size_t parent_ratio = 50;
    const std::size_t mutate_probability = 10;
    const std::size_t transfer_ratio = 15;
    const std::size_t crossover = 50;


    cwt::population population(population_size, parent_ratio, mutate_probability, transfer_ratio, crossover);
    
    while(population.front().get_fitness() < target.size()) 
    {
        std::cout << population.get_generation() << ". Generations best match: " << population.front().get_value() << std::endl;
        population.create_next_generation(); 
        population.sort();
    }
    
    return 0;
}