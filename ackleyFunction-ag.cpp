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
        private:
            void calculate_fitness(std::vector<int> x){

            }
    }
    
}

int main(){

    srand(time(NULL));

    int n_bits = 128;
    std::vector<std::pair<int, int>> bounds = {{-2, 2}, {-2, 2}, {-2, 2}};
    std::string s = algorithm::detail::get_random_string(n_bits*bounds.size());
    cout << "s: " << s << endl;
    std::vector<double> values = algorithm::detail::decode(bounds, n_bits, s); 
    
    
    cout << s << endl;
    cout << algorithm::detail::bitTointeger(s) << endl;
    for(auto v : values){
        cout << v << ' ';
    }
    cout << endl;
    
    return 0;
}