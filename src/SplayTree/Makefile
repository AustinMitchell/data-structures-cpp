DEPENDENCIES = main.cpp SplayTree.hpp Node.hpp TreePrinter.cpp TreePrinter.hpp
SOURCE = main.cpp TreePrinter.cpp

all: $(DEPENDENCIES)
	clang++ -Wall -std=c++17 -O3 $(SOURCE)

debug: $(DEPENDENCIES)
	clang++ -Wall -std=c++17 $(SOURCE) -g -DDEBUG