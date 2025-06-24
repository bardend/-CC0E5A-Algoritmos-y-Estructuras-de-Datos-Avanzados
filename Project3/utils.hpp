#ifndef UTILS_HPP
#define UTILS_HPP
#include <memory>
#include "entry.hpp"

using params = MTreeParams<double, std::string, 3>;
using entry_t = entry<params>;
using node_t = node<params>;
using entry_type = std::shared_ptr<entry<params>>;
using space = metric_space<params>;



auto manhattan = space::manhattan;
auto euclidean = space::euclidean;
space manhattan_space(manhattan);
space euclidean_space(euclidean);

// Función para crear un entry básico (leaf entry)
template <typename... args_t>
std::shared_ptr<entry_t> make_entry(args_t&&... args) {
    return std::make_shared<entry_t>(std::forward<args_t>(args)...);
}

// Función para crear un leaf entry (copia de un entry existente)
template <typename... args_t>
std::shared_ptr<entry_t> make_leaf(args_t&&... args) {
    return std::make_shared<entry_t>(std::forward<args_t>(args)...);
}

// Función para crear un internal entry
template <typename... args_t>
std::shared_ptr<entry_t> make_inter(args_t&&... args) {
    return std::make_shared<entry_t>(std::forward<args_t>(args)...);
}

// Función para crear un nodo
template <typename... args_t>
std::shared_ptr<node_t> make_node(args_t&&... args) {
    return std::make_shared<node_t>(std::forward<args_t>(args)...);
}


#endif // UTILS_HPP
