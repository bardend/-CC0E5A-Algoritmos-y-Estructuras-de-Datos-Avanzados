#pragma once
#include <iostream>
#include "entry.hpp"

using params = MTreeParams<double, std::string, 5>;

using leaf_entry_t = leaf_entry<params>;
using internal_entry_t = internal_entry<params>;

using entry_leaf_type = std::unique_ptr<leaf_entry<params>>;
using entry_inter_type = std::unique_ptr<internal_entry<params>>;
using entry_type = std::unique_ptr<entry<params>>;

using space = metric_space<params>;
auto manhattan = space::manhattan;
auto euclidean = space::euclidean;
space manhattan_space(manhattan);
space euclidean_space(euclidean);

template <typename... args_t>
std::unique_ptr<leaf_entry_t> make_leaf(args_t&&... args) {
    return std::make_unique<leaf_entry_t>(std::forward<args_t>(args)...);
}

// crea un internal_entry
template <typename... args_t>
std::unique_ptr<internal_entry_t> make_inter(args_t&&... args) {
    return std::make_unique<internal_entry_t>(std::forward<args_t>(args)...);
}

// crea un entry
template <typename... args_t>
std::unique_ptr<entry<params>> make_entry(args_t&&... args) {
    return std::make_unique<entry<params>>(std::forward<args_t>(args)...);
}

int main() {
    ////////////////////////////////////////End_point//////////////////////////
    std::vector<double> features1 = {1.0, 2.0};
    std::vector<double> features2 = {0.0, 0.0};
    std::string xd = "xd";


    entry_type e1 = make_entry(features1, xd);
    entry_type e2 = make_entry(features2, xd); 
    ////////////////////////////////////////////////////////////////////////////


    entry_leaf_type entry_leaf = make_leaf(*e2);
    entry_inter_type entry_inter = make_inter(*e2, nullptr, 1.5);

    std::cout << e1->distance_to(*e2, manhattan_space) << std::endl;
    std::cout << entry_inter->distance_to(*e1, manhattan_space) << std::endl;
    std::cout << entry_leaf->distance_to(*e1, manhattan_space) << std::endl;
    return 0;
}
