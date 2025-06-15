#pragma once
#include <iostream>
#include "entry.hpp"

    
using params = MTreeParams<double, std::string, 5>;

using LeafEntry = leaf_entry<params>;
using entry_leaf_type = std::unique_ptr<leaf_entry<params>>;
using entry_inter_type = std::unique_ptr<internal_entry<params>>;


using space = metric_space<params>;
auto manhattan = space::manhattan;
auto euclidean = space::euclidean;
space manhattan_space(manhattan);
space euclidean_space(euclidean);

int main() {
    std::vector<double> features1 = {1.0, 2.0};
    std::vector<double> features2 = {0.0, 0.0};
    std::string xd = "xd";

    auto make_leaf = [](auto&&... args) {
        return std::make_unique<LeafEntry>(std::forward<decltype(args)>(args)...);
    };

    auto e1 = std::make_unique<entry<params>>(features1, xd);

    auto ee = make_leaf(*e1);
    std::unique_ptr<entry<params>> e2 = std::make_unique<entry<params>>(features2, xd);
    std::cout << e1->distance_to(*e2, manhattan_space) << std:: endl;

    entry_leaf_type entry_leaf = make_leaf(*e2);
    entry_inter_type entry_inter = std::make_unique<internal_entry<params>>(*e2, nullptr, 1.5);

    std::cout << entry_inter->distance_to(*e1, manhattan_space) << std::endl;
    std::cout << entry_leaf->distance_to(*e1, manhattan_space) << std::endl;

    return 0;
}
