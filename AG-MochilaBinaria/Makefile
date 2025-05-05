CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra

all: main

main: genetic_algorithm.o main.o
	$(CXX) $(CXXFLAGS) $^ -o $@

genetic_algorithm.o: genetic_algorithm.cpp genetic_algorithm.hpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

main.o: main.cpp genetic_algorithm.hpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f *.o main