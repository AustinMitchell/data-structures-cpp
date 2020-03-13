DEPENDENCIES = main.cpp splay_tree.hpp splay_tree_node.hpp TreePrinter.cpp TreePrinter.hpp
SOURCE = main.cpp TreePrinter.cpp

all: $(DEPENDENCIES)
	clang++ -pedantic -Wall -std=c++17 -O3 $(SOURCE)

debug: $(DEPENDENCIES)
	clang++ -pedantic -Wall -std=c++17 $(SOURCE) -g -DDEBUG