DEPENDENCIES = main.cpp ring_vector.hpp
SOURCE = main.cpp

all: $(DEPENDENCIES)
	clang++ -Wpedantic -Wall -std=c++17 -O3 $(SOURCE)

debug: $(DEPENDENCIES)
	clang++ -Wpedantic -Wall -std=c++17 $(SOURCE) -g -DDEBUG