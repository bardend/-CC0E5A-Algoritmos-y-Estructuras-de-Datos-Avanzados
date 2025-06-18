#ifndef M_TREE_HPP
#define M_TREE_HPP
#include <memory>
#include <vector>
#include <functional>
#include <cmath>
#include <cassert>
#include <vector>
#include <limits>
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
    using distance_type = typename Params::feature_type; // Retorna lo mismo que los features :)

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

    entry_ptr& get_entry(int i) {
        assert(i >= 0 and i < capacity+1);
        return entries_[i];
    }

    node_ptr cover_tree(int pos) { 
        assert(pos>= 0 and pos < capacity+1); 
        if (auto* internal = dynamic_cast<internal_entry<Params>*>(entries_[pos].get())) 
            return entries_[pos]->get_cover_tree();
        else {
            assert(false && "No deberia llegar aqui");
            return nullptr;
        }
    }
    // Setea el entry de la posicion (pos) un nuevo cover_tree
    //pa->set_cover_tree(pos, child);
    void set_cover_tree(int pos, node_ptr new_cover_tree) {
        if (auto* internal = dynamic_cast<internal_entry<Params>*>(entries_[pos].get())) 
            entries_[pos]->set_cover_tree(new_cover_tree);
        else assert(false && "No deberia llegar aqui");
    }

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
        std::cout << "Mi nuevo current_pos_entry :" << current_pos_entry_ << std::endl;
        return ret;
    }
    
    int insert_entry(entry_inter_ptr& entry) {

        for(int i = current_pos_entry_; i <= capacity; i++)
            if(!entries_[i]) {
                current_pos_entry_ = i;
                break;
            }

        std::cout << "Inserta inter-entry en : " << current_pos_entry_ << std::endl; 
        entries_[current_pos_entry_] = entry;
        return update_state_insert();
    }

    int insert_entry(entry_ptr& entry) {
        for(int i = current_pos_entry_; i <= capacity; i++)
            if(!entries_[i]) {
                current_pos_entry_ = i;
                break;
            }

        std::cout << "Inserta entry en : " << current_pos_entry_ << std::endl; 
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

    node_ptr split_after_promote(int id, std::vector<int>partition, int& x);
};


template<typename Params>
auto node<Params>::split_after_promote(int ind_o1, std::vector<int>partition, int& x) -> node_ptr {

    std::cout << "Llegamos a split_after_promote" << std::endl;
    std::cout << "ind_o1: "<< ind_o1 << std::endl;
    for(auto e: partition) 
        std::cout << e << " ";
    std::cout<<std::endl;
    auto new_sibling = std::make_shared<node<Params>>(is_leaf());
    for(int i = 0; i < (int)partition.size(); i++) {
        this->move_entry(partition[i], new_sibling, i);
        if(ind_o1 == partition[i]) {
            std::cout << "La posicion agregada en el nuevo ptr es :" << i << std::endl;
            std::cout << "Reafirmamos :" << new_sibling->get_entry(i)->get_pos() << std::endl;
            x = new_sibling->get_entry(i)->get_pos();
            assert(x==i);
        }
    }

    return new_sibling;
}

template <typename Params>
class m_tree {
public:
    using node_ptr = std::shared_ptr<node<Params>>;
    using entry_ptr = std::shared_ptr<entry<Params>>;
    static constexpr int capacity =  Params::cap_node;
    using distance_type = typename Params::feature_type; // Retorna lo mismo que los features :)

private:
    node_ptr root_;
    int size_nodes_;
    int size_entries_;

public:
     explicit m_tree(): size_nodes_{0}, size_entries_{0}{}

    ~m_tree() = default;
    int size_nodes() const noexcept { return size_nodes_; }
    void clear() noexcept { root_.reset(); size_nodes_ = 0; size_entries_ =0; }

    bool keep_overflow(node_ptr node);

    bool insert(entry_ptr& new_entry) {  
        if(!root_) {
            root_ = std::make_shared<node<Params>>(true);
            root_->insert_entry(new_entry);
            size_nodes_ = size_entries_ = 1;
            return true;
        }

        auto current_leaf = find_leaf(new_entry, root_);
        std::cout << "---------------------------NEW_ENTRY--------------------" << std::endl;
        std::cout << new_entry->oid << std::endl;
        std::cout << "------------------------------------LEAF---------------" << std::endl;
        current_leaf->print_node();

        std::cout << "----------------------------------AFTER-LEAF---------------" << std::endl;
        current_leaf->insert_entry(new_entry);
        current_leaf->print_node();

        size_entries_ += 1;
        return keep_overflow(current_leaf);
    }

    node_ptr find_leaf(entry_ptr entry, node_ptr curr_node);
    void relate(node_ptr& pa, int pos, node_ptr& child);
    void print_tree() const noexcept;
    void print_tree(node_ptr node, int depth) const noexcept;
    node_ptr promote(node_ptr node);
    std::pair<std::vector<node_ptr>, std::vector<entry_ptr>> pick_promoters(node_ptr& node);
};

template <typename Params>
auto m_tree<Params>::find_leaf(entry_ptr new_entry, node_ptr curr_node) -> node_ptr {

    if (curr_node->is_leaf()) {
        return curr_node;
    }

    node_ptr best_candidate = nullptr;
    distance_type best_distance = std::numeric_limits<distance_type>::max();

    node_ptr fallback_candidate = nullptr;
    distance_type min_radius_increase = std::numeric_limits<distance_type>::max();

    for (int i = 0; i < capacity; ++i) {
        auto e = curr_node->get_entry(i);
        if (!e) continue;

        distance_type d = e->distance_to(*new_entry);

        if (d <= e->get_cover_radius()) {
            // Este candidato ya cubre al nuevo
            if (d < best_distance) {
                best_distance = d;
                best_candidate = e->get_cover_tree();
            }
        } else {
            // No cubre, calcular cuánto se tendría que aumentar el radio
            distance_type radius_increase = d - e->get_cover_radius();
            if (radius_increase < min_radius_increase) {
                min_radius_increase = radius_increase;
                fallback_candidate = e->get_cover_tree();
            }
        }
    }

    if (best_candidate) {
        return find_leaf(new_entry, best_candidate);
    } 
    return find_leaf(new_entry, fallback_candidate);
     
    assert("No deberia llegar aqui");
}


template <typename Params>
auto m_tree<Params>::promote(node_ptr node) -> node_ptr {
    auto ancestor_node = (node == root_ ? make_node(false) : node->get_father_node());

    if(node != root_) 
        ancestor_node->remove_entry(node->get_father_entry()->get_pos());
    
    auto [new_covers, new_entries] = pick_promoters(node);

    std::cout <<"IDS _--------------------------------------" <<std::endl;
    for(auto e: new_entries)
        std::cout << e->oid << " ";

    std::cout << " " << std::endl;

    std::vector<int>positions;
    for(auto entry: new_entries)
        positions.push_back(ancestor_node->insert_entry(entry));

    for(int i = 0; i <(int)positions.size(); i++)
        relate(ancestor_node, positions[i], new_covers[i]);

    size_nodes_ += 2;
    return ancestor_node;
}

template <typename Params>
auto m_tree<Params>::keep_overflow(node_ptr node) ->bool{ 

    while(node->get_father_node() and node->is_overflow()) 
        node = promote(node);

    if(not (node->is_overflow())) return true;
    std::cout << "Imprimimos arbol antes de crear un nuevo nodo" << std::endl;
    print_tree();
    root_ = promote(root_);
    std::cout << "Imprimimos arbol despues new_root nodo" << std::endl;
    print_tree();
    return true;
}

template <typename Params>
auto m_tree<Params>::relate(node_ptr& pa, int pos, node_ptr& child) -> void {
    std::cout << "Que vamos a relacionar :)" << std::endl;
    std::cout << "Vamos a relacionar pa set_cover_tree " << std::endl;
    std::cout << "child vamos a fijar pa como padre " << std::endl;
    std::cout << "Vamos a fijar el entry_padre " << std::endl;
    pa->print_node();
    child->print_node();
    std::cout << "pos_entry_padre :" <<  pa->get_entry(pos)->oid << std::endl;

    pa->set_cover_tree(pos, child);
    child->set_father_node(pa);
    child->set_father_entry(pa->get_entry(pos)); // Asumiendo que necesitas esto
}


template <typename Params>
auto m_tree<Params>::pick_promoters(node_ptr& node) ->
std::pair<std::vector<node_ptr>, std::vector<entry_ptr>> {
    assert(node->size_entry() == node->capacity+1);
    std::vector<int>picks(2);

    picks[0] = 0, picks[1] = node->size_entry()-1;

    std::cout << "picks : " << picks[0] << " " << picks[1] << std::endl;

    std::vector<int>partition[2];
    for(int i = 0; i < node->size_entry(); i++) {
        int id_partition = (node->get_entry(picks[0])->distance_to(*node->get_entry(i))  <
                           node->get_entry(picks[1])->distance_to(*node->get_entry(i)) ? 0 : 1);

        std::cout << "partion - ele-in-partition:" << id_partition << " " << i << std::endl;
        partition[id_partition].push_back(node->get_pos_entry(i));
    }

    std::cout << "Partitions :) " << std::endl;

    for(int i = 0; i < 2; i++) {
        std::cout << picks[i] << " : ";
        for(auto e: partition[i]) {
            std::cout << e << " ";
        }
        std::cout <<" " <<std::endl;
    }

    std::vector<int>pos_promote(2);
    std::vector<node_ptr>ptr_covert_trees(2);
    for(int i = 0; i < 2; i++) {
        int r;
        ptr_covert_trees[i] = node->split_after_promote(picks[i], partition[i], r);
        ptr_covert_trees[i]->print_node();
        pos_promote[i] =r;
    }


    std::vector<entry_ptr>ptr_entrys;
    for(int i = 0; i < 2; i++) {
        distance_type new_cover_radius = 0.0; 
        int pos = pos_promote[i]; //en que posicion se guardo el 
        int ii = 0;
        for(auto id : partition[i]) {
            std::cout << "medir :" << picks[i] << " ----> " << id << std::endl;
            std::cout << "pos  :" << pos << std::endl;
            if(id == picks[i]) {
                ii++;
                continue;
            }
            new_cover_radius = std::max(new_cover_radius, 
            ptr_covert_trees[i]->get_entry(pos)->distance_to(*ptr_covert_trees[i]->get_entry(ii)));
            std::cout << "radio :" << new_cover_radius << std::endl;
            ii+=1;
        }

        std::cout << "pos - hijo - promotor :" << pos_promote[i] << std::endl;
        auto oi = make_inter(*ptr_covert_trees[i]->get_entry(pos), ptr_covert_trees[i] , new_cover_radius);
        ptr_entrys.push_back(oi);
    }
    std::cout << "ok2" << std::endl;

    return  {ptr_covert_trees, ptr_entrys};
}

template <typename Params>
void m_tree<Params>::print_tree(node_ptr node, int depth) const noexcept {
    if (!node) return;

    std::string indent(depth * 8, ' ');
    std::cout << indent << "[";

    // Imprimir oids de las entradas del nodo
    for (int i = 0; i < Params::cap_node + 1; ++i) {
        auto entry = node->get_entry(i);
        if (entry) {
            std::cout << entry->oid << " ";
        }
    }
    std::cout << "]\n";

    if (!node->is_leaf()) {
        for (int i = 0; i < Params::cap_node + 1; ++i) {
            auto entry = node->get_entry(i);
            if(entry) {
                print_tree(node->cover_tree(i), depth + 1);
            }
        }
    }
}

template <typename Params>
void m_tree<Params>::print_tree() const noexcept {

    std::cout << "==============***********************************************************=======" << std::endl;
    print_tree(root_, 0);

    std::cout << "==============***********************************************************=======" << std::endl;
}

#endif // M_TREE_HPP
