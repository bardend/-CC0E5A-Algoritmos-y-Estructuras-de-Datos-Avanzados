//main.cpp
#include <iostream>
#include <random>
#include <fstream>
#include <sstream>
#include <iomanip>  // Necesario para std::fixed y std::setprecision

#include "m_tree.h"

using tree_t = m_tree<MTreeParams<double, std::string, 10>>;

// Función make_tree trasladada desde utils.hpp
template <typename... args_t>
std::shared_ptr<tree_t> make_tree(args_t&&... args) {
    return std::make_shared<tree_t>(std::forward<args_t>(args)...);
}

int main() {
    std::vector<double>v = {1.2, 2.0};
    std::string st = "hola";
    entry_type e12 = make_entry(v, st);
    std::cout << e12->oid << std::endl;
    auto tree = make_tree();
}
