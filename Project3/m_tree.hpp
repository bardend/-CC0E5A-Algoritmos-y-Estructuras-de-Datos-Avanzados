#ifndef M_TREE_HPP
#define M_TREE_HPP
#include <memory>
#include <vector>
#include <functional>
#include <cmath>
#include <cassert>
#include <vector>
#include <algorithm>

#include "entry.hpp"
#include "utils.hpp"

template <typename Params> class entry;

template <typename Params>
class node : public std::enable_shared_from_this<node<Params>> {
public:
    using node_ptr = std::shared_ptr<node<Params>>;
    using entry_ptr = std::shared_ptr<entry<Params>>;
    using entry_inter_ptr = std::shared_ptr<internal_entry<Params>>;
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
    int current_pos_entry_;
    entry_ptr father_entry_;

public:
    ~node() = default;
    void set_leaf(bool n_leaf = false) noexcept { fields_.is_leaf = n_leaf;}
    explicit node(bool leaf = true) : fields_{leaf, 0, nullptr},
             current_pos_entry_{0},size_entry_{0}, entries_(capacity + 1), childrens_(capacity+1) {}

    bool is_leaf() const noexcept { return fields_.is_leaf; } // 'noexcept' para optimización
    bool is_overflow() const noexcept { return size_entry_ > capacity;}
    bool empty() const noexcept {return size_entry_ == 0;}
    int size_entry() const noexcept { return size_entry_;}

    entry_ptr& get_entry(int i) { assert(i >= 0 and i < capacity+1);  return entries_[i]; }
    node_ptr& cover_tree(int i) { 
        std::cout << "i :" << i << std:: endl;
        assert(i>= 0 and i < capacity+1); 
        return entries_[i]->cover_tree;
    }

    // Actualiza el cover_tree del entry_x
    //void set_cover_tree(int pos, node_ptr& x){ entries_[pos]->set_cover_tree(x);}
    void set_father_node(node_ptr& x) {fields_.father = x;}
    void set_father_entry(entry_ptr& entry) { father_entry_ = entry;}
    void set_father_entry(entry_inter_ptr& entry) { father_entry_ = entry;}

    node_ptr& get_father_node() { return fields_.father;}
    entry_ptr& get_father_entry() { return father_entry_;}
    int get_pos_entry(int i) { return entries_[i]->get_pos();}


    int update_state_insert() {
        size_entry_++;
        entries_[current_pos_entry_]->set_pos(current_pos_entry_);
        int ret = current_pos_entry_;
        for(int i = current_pos_entry_+1; i < capacity +1; i++) 
            if(!entries_[i]) {
                current_pos_entry_ = i; 
                break;
            }
        return ret;
    }
    
    int insert_entry(entry_inter_ptr& entry) {
        entries_[current_pos_entry_] = entry;
        return update_state_insert();
    }

    int insert_entry(entry_ptr& entry) {
        if(is_leaf()) entries_[current_pos_entry_] = make_leaf(*entry);
        else entries_[current_pos_entry_] = make_inter(*entry, nullptr, 1.5);
        return update_state_insert();
    }

    void remove_entry(int pos) {
        entries_[pos].reset();
        current_pos_entry_ = std::min(current_pos_entry_, pos);
        size_entry_--;
    }

    // Mi i por j del otro

    void swap_entry(int i, node_ptr& other, int j){
        //assert(other != this and i!=j);
        params_type::swap(entries_[i], other->entries_[j]);
        entries_[i]->set_pos(i);
        other->entries_[j]->set_pos(j);
    }
    void swap_entry(int i, node_ptr& other) {
        //assert(other != this);
        params_type::swap(entries_[i], other->entries_[i]);
    }
    void swap_entry(int i, int j) {
        assert(i != j);
        params_type::swap(entries_[i], entries_[j]);
        entries_[i]->set_pos(j);
        entries_[j]->set_pos(i);
    }

    // Move entry i in this node to entry j in node other
    // No estamos cambiando el puntero 
    void move_entry(int i, node_ptr& other, int j) {
        //assert(other != this);
        other->entries_[j] = entries_[i];
        entries_[i].reset();
        other->entries_[j]->set_pos(j);
        other->size_entry_+= 1;

        current_pos_entry_ = std::min(current_pos_entry_, i);
        size_entry_--;
        std::cout << "size: " << size_entry_ << std::endl;
    }

    void move_entry(int i, node_ptr other) {
        move_entry(i, other, i);
    }

    void print_node() const {

        std::cout << "======================================================" << std::endl;
        std::cout << "Node-Leaf: " << (is_leaf() ? "Yes" : "No")  << ", Size: " << size_entry_ << std::endl;
        std::cout << "El num entries: " << size_entry_ << std::endl;
        for(int i = 0; i < capacity + 1; i++) 
            if(entries_[i]) std::cout << "[" << i << "]: " << entries_[i]->oid << " " << entries_[i]->get_pos() << "      ";
            else std::cout << "[" << i << "]: empty ";
        std::cout << std::endl << std::endl;
    }

    std::pair<node_ptr, node_ptr> split_after_promote(int ind_o1, int ind_o2);
};


template <typename Params>
auto node<Params>::split_after_promote(int ind_o1, int ind_o2) -> std::pair<node_ptr, node_ptr> {

    int n = size_entry();
    int mid = n/2;
    std::vector<int>ids1; 
    std::vector<int>ids2;
    for(int i = 0; i < mid; i++) ids1.push_back(i);
    for(int i = mid; i < n; i++) ids2.push_back(i);

    //Make sure p1, p2 are partition of entries of this node;
    assert((int)ids1.size() + (int)ids2.size() == n); 
    //Here I leave with p1 partion and create a new node for p2;
    auto new_sibling = std::make_shared<node<Params>>(is_leaf());

    for(int i = 0; i < (int)ids2.size(); i++) 
        this->move_entry(ids2[i], new_sibling, i);

    // Usar this-> para acceder al método de la clase base
    return std::make_pair(this->shared_from_this(), new_sibling);
}

template <typename Params>
class m_tree {
public:
    using node_ptr = std::shared_ptr<node<Params>>;
    using entry_ptr = std::shared_ptr<entry<Params>>;
    static constexpr int capacity =  Params::cap_node;

private:
    node_ptr root_;
    int size_nodes_;
    int size_entries_;

public:
     explicit m_tree(): size_nodes_{0}, size_entries_{0}{}

    ~m_tree() = default;
    int size_nodes() const noexcept { return size_nodes_; }
    void clear() noexcept { root_.reset(); size_nodes_ = 0; size_entries_ =0; }

    void promote(node_ptr& node);
    bool keep_overflow(node_ptr node);

    bool insert(entry_ptr& new_entry) {  
        if(!root_) {
            root_ = std::make_shared<node<Params>>(true);
            root_->insert_entry(new_entry);
            size_nodes_ = size_entries_ = 1;
            std::cout << "Insertamos" << std:: endl;
            return true;
        }
        //find :)
        root_->insert_entry(new_entry);
        return keep_overflow(root_);
    }

    std::pair<int, int>pick_promoters(node_ptr& node);
    void relate(node_ptr& pa, int pos, node_ptr& child);
    //std::vector<int>& ids1, std::vector<int>& ids2) 
    //
    void print_tree() const noexcept;
    void print_tree(node_ptr node, int depth) const noexcept;

};

template <typename Params>
auto m_tree<Params>::keep_overflow(node_ptr node) ->bool{ 

    while(node->get_father_node() and node->is_overflow()) {

        node->get_father_node()->remove_entry(node->get_father_entry()->get_pos());
        auto [o1, o2] = pick_promoters(node);

        auto oi = make_inter(*node->get_entry(o1), nullptr, 1.5);
        auto oj = make_inter(*node->get_entry(o2), nullptr, 2.5);

        int pos1 = node->get_father_node()->insert_entry(oi);
        int pos2 = node->get_father_node()->insert_entry(oj);
        auto [l, r] = node->split_after_promote(o1, o2);

        relate(node->get_father_node(), pos1, l);
        relate(node->get_father_node(), pos2, r);
        node = node->get_father_node();
    }

    if(not (node->is_overflow())) return true;
    auto new_root_ = make_node(false);

    auto [o1, o2] = pick_promoters(node);

    auto oi = make_inter(*node->get_entry(o1), nullptr, 1.5);
    auto oj = make_inter(*node->get_entry(o2), nullptr, 2.5);

    int pos1 = new_root_->insert_entry(oi);
    int pos2 = new_root_->insert_entry(oj);
    auto [l, r] = node->split_after_promote(o1, o2);
    relate(new_root_, pos1, l);
    relate(new_root_, pos2, r);
    root_ = new_root_;
    size_nodes_ += 2;  // Añadimos 2 nuevos nodos (l y r)

    return true;
}

template <typename Params>
auto m_tree<Params>::relate(node_ptr& pa, int pos, node_ptr& child) -> void {
    //pa->set_cover_tree(pos, child);
    child->set_father_node(pa);
    child->set_father_entry(pa->get_entry(pos)); // Asumiendo que necesitas esto
}

template <typename Params>
auto m_tree<Params>::pick_promoters(node_ptr& node) ->  std::pair<int, int>{
    //falta una heuristica para definir los promotores :)
    int oi = 0, oj = node->size_entry()-1;
    return std::make_pair(oi, oj);
}


template <typename Params>
void m_tree<Params>::print_tree(node_ptr node, int depth) const noexcept {
    if (!node) return;

    std::string indent(depth * 4, ' ');
    std::cout << indent << "[";

    // Imprimir oids de las entradas del nodo
    for (int i = 0; i < Params::cap_node + 1; ++i) {
        auto entry = node->get_entry(i);
        if (entry) {
            std::cout << entry->oid << " ";
        }
    }
    std::cout << "]\n";

   // entry_ptr& get_entry(int i) { assert(i >= 0 and i < capacity+1);  return entries_[i]; }
   //  node_ptr& cover_tree(int i) { assert(i>= 0 and i < capacity+1); return entries_[i]->cover_tree;}

    // if (!node->is_leaf()) {
    //     for (int i = 0; i < Params::cap_node + 1; ++i) {
    //         auto entry = node->get_entry(i);
    //         if (entry && node->cover_tree(i)) {
    //             print_tree(node->cover_tree(i), depth + 1);
    //         }
    //     }
    // }
}

template <typename Params>
void m_tree<Params>::print_tree() const noexcept {
    print_tree(root_, 0);
}

#endif // M_TREE_HPP
