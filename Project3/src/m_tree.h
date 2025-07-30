#ifndef M_TREE_H
#define M_TREE_H
#include <memory>
#include <vector>
#include <functional>
#include <cmath>
#include <cassert>
#include <vector>
#include <limits>
#include <algorithm>
#include <queue>
#include <iostream>
#include <random>
#include "entry.h"
#include "utils.h"

template <typename Params> class entry;
template <typename Params> class heuristic_split;

template <typename Params>
class node : public std::enable_shared_from_this<node<Params>> {
public:
    using node_ptr = std::shared_ptr<node<Params>>;
    using entry_ptr = std::shared_ptr<entry<Params>>;
    static constexpr int capacity =  Params::cap_node;
  	typedef Params params_type;
    using distance_type = typename Params::feature_type;
    using heuristic_split_f = heuristic_split<Params>;

private:
    struct base_fields {
        bool is_leaf;
        int size;
        node_ptr father;
    };
    base_fields fields_;
    std::vector<entry_ptr> entries_;
    int size_entry_;
    int current_pos_entry_;
    entry_ptr father_entry_;

public:
    ~node() = default;
    void set_leaf(bool n_leaf = false) noexcept;
    explicit node(bool leaf = true);

    bool is_leaf() const noexcept;
    bool is_overflow() const noexcept;
    bool empty() const noexcept;
    int size_entry() const noexcept;

    const entry_ptr& get_entry(int i) const;
    node_ptr cover_tree(int pos) const;
    void set_cover_tree(int pos, node_ptr new_cover_tree);
    entry_ptr get_father_entry();
    void set_father_entry(entry_ptr entry);
    void set_father_node(node_ptr x);
    node_ptr get_father_node();
    int get_pos_entry(int i);
    int insert_entry(entry_ptr entry);
    int update_state_insert();
    void remove_entry(int pos);
    void swap_entry(int i, node_ptr& other, int j);
    void swap_entry(int i, node_ptr& other);
    void swap_entry(int i, int j);
    void move_entry(int i, node_ptr&other);
    std::pair<int, int> pick_promoters(const heuristic_split_f& heuristic = heuristic_split_f()) const;
    void make_partition_afetr_split(int pos, std::vector<int>partition);
};

template <typename Params>
class heuristic_split {
public:
    using feature_type = typename Params::feature_type;
    using distance_type = typename Params::feature_type;
    using feature_vector = std::vector<feature_type>;
    using node_ptr = std::shared_ptr<node<Params>>;
    using entry_ptr = std::shared_ptr<entry<Params>>;
    static constexpr int capacity =  Params::cap_node;

    using heuristic_function = std::function<std::pair<int,int>(const std::shared_ptr<const node<Params>>& node)>;
private:
    heuristic_function heuristic;

public:
    static std::pair<int, int> random_max_dis(const std::shared_ptr<const node<Params>>& node);
    static std::pair<int, int> pair_max_dis(const std::shared_ptr<const node<Params>>& node);
    heuristic_split(heuristic_function func = pair_max_dis);
    std::pair<int, int> operator()(const std::shared_ptr<const node<Params>>& node) const;
};

template <typename Params>
class m_tree {
public:
    using node_ptr = std::shared_ptr<node<Params>>;
    using entry_ptr = std::shared_ptr<entry<Params>>;
    static constexpr int capacity =  Params::cap_node;
    using distance_type = typename Params::feature_type;
    using identifier_type = typename Params::identifier_type;

private:
    node_ptr root_;
    int size_nodes_;
    int size_entries_;
    int num_entries_promoters = 2;

public:
     explicit m_tree();
    ~m_tree() = default;
    int size_nodes() const noexcept;
    void clear() noexcept;
    node_ptr get_root() const;
    void keep_overflow(node_ptr node, std::vector<int>&v);
    void insert(entry_ptr new_entry);
    node_ptr find_leaf(entry_ptr entry, node_ptr curr_node);
    node_ptr promote(node_ptr node, std::vector<int>&v);
    void split(node_ptr& node, std::vector<int>&v);
    void keep_invariant(node_ptr node, std::vector<int>&v);
    std::vector<identifier_type> k_nn_entries(entry_ptr query, int k);
    
};

#endif // M_TREE_H
