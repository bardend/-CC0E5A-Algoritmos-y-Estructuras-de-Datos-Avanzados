#pragma once
#include <iostream>
#include "entry.hpp"
#include "utils.hpp"

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
