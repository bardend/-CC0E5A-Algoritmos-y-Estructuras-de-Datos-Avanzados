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

template <typename Params>
class node : public std::enable_shared_from_this<node<Params>> {
public:
    using node_ptr = std::shared_ptr<node<Params>>;
    using entry_ptr = std::shared_ptr<entry<Params>>;
    static constexpr int capacity =  Params::cap_node;
  	typedef Params params_type;
    using distance_type = typename Params::feature_type;

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
    void set_leaf(bool n_leaf = false) noexcept { fields_.is_leaf = n_leaf;}
    explicit node(bool leaf = true) : fields_{leaf, 0, nullptr},
             current_pos_entry_{0},size_entry_{0}, entries_(capacity + 1) {}

    bool is_leaf() const noexcept { return fields_.is_leaf; }
    bool is_overflow() const noexcept { return size_entry_ > capacity;}
    bool empty() const noexcept {return size_entry_ == 0;}
    int size_entry() const noexcept { return size_entry_;}

    entry_ptr& get_entry(int i) {
        assert(i >= 0 and i < capacity+1);
        return entries_[i];
    }

    node_ptr cover_tree(int pos) { 
        assert(pos>= 0 and pos < capacity+1); 
        auto entry = entries_[pos];
        if (entry && entry->is_internal()) 
            return entry->get_cover_tree();
        else {
            return nullptr;
        }
    }

    void set_cover_tree(int pos, node_ptr new_cover_tree) {
        // 1. el entry i-simo (pos) setearemos para que tenga un nuevo cover-tree
        auto entry = entries_[pos];
        if (entry && entry->is_internal()) 
            entry->set_cover_tree(new_cover_tree);
        else assert(false && "No deberia llegar aqui");
    }

    entry_ptr get_father_entry() { return father_entry_;}
    void set_father_entry(entry_ptr entry) { father_entry_ = entry;}

    void set_father_node(node_ptr x) {fields_.father = x;}
    node_ptr get_father_node() { return fields_.father;}

    //Esto es importante ----> Porque necesito que posicion esta en el nodo;
    int get_pos_entry(int i) { return entries_[i]->get_pos();}

    //Insert a parte de insertar retorna la posicion donde se inserto.
    int insert_entry(entry_ptr entry) {
        entries_[current_pos_entry_] = entry;
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
        return ret;
    }

    void remove_entry(int pos) {
        entries_[pos].reset();
        current_pos_entry_ = std::min(current_pos_entry_, pos);
        size_entry_--;
    }

    // Mi i por j del otro
    void swap_entry(int i, node_ptr& other, int j){
        assert(other != this);
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
        entries_[i]->set_pos(i);
        entries_[j]->set_pos(j);
    }

    // Movemos el entry i de this hacia el otro
    void move_entry(int i, node_ptr&other) {
        other->insert_entry(entries_[i]);
        entries_[i].reset();
        current_pos_entry_ = std::min(current_pos_entry_, i);
        size_entry_--;
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
    
    void make_partition_afetr_split(int pos, std::vector<int>partition);
    void print_tree() const noexcept;

    void print_tree(node_ptr node, int depth) const noexcept;
    void print_tree(node_ptr node, std::string msg) const noexcept;

};

template <typename Params>
void node<Params>::print_tree(node_ptr node, int depth) const noexcept {
    if (!node) return;

    std::string indent(depth * 8, ' ');
    std::cout << indent << "[";

    for (int i = 0; i < Params::cap_node + 1; ++i) {
        auto entry = node->get_entry(i);
        if (entry) {
            std::cout << entry->oid << " ";
        }
    }
    std::cout << "]\n";
    //
    // if(node->get_father_node()){ 
    //     std::cout << "Y yo vengo del padre : ---------------------"  << std::endl;
    //     node->get_father_node()->print_node();
    // }

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
void node<Params>::print_tree(node_ptr node, std::string msg) const noexcept {
    std::cout << "============== " << msg << " ==============" << std::endl;
    print_tree(node, 0);
    std::cout << "============== FIN  ==============" << std::endl;
}



template<typename Params>
auto node<Params>::make_partition_afetr_split(int pos_parent, std::vector<int>partition) -> void {
    /* Input : 
     *      -pos_parent : Posicion actual en el padre del entry promotor.
     *      -partition : Id de entry que seguiran al entry que fue promovida.
     *
     * 1. Generamos la nueva particion
     * 2. Insertamos las nuevas realciones entre la nueva particion(new_sibling) y el promotor
     */
    auto new_sibling = std::make_shared<node<Params>>(is_leaf());

    //Asignamos como padre a new_sibling a los cover_tree de node
    for(auto id : partition) 
        if(this->cover_tree(id))
            this->cover_tree(id)->set_father_node(new_sibling);

    //movemos las entrys
    for(auto id : partition)
        this->move_entry(id, new_sibling);

    //asignamos relaciones con el padre
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
    using distance_type = typename Params::feature_type;

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
            root_ = std::make_shared<node<Params>>(true);
            root_->insert_entry(new_entry);
            size_nodes_ = size_entries_ = 1;
            return;
        }
        auto current_leaf = find_leaf(new_entry, root_);
        current_leaf->insert_entry(new_entry);
        size_entries_ += 1;
        keep_overflow(current_leaf);
    }

    node_ptr find_leaf(entry_ptr entry, node_ptr curr_node);
    node_ptr promote(node_ptr node);
    void split(node_ptr& node);
    void keep_invariant(node_ptr node);
    void print_tree() const noexcept;
    void print_tree(node_ptr node, int depth) const noexcept;
    void print_tree(node_ptr node, std::string msg) const noexcept;

    //////////////////////////////////////////////////////////////////////////////
    public:
    void bottom_up() const noexcept;
    void collect_leaves(node_ptr node, std::vector<node_ptr>& leaves) const noexcept;
    void print_path_to_root(node_ptr leaf) const noexcept;

};

// Implementaciones de los métodos
template <typename Params>
void m_tree<Params>::bottom_up() const noexcept {
    if (!root_) {
        std::cout << "Árbol vacío" << std::endl;
        return;
    }
    
    std::vector<node_ptr> leaves;
    collect_leaves(root_, leaves);

    
    std::cout << "============== BOTTOM-UP TRAVERSAL ==============" << std::endl;
    std::cout << "Número de hojas encontradas: " << leaves.size() << std::endl;
    std::cout << "==================================================" << std::endl;
    
    for (size_t i = 0; i < leaves.size(); ++i) {
        std::cout << "\n--- HOJA " << (i + 1) << " ---" << std::endl;
        print_path_to_root(leaves[i]);
        std::cout << "----------------------------------------" << std::endl;
    }
}

template <typename Params>
void m_tree<Params>::collect_leaves(node_ptr node, std::vector<node_ptr>& leaves) const noexcept {
    if (!node) return;
    
    if (node->is_leaf()) {
        leaves.push_back(node);
        return;
    }
    
    // Recorrer todos los entries del nodo interno
    for (int i = 0; i < capacity + 1; ++i) {
        if (node->cover_tree(i)) {
            auto subtree = node->cover_tree(i);
            collect_leaves(subtree, leaves);
        }
    }
}

template <typename Params>
void m_tree<Params>::print_path_to_root(node_ptr leaf) const noexcept {
    if (!leaf) return;

    std::vector<node_ptr> path;
    node_ptr current = leaf;
    // Construir el path desde la hoja hasta la raíz
    while (current) {
        path.push_back(current);
        current = current->get_father_node();
    }
    
    // Imprimir información de la hoja
    std::cout << "HOJA (Nivel " << (path.size() - 1) << "): [";
    path[0]->print_node();
    
    // Imprimir el path completo desde hoja hacia raíz
    std::cout << "Path hacia raíz:" << std::endl;
    for (int level = 1; level <(int)path.size(); level++) {
        node_ptr node = path[level];
        std::string indent(2 * (path.size() - 1 - level), ' ');
        
        std::cout << indent << "↑ Nivel " << level << ": [";
        node->print_node();
        std::cout << "]";
        
        if (level == 0) {
            std::cout << " (ROOT)";
        }
        std::cout << std::endl;
    }
    
    std::cout << "Profundidad del path: " << (path.size() - 1) << std::endl;
}
/////////////////////////////////////////////////////


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
            if (d < best_distance) {
                best_distance = d;
                best_candidate = e->get_cover_tree();
            }
        } else {
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

template <typename Params>
auto m_tree<Params>::keep_overflow(node_ptr node) ->void{ 
    /*
     * Inputs:
     *      node : nodo actual al cual verificar que cumpla la invariante
     * Se hace un recorrido bottom-up verificando la invariante,
     */
    while(node->get_father_node() and node->is_overflow())  
        node = promote(node);

    //Bien ahora debemos actuzalizar los radios de covertura de sus padres, mateniendo la invariante
    if(not (node->is_overflow())) {
        keep_invariant(node); //Esto esta mal porque voy por todas la entry :(, solo deberia ir por las que acabo de insertar
        return ;
    }
    root_ = promote(root_);
}

//Aqui deberia enviar el entry que se acaba de insertar, pero bueno pendiente
template <typename Params>
auto m_tree<Params>::keep_invariant(node_ptr node) -> void {
    if(node->get_father_entry()) {
        entry_ptr entry_p = node->get_father_entry();

        distance_type new_cover_radius = entry_p->radio_covertura;

        for(int i = 0; i < capacity; i++) {
        if(node->get_entry(i)) {
        new_cover_radius = std::max(new_cover_radius, 
            entry_p->distance_to(*node->get_entry(i)) + node->get_entry(i)->radio_covertura);
        }
        }
        entry_p->set_cover_radius(new_cover_radius);
        keep_invariant(node->get_father_node());
    }
}

template <typename Params>
auto m_tree<Params>::promote(node_ptr node) -> node_ptr {
    /*
     * Inputs:
     *      - node : nodo actual el cual se piensa promover algunas(2) entries
     * Verificamos si estamos en el root necesitamos crear un nuevo ``new_root``
     * Caso contrario eliminamos al entry_padre que estamos enlazados y promovemos nuevas entry(split)
     */

    node_ptr ancestor_node;
    if(node == root_) ancestor_node = make_node(false) ;
    else  ancestor_node = node->get_father_node();

    if(node != root_)  ancestor_node->remove_entry(node->get_father_entry()->get_pos());
    node->set_father_node(ancestor_node);

    split(node);
    size_nodes_ += 2;
    node.reset();
    return ancestor_node;
}

template <typename Params>
auto m_tree<Params>::split(node_ptr& node) -> void {
    /* 
     * Inputs :
     *      - node : nodo actual al cual pienso particinar.
     * 
     * 1. picks son los indices de los nodos promovidos dentro del nodo.
     * 2. partition[x] son el conjunto de indices que seguiran el x-simo pick
     * Con esto (1, 2) generar el nuevo entry acorde a diversas heuristicas, insertar en el padre,
     * relacionarse (get_father, get_entry, get_covert_tree).
     */
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

    auto gen_entry_promoter = [&](int id, std::vector<int>followers) {
        distance_type new_cover_radius = node->get_entry(id)->radio_covertura;
        for(auto e : followers) {
            if(e == id) continue;
            new_cover_radius = std::max(new_cover_radius, 
            node->get_entry(id)->distance_to(*node->get_entry(e)) + node->get_entry(e)->radio_covertura);
        }

        entry_ptr new_promoter;
        return new_promoter = make_inter(*node->get_entry(id), nullptr, new_cover_radius); //Recordamos que mas adelante seteas el nuevo cover_tree.
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

    for (int i = 0; i < Params::cap_node + 1; ++i) {
        auto entry = node->get_entry(i);
        if (entry) {
            std::cout << entry->oid << "-" << entry->radio_covertura << "    ";
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
void m_tree<Params>::print_tree(node_ptr node, std::string msg) const noexcept {
    std::cout << "============== " << msg << " ==============" << std::endl;
    print_tree(node, 0);
    std::cout << "============== FIN  ==============" << std::endl;
}

template <typename Params>
void m_tree<Params>::print_tree() const noexcept {
    std::cout << "==============***********************************************************=======" << std::endl;
    print_tree(root_, 0);
    std::cout << "==============***********************************************************=======" << std::endl;
}
#endif // M_TREE_HPP
