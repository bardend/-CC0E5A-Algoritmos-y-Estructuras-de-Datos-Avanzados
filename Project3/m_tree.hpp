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
#include <queue>
#include <iostream>

#include "entry.hpp"
#include "utils.hpp"

template <typename Params> class entry;
template <typename Params> class internal_entry;

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


    entry_ptr get_entry(int i) {
        assert(i >= 0 and i < capacity+1);
        return entries_[i];
    }

    node_ptr cover_tree(int pos) { 
        assert(pos>= 0 and pos < capacity+1); 
        if (auto* internal = dynamic_cast<internal_entry<Params>*>(entries_[pos].get())) 
            return entries_[pos]->get_cover_tree();
        else 
            return nullptr;
    }

    // 1. el entry i-simo (pos) setearemos para que tenga un nuevo cover-tree
    void set_cover_tree(int pos, node_ptr new_cover_tree) {
        if (auto* internal = dynamic_cast<internal_entry<Params>*>(entries_[pos].get())) 
            entries_[pos]->set_cover_tree(new_cover_tree);
        else assert(false && "No deberia llegar aqui");
    }

    void set_father_node(node_ptr x) {fields_.father = x;}
    void set_father_entry(entry_ptr entry) { father_entry_ = entry;}
    void set_father_entry(entry_inter_ptr entry) { father_entry_ = entry;}

    node_ptr get_father_node() { return fields_.father;}
    entry_ptr get_father_entry() { return father_entry_;}

    //Esto es importante ----> Porque necesito que posicion esta en el nodo;
    int get_pos_entry(int i) { return entries_[i]->get_pos();}

    //Insert a parte de insertar retorna la posicion donde se inserto.
    int insert_entry(entry_inter_ptr entry) {
        entries_[current_pos_entry_] = entry;
        return update_state_insert();
    }
    int insert_entry(entry_ptr entry) {
        if(is_leaf()) entries_[current_pos_entry_] = make_leaf(*entry);
        else entries_[current_pos_entry_] = make_inter(*entry, nullptr, 1.5);
        return update_state_insert();
    }

    int update_state_insert() {
        size_entry_++;
        entries_[current_pos_entry_]->set_pos(current_pos_entry_);
        int ret = current_pos_entry_;
        for(int i = current_pos_entry_+1; i < capacity +1; i++) 
            if(!entries_[i]) {
                current_pos_entry_ = i; 
                break;
            }
        std::cout << "la posicion en que fue insertada :" << ret << std::endl;
        return ret;
    }

    void remove_entry(int pos) {
        entries_[pos].reset();
        current_pos_entry_ = std::min(current_pos_entry_, pos);
        size_entry_--;
    }

    // Mi i por j del otro
    void swap_entry(int i, node_ptr other, int j){
        assert(other != this);
        params_type::swap(entries_[i], other->entries_[j]);
        entries_[i]->set_pos(i);
        other->entries_[j]->set_pos(j);
    }

    void swap_entry(int i, node_ptr other) {
        //assert(other != this);
        params_type::swap(entries_[i], other->entries_[i]);
    }

    void swap_entry(int i, int j) {
        assert(i != j);
        params_type::swap(entries_[i], entries_[j]);
        entries_[i]->set_pos(i);
        entries_[j]->set_pos(j);
    }

    // Movemos el entry i de this hacia el otro
    void move_entry(int i, node_ptr other) {
        std::cout << "=========================INICIO==DEL==MOVE========================" << std::endl;
        
        if(this->cover_tree(i)) this->cover_tree(i)->print_node();
        int id = other->insert_entry(entries_[i]);
        if(other->cover_tree(id)) other->cover_tree(id)->print_node();
        entries_[i].reset();
        current_pos_entry_ = std::min(current_pos_entry_, i);
        size_entry_--;
        std::cout << "=========================FIN==DEL==MOVE========================" << std::endl;
    }

    void print_node(bool ok = 1) const {

        for(int i = 0; i < capacity + 1; i++)  {
            if(entries_[i]) std::cout << entries_[i]->oid << " ";
        }
        std::cout << std::endl;
    }
    void make_partition_afetr_split(int pos, std::vector<int>partition);
};

/* Input : 
 *      -pos_parent : Posicion actual en el padre del entry promotor.
 *      -partition : Id de entry que seguiran al entry que fue promovida.
 *
 * 1. Generamos la nueva particion
 * 2. Insertamos las nuevas realciones entre la nueva particion(new_sibling) y el promotor
 */
template<typename Params>
auto node<Params>::make_partition_afetr_split(int pos_parent, std::vector<int>partition) -> void {

    auto new_sibling = std::make_shared<node<Params>>(is_leaf());

    for(auto id : partition)
        this->move_entry(id, new_sibling);

    new_sibling->set_father_entry(this->get_father_node()->get_entry(pos_parent));
    new_sibling->set_father_node(this->get_father_node());
    this->get_father_node()->get_entry(pos_parent)->set_cover_tree(new_sibling);
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
    node_ptr get_root() const { return root_; }
    void keep_overflow(node_ptr node);

    void insert(entry_ptr new_entry) {  
        if(!root_) {
            root_ = make_node(true);
            root_->insert_entry(new_entry);
            size_nodes_ = size_entries_ = 1;
            return ;
        }
        auto current_leaf = find_leaf(new_entry, root_);
        current_leaf->insert_entry(new_entry);
        size_entries_ += 1;
        keep_overflow(current_leaf);
    }

    node_ptr find_leaf(entry_ptr entry, node_ptr curr_node);
    node_ptr promote(node_ptr node);
    void split(node_ptr node);
    void print_tree( ) const noexcept;
    void print_tree(node_ptr node) const noexcept;
    void print_tree(node_ptr node, int depth) const noexcept;
};


template <typename Params>
auto m_tree<Params>::find_leaf(entry_ptr new_entry, node_ptr curr_node) -> node_ptr {

    if (curr_node->is_leaf()) 
        return curr_node;

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
     
    assert(false && "No deberia llegar aqui");
}

/*
 * Inputs:
 *      node : nodo actual al cual verificar que cumpla la invariante
 * Se hace un recorrido bottom-up verificando la invariante,
 */

template <typename Params>
auto m_tree<Params>::keep_overflow(node_ptr node) -> void{ 
    while(node->get_father_node() and node->is_overflow()) 
        node = promote(node);

    if(not (node->is_overflow())) return;
    print_tree();
    root_ = promote(root_);
}

/*
 * Inputs:
 *      - node : nodo actual el cual se piensa promover algunas_ por el momento(2) entries
 * Verificamos si estamos en el root necesitamos crear un nuevo ``new_root``
 * Caso contrario eliminamos al entry_padre que estamos enlazados y promovemos nuevas entry(split)
 */

template <typename Params>
auto m_tree<Params>::promote(node_ptr node) -> node_ptr {
    node_ptr ancestor_node = (node == root_ ? make_node(false) : node->get_father_node());
    if(node != root_)  ancestor_node->remove_entry(node->get_father_entry()->get_pos());
    node->set_father_node(ancestor_node);
    split(node);
    size_nodes_ += 2;
    return ancestor_node;
}
/* 
 * Inputs :
 *      - node : nodo actual al cual pienso particionar.
 * 
 * 1. picks son los indices de los nodos promovidos dentro del nodo.
 * 2. partition[x] son el conjunto de indices que seguiran el x-simo pick
 * Con esto (1, 2) generar el nuevo entry acorde a diversas heuristicas, insertar en el padre,
 * relacionarse (get_father, get_entry, get_covert_tree).
 */

template <typename Params>
auto m_tree<Params>::split(node_ptr node) -> void {

    assert(node->size_entry() == node->capacity+1);

    // Por ahora solo se trabajan con 2 promotores
    const int num_promots = 2;
    std::vector<int>picks(num_promots);
    picks[0] = 0, picks[1] = node->size_entry()-1; //se debe cambiar por la heuristica
    

    std::vector<std::vector<int>>partition(num_promots);

    for(int i = 0; i < node->size_entry(); i++) {
        int id_partition = (node->get_entry(picks[0])->distance_to(*node->get_entry(i))  <
                           node->get_entry(picks[1])->distance_to(*node->get_entry(i)) ? 0 : 1);

        partition[id_partition].push_back(node->get_pos_entry(i));
    }
    for(int i = 0;i < num_promots; i++) {
        for(auto e : partition[i]) {
            std::cout << node->get_entry(e)->oid << " ";
        }
        std::cout << std::endl;
    }

    auto gen_entry_promoter = [&](int id, std::vector<int>followers) {
        distance_type new_cover_radius = 0.0; 
        for(auto e : followers) {
            if(e == id) continue;
            new_cover_radius = std::max(new_cover_radius, 
                    node->get_entry(id)->distance_to(*node->get_entry(e)));
        }
        return make_inter(*node->get_entry(id), nullptr, new_cover_radius);
    };

    for(int i = 0; i < num_promots; i++) {

        entry_ptr cur_promoter = gen_entry_promoter(picks[i], partition[i]);
        int pos_promoter_in_father = node->get_father_node()->insert_entry(cur_promoter);
        node->make_partition_afetr_split(pos_promoter_in_father, partition[i]);
    }
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
                print_tree(node->get_entry(i)->get_cover_tree(), depth + 1);
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

template <typename Params>
void m_tree<Params>::print_tree(node_ptr source) const noexcept {

    std::cout << "==============***********************************************************=======" << std::endl;
    print_tree(source, 0);

    std::cout << "==============***********************************************************=======" << std::endl;
}


#endif // M_TREE_HPP
