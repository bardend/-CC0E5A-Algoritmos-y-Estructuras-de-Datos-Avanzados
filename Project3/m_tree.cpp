//m_tree.cpp
#include <iostream>
#include "entry.hpp"
#include "utils.hpp"
#include "m_tree.hpp"


using tree_t = m_tree<params>;

// Función make_tree trasladada desde utils.hpp
template <typename... args_t>
std::shared_ptr<tree_t> make_tree(args_t&&... args) {
    return std::make_shared<tree_t>(std::forward<args_t>(args)...);
}

int main() {
    ////////////////////////////////////////End_point//////////////////////////
   
    // std::vector<double> features1 = {1.0, 2.0};
    // std::vector<double> features2 = {0.0, 0.0};
    // std::string xd = "xd";
    // entry_type e1 = make_entry(features1, "1");
    // entry_type e2 = make_entry(features2, "2"); 
    // entry_type e3 = make_entry(features1, "3");
    // entry_type e4 = make_entry(features1, "4");
    // std::cout << e2->oid << std::endl;
    // ////////////////////////////////////////////////////////////////////////////
    // // std::cout << e1->distance_to(*e2, manhattan_space) << std::endl;
    // // std::cout << entry_inter->distance_to(*e1, manhattan_space) << std::endl;
    // // std::cout << entry_leaf->distance_to(*e1, manhattan_space) << std::endl;
    //
    // // Crear nodos usando shared_ptr desde el inicio
    // auto leaf_node = make_node(true);
    // leaf_node->print_node();
    // leaf_node->insert_entry(e1);
    // leaf_node->print_node();
    // leaf_node->insert_entry(e2);
    // leaf_node->print_node();
    // leaf_node->insert_entry(e3);
    // leaf_node->print_node();
    //
    // std::cout << "-ANOTHER" << std::endl;
    // auto other = make_node(true);
    // leaf_node->move_entry(2, other, 3);
    // other->insert_entry(e4);
    // other->print_node();
    // other->swap_entry(0, leaf_node, 1);
    // other->print_node();
    // other->remove_entry(0);
    // other->print_node();
    
    std::vector<double> features1 = {1.0, 1.0};
    std::vector<double> features2 = {2.0, 2.0};
    std::vector<double> features3 = {4, 6};
    std::vector<double> features4 = {4, 7};
    std::vector<double> features5 = {1, 8};
    std::vector<double> features6 = {2, 6.0};
    std::vector<double> features7 = {1, 0};
    std::vector<double> features8 = {0, 2};
    std::vector<double> features9 = {3, 2.0};

    std::vector<double> features10 = {0, 0.0};

    auto tree = make_tree();
    entry_type e1 = make_entry(features1, "0");
    entry_type e2 = make_entry(features2, "1");
    entry_type e3 = make_entry(features3,"2");
    entry_type e4 = make_entry(features4, "3");
    entry_type e5 = make_entry(features5, "4");
    entry_type e6 = make_entry(features6, "5");
    entry_type e7 = make_entry(features7, "6");
    entry_type e8 = make_entry(features8, "7");
    entry_type e9 = make_entry(features9, "8");
    entry_type e10 = make_entry(features10, "10");

    tree->insert(e1);
    tree->insert(e2);
    tree->insert(e3);
    tree->print_tree();

    tree->insert(e4);
    tree->print_tree();

    tree->insert(e5);
    tree->print_tree();

    tree->insert(e6);
    tree->print_tree();

    tree->insert(e7);
    tree->print_tree();

    tree->insert(e8);
    tree->print_tree();

    tree->insert(e9);
    tree->print_tree();

    return 0;
}
