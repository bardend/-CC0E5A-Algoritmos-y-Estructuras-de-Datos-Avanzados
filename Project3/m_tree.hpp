#pragma once
#include <memory>
#include <vector>
#include <functional>
#include <cmath>
#include <cassert>
#include "entry.hpp"

template <typename Params> class entry;

template <typename Params>
class node {
public:
    using node_ptr = std::unique_ptr<node<Params>>;
    using entry_ptr = std::unique_ptr<entry<Params>>;
    static constexpr int capacity =  Params::cap_node;

private:
    struct base_fields {
        bool is_leaf;
        int size;
        node_ptr father;
    };
    base_fields fields_;
    std::vector<entry_ptr> entries_;
    int size_entry_;

public:
    ~node() = default;
    void set_leaf(bool n_leaf = false) noexcept { fields_.is_leaf = n_leaf;}
    explicit node(bool leaf = true) : fields_{leaf, 0, nullptr}, size_entry_{0}, entries_(capacity) {}
    bool is_leaf() const noexcept { return fields_.is_leaf; } // 'noexcept' para optimización
    bool is_overflow() const noexcept { return size_entry_ > capacity;}
    entry_ptr get_entry(int i) { assert(i >= 0 and i < size_entry_);  return entries_[i]; }
    //
    // void insert_entry(entry_ptr& entry) {
    //     entries_[size_entry_++] = (is_leaf() ? entry, 
    //
    //     entries_[size_entry_++] = (is_leaf() ? 
    // }
};

