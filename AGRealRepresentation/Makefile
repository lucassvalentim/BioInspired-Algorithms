CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra

all: ga_test

ga_test: genetic_algorithm.o test_ga.o
	$(CXX) $(CXXFLAGS) $^ -o $@

genetic_algorithm.o: genetic_algorithm.cpp genetic_algorithm.hpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

test_ga.o: test_ga.cpp genetic_algorithm.hpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f *.o ga_test

.PHONY: all clean