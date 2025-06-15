#ifndef M_TREE_HPP
#define M_TREE_HPP
#include <memory>
#include <vector>
#include <functional>
#include <cmath>
#include <cassert>
#include <vector>
#include "entry.hpp"

template <typename Params> class entry;

template <typename Params>
class node {
public:
    using node_ptr = std::unique_ptr<node<Params>>;
    using entry_ptr = std::unique_ptr<entry<Params>>;
    static constexpr int capacity =  Params::cap_node;
  	typedef Params params_type;


private:
    struct base_fields {
        bool is_leaf;
        int size;
        node_ptr father;
    };
    base_fields fields_;
    std::vector<entry_ptr> entries_;
    std::vector<node_ptr> childrens_;
    int size_entry_;

public:
    ~node() = default;
    void set_leaf(bool n_leaf = false) noexcept { fields_.is_leaf = n_leaf;}
    explicit node(bool leaf = true) : fields_{leaf, 0, nullptr},
                                      size_entry_{0}, entries_(capacity + 1), childrens_(capacity+1) {}
    bool is_leaf() const noexcept { return fields_.is_leaf; } // 'noexcept' para optimización
    bool is_overflow() const noexcept { return size_entry_ > capacity;}
    bool empty() const noexcept {return size_entry_ == 0;}
    int size_entry() const noexcept { return size_entry_;}

    entry_ptr get_entry(int i) { assert(i >= 0 and i < size_entry_);  return entries_[i]; }
    node_ptr cover_tree(int i) { assert(i>= 0 and i < size_entry_); return entries_[i]->cover_tree;}
    node_ptr get_father() { return fields_.father;}
    //void update_covert_tree() {} estos son solo swaps

    void insert_entry(entry_ptr& entry) {
        entries_[size_entry_++] = (is_leaf() ? make_leaf(*entry) : make_inter(*entry, nullptr, 1.5));
    }

    void swap_entry(int i, node_ptr other, int j){
        assert(other != this and i!=j);
        params_type::swap(entries_[i], other->entries_[j]);
    }
    void swap_entry(int i, node_ptr other) {
        assert(other != this);
        params_type::swap(entries_[i], other->entries_[i]);
    }
    void swap_entry(int i, int j) {
        assert(i != j);
        params_type::swap(entries_[i], entries_[j]);
    }

    // Move entry i in this node to entry j in node other
    // No estamos cambiando el puntero solo el objeto
    void move_entry(int i, node_ptr other, int j) {
        assert(other != this);
        other->entries_[j] = std::move(fields_.values[i]);
    }
    void move_entry(int i, node_ptr other) {
        move_entry(i, other, i);
    }
    void split_afer_promote(const std::vector<entry_ptr>& p1, const std::vector<entry_ptr>&p2);

};

//m_tree methods
template <typename Params>
// vector de indices
void node<Params>::split_afer_promote(const std::vector<entry_ptr>&ids1,const std::vector<entry_ptr>&ids2){
    int cnt = size_entry();
    //Make sure p1, p2 are partition of entries of this node;
    assert((int)ids1.size() + (int)ids2.size() == cnt); 
    //Here I leave with p1 partion and create a new node for p2;
    auto new_sibling = make_node(is_leaf());
    for(int i = 0; i < (int)ids2.size(); i++) 
        this->move_entry(ids2[i], new_sibling, i);
}
    
#endif // M_TREE_HPP
