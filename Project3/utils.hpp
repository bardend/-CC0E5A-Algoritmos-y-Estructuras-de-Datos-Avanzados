#ifndef UTILS_HPP
#define UTILS_HPP
#include <memory>
#include "entry.hpp"

using params = MTreeParams<double, std::string, 3>;
using leaf_entry_t = leaf_entry<params>;
using internal_entry_t = internal_entry<params>;
using node_t = node<params>;
using entry_leaf_type = std::shared_ptr<leaf_entry<params>>;
using entry_inter_type = std::shared_ptr<internal_entry<params>>;
using entry_type = std::shared_ptr<entry<params>>;
using entry_share_type = std::shared_ptr<entry<params>>;
using space = metric_space<params>;



auto manhattan = space::manhattan;
auto euclidean = space::euclidean;
space manhattan_space(manhattan);
space euclidean_space(euclidean);

template <typename... args_t>
std::shared_ptr<leaf_entry_t> make_leaf(args_t&&... args) {
    return std::make_shared<leaf_entry_t>(std::forward<args_t>(args)...);
}

template <typename... args_t>
std::shared_ptr<internal_entry_t> make_inter(args_t&&... args) {
    return std::make_shared<internal_entry_t>(std::forward<args_t>(args)...);
}

template <typename... args_t>
std::shared_ptr<entry<params>> make_entry(args_t&&... args) {
    return std::make_shared<entry<params>>(std::forward<args_t>(args)...);
}

template <typename... args_t>
std::shared_ptr<node_t> make_node(args_t&&... args) {
    return std::make_shared<node_t>(std::forward<args_t>(args)...);
}


#endif // UTILS_HPP
