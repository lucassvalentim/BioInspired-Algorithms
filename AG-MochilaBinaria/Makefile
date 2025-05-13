CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra

all: mainTabela mainConvergencia

mainTabela: mainTabela.o genetic_algorithm.o
	$(CXX) $(CXXFLAGS) $^ -o $@

mainConvergencia: mainConvergencia.o genetic_algorithm.o
	$(CXX) $(CXXFLAGS) $^ -o $@

genetic_algorithm.o: genetic_algorithm.cpp genetic_algorithm.hpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

mainTabela.o: mainTabela.cpp genetic_algorithm.hpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

mainConvergencia.o: mainConvergencia.cpp genetic_algorithm.hpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f *.o mainTabela mainConvergencia
