#include <iostream>

#include <dsc/splay_tree.hpp>
#include <dsc/tree_printer.hpp>

using std::cout, std::cin;


template<typename splay_type>
auto builder() -> void {
    auto tree   = dsc::splay_tree<int, splay_type>{};
    auto input  = 0;
    auto type   = 0;
    while (true) {
        cout << "Operation: (1) insert, (2) find: ";
        cin >> type;
        cout << "\n";

        cout << "\n";
        cout << "Value: ";
        cin >> input;
        cout << "\n";
        cout << "\n";


        if (type == 1){
            tree.insert(input);
        } else if (type == 2) {
            tree.contains(input);
        }
        dsc::print_ascii_tree(tree.root());
    }
}


int main() {
    auto type = 0;

    cout << "Select splay_tree type: (1) full splay, (2) semi splay: ";
    cin >> type;
    cout << "\n";

    if (type == 1) {
        builder<dsc::fullsplay>();
    } else if (type == 2) {
        builder<dsc::semisplay>();
    }
}