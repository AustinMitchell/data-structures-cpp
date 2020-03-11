DEPENDENCIES = main.cpp ring_vector.hpp
SOURCE = main.cpp

all: $(DEPENDENCIES)
	clang++ -pedantic -Wall -std=c++17 -O3 $(SOURCE)

debug: $(DEPENDENCIES)
	clang++ -pedantic -Wall -std=c++17 $(SOURCE) -g -DDEBUG