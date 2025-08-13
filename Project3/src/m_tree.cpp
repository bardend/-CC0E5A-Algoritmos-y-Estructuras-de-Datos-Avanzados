#include "m_tree.h"

// Implementaciones de las funciones de la clase node
template <typename Params>
void node<Params>::set_leaf(bool n_leaf) noexcept { 
    fields_.is_leaf = n_leaf;
}

template <typename Params>
node<Params>::node(bool leaf) : fields_{leaf, 0, nullptr},
         current_pos_entry_{0},size_entry_{0}, entries_(capacity + 1) {}

template <typename Params>
bool node<Params>::is_leaf() const noexcept { 
    return fields_.is_leaf; 
}

template <typename Params>
bool node<Params>::is_overflow() const noexcept { 
    return size_entry_ > capacity;
}

template <typename Params>
bool node<Params>::empty() const noexcept {
    return size_entry_ == 0;
}

template <typename Params>
int node<Params>::size_entry() const noexcept { 
    return size_entry_;
}

template <typename Params>
const typename node<Params>::entry_ptr& node<Params>::get_entry(int i) const {
    assert(i >= 0 and i < capacity+1);
    return entries_[i];
}

template <typename Params>
typename node<Params>::node_ptr node<Params>::cover_tree(int pos) const { 
    assert(pos>= 0 and pos < capacity+1); 
    auto entry = entries_[pos];
    if (entry && entry->is_internal()) 
        return entry->get_cover_tree();
    else {
        return nullptr;
    }
}

template <typename Params>
void node<Params>::set_cover_tree(int pos, node_ptr new_cover_tree) {
    auto entry = entries_[pos];
    if (entry && entry->is_internal()) 
        entry->set_cover_tree(new_cover_tree);
    else assert(false && "No deberia llegar aqui");
}

template <typename Params>
typename node<Params>::entry_ptr node<Params>::get_father_entry() { 
    return father_entry_;
}

template <typename Params>
void node<Params>::set_father_entry(entry_ptr entry) { 
    father_entry_ = entry;
}

template <typename Params>
void node<Params>::set_father_node(node_ptr x) {
    fields_.father = x;
}

template <typename Params>
typename node<Params>::node_ptr node<Params>::get_father_node() { 
    return fields_.father;
}

template <typename Params>
int node<Params>::get_pos_entry(int i) { 
    return entries_[i]->get_pos();
}

template <typename Params>
int node<Params>::insert_entry(entry_ptr entry) {
    entries_[current_pos_entry_] = entry;
    return update_state_insert();
}

template <typename Params>
int node<Params>::update_state_insert() {
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

template <typename Params>
void node<Params>::remove_entry(int pos) {
    entries_[pos].reset();
    current_pos_entry_ = std::min(current_pos_entry_, pos);
    size_entry_--;
}

template <typename Params>
void node<Params>::swap_entry(int i, node_ptr& other, int j){
    assert(other.get() != this);
    params_type::swap(entries_[i], other->entries_[j]);
    entries_[i]->set_pos(i);
    other->entries_[j]->set_pos(j);
}

template <typename Params>
void node<Params>::swap_entry(int i, node_ptr& other) {
    params_type::swap(entries_[i], other->entries_[i]);
}

template <typename Params>
void node<Params>::swap_entry(int i, int j) {
    assert(i != j);
    params_type::swap(entries_[i], entries_[j]);
    entries_[i]->set_pos(i);
    entries_[j]->set_pos(j);
}

template <typename Params>
void node<Params>::move_entry(int i, node_ptr&other) {
    other->insert_entry(entries_[i]);
    entries_[i].reset();
    current_pos_entry_ = std::min(current_pos_entry_, i);
    size_entry_--;
}

template <typename Params>
std::pair<int, int> node<Params>::pick_promoters(const heuristic_split_f& heuristic) const {
    return heuristic(this->shared_from_this());
}


template<typename Params>
void node<Params>::make_partition_afetr_split(int pos_parent, std::vector<int>partition) {
    /* Input : 
     *      -pos_parent : Posicion actual en el padre del entry promotor.
     *      -partition : Id's de los entries que seguiran al entry que fue promovida.
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

// Implementaciones de las funciones de la clase m_tree
template <typename Params>
typename m_tree<Params>::node_ptr m_tree<Params>::find_leaf(entry_ptr new_entry, node_ptr curr_node) {
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
void m_tree<Params>::keep_overflow(node_ptr node, std::vector<int>&positions_promote) {
    /*
     * Inputs:
     *      node : nodo actual al cual verificar que cumpla la invariante
     * Se hace un recorrido bottom-up verificando la invariante,
     */
    
    while(node->get_father_node() and node->is_overflow())  
        node = promote(node, positions_promote);

    if(not(node->is_overflow())) {
        keep_invariant(node, positions_promote);
        return ;
    }
    root_ = promote(root_, positions_promote);
}

template <typename Params>
void m_tree<Params>::keep_invariant(node_ptr node, std::vector<int>&positions) {
    /*
     * Inputs:
     *      -node el nodo actual camino al root
     *      -positions vector de las posiciones de las entries que fueron actualizadas y debemos propagar hacia su padre node
     */

    if(node->get_father_entry()) {
        entry_ptr entry_p = node->get_father_entry();
        distance_type new_cover_radius = entry_p->radio_covertura;
        for(auto pos : positions)
            if(node->get_entry(pos)) {
            new_cover_radius = std::max(new_cover_radius, 
                entry_p->distance_to(*node->get_entry(pos)) + node->get_entry(pos)->radio_covertura);
            }
        entry_p->set_cover_radius(new_cover_radius);
        std::vector<int>continue_path_positions(1);
        continue_path_positions[0] = node->get_father_entry()->get_pos();
        keep_invariant(node->get_father_node(), continue_path_positions);
    }
}

template <typename Params>
typename m_tree<Params>::node_ptr m_tree<Params>::promote(node_ptr node, std::vector<int>&positions_promote) {

    /*
     * Inputs:
     *      - node : nodo actual el cual se piensa promover algunas(2) entries
     * Verificamos si estamos en el root necesitamos crear un nuevo ``new_root``
     * Caso contrario eliminamos al entry_padre que estamos enlazados y promovemos nuevas entry(split)
     */

    positions_promote.clear();
    node_ptr ancestor_node;
    if(node == root_) ancestor_node = make_node(false) ;
    else  ancestor_node = node->get_father_node();

    if(node != root_)  ancestor_node->remove_entry(node->get_father_entry()->get_pos());
    node->set_father_node(ancestor_node);

    split(node, positions_promote);
    size_nodes_ += num_entries_promoters;
    node.reset();
    return ancestor_node;
}

template <typename Params>
void m_tree<Params>::split(node_ptr& node, std::vector<int>&positions_promote) {

    /* 
     * Inputs :
     *      - node : nodo actual al cual pienso particinar.
     * 
     * 1. picks son los indices de los nodos promovidos dentro del nodo.
     * 2. partition[x] son el conjunto de indices que seguiran el x-simo pick
     * Con esto (1, 2) generar el nuevo entry acorde a diversas heuristicas, insertar en el padre,
     * relacionarse (get_father, get_entry, get_covert_tree).
     */

    assert(node->size_entry() == capacity+1);
    std::vector<int>picks(num_entries_promoters);
    auto [pick1, pick2] = node->pick_promoters();
    picks[0] = pick1; picks[1] = pick2;

    std::vector<std::vector<int>>partition(num_entries_promoters);

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
        return new_promoter = make_inter(*node->get_entry(id), nullptr, new_cover_radius);
    };

    positions_promote.resize(num_entries_promoters);
    for(int i = 0; i < num_entries_promoters; i++) {
        entry_ptr cur_promoter = gen_entry_promoter(picks[i], partition[i]);
        int pos_promoter_in_father = node->get_father_node()->insert_entry(cur_promoter);
        node->make_partition_afetr_split(pos_promoter_in_father, partition[i]);
        positions_promote.push_back(pos_promoter_in_father);
    }
}

template <typename Params>
std::vector<typename Params::identifier_type> m_tree<Params>::k_nn_entries(entry_ptr query, int k) {
    auto d = [&](entry_ptr entry, entry_ptr query) -> std::pair<distance_type, distance_type> {
        std::pair<distance_type, distance_type> ret; 
        return ret = { std::max(0.0, (entry->distance_to(*query)- (entry->get_cover_radius()))),
                       entry->distance_to(*query) + entry->get_cover_radius() };
    };

    auto validate_candidate =[&](std::priority_queue<std::pair<distance_type, entry_ptr>>&candidates, entry_ptr entry) -> void {
        auto [mini, maxi] = d(entry, query);
        if((int)candidates.size() < k) {
            candidates.push({-mini, entry});
        }
        else {
            candidates.push({-mini, entry});
        }
    };

    std::priority_queue<std::pair<distance_type, entry_ptr>>candidates;
    std::priority_queue<std::pair<distance_type, entry_ptr>>best;

    node_ptr node = root_;
    for(int i = 0; i < capacity; i++) {
        auto e = node->get_entry(i);
        if(!e) continue;
        validate_candidate(candidates, e);
    }
    while(!candidates.empty()) {
        auto [dis, entry] = candidates.top();
        candidates.pop();

        if(entry->is_leaf()) {
            best.push({-(entry->distance_to(*query)), entry});
            if((int)best.size() == k) break;
        }
        else {
            node_ptr node = entry->get_cover_tree();
            for(int i = 0; i < capacity; i++) {
                auto e = node->get_entry(i);
                if(!e) continue;
                validate_candidate(candidates, e);
            }
        }
    }

    std::vector<identifier_type> ret(k);
    for(int i = 0; i < k; i++) {
        ret[i] = best.top().second->oid;
        best.pop();
    }
    return ret;
}

// Instanciaciones explícitas para los tipos que se usan
// Implementaciones de heuristic_split
template <typename Params>
std::pair<int, int> heuristic_split<Params>::random_max_dis(const std::shared_ptr<const node<Params>>& node) {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(0, capacity);
    int ini = dis(gen);
    int pick;
    distance_type min_dis = std::numeric_limits<distance_type>::infinity();
    for(int j = 0; j <= capacity; j++) {
        if(j == ini)continue;
        distance_type cur_dis = node->get_entry(j)->distance_to(*(node->get_entry(ini)));
        if(cur_dis< min_dis) {
            min_dis = cur_dis;
            pick = j;
        }
    }
    return {ini, pick};
}

template <typename Params>
std::pair<int, int> heuristic_split<Params>::pair_max_dis(const std::shared_ptr<const node<Params>>& node) {
    distance_type min_dis = std::numeric_limits<distance_type>::infinity();
    int pick1, pick2;
    for(int i = 0; i <= capacity; i++) {
        for(int j = i+1; j <= capacity; j++) {
            distance_type cur_dis = node->get_entry(i)->distance_to(*(node->get_entry(j)));
            if (cur_dis < min_dis) {
                min_dis = cur_dis;
                pick1 = i;
                pick2 = j;
            }
        }
    }
    return {pick1, pick2};
}

template <typename Params>
heuristic_split<Params>::heuristic_split(heuristic_function func) : heuristic(func){}

template <typename Params>
std::pair<int, int> heuristic_split<Params>::operator()(const std::shared_ptr<const node<Params>>& node) const {
    return heuristic(node);
}

// Implementaciones de m_tree
template <typename Params>
m_tree<Params>::m_tree(): size_nodes_{0}, size_entries_{0}{}

template <typename Params>
int m_tree<Params>::size_nodes() const noexcept { 
    return size_nodes_; 
}

template <typename Params>
void m_tree<Params>::clear() noexcept { 
    root_.reset(); 
    size_nodes_ = 0; 
    size_entries_ =0; 
}

template <typename Params>
typename m_tree<Params>::node_ptr m_tree<Params>::get_root() const { 
    return root_; 
}

template <typename Params>
void m_tree<Params>::insert(entry_ptr new_entry) {
    if(!root_) {
        root_ = std::make_shared<node<Params>>(true);
        root_->insert_entry(new_entry);
        size_nodes_ = size_entries_ = 1;
        return;
    }
    auto current_leaf = find_leaf(new_entry, root_);
    std::vector<int>positions;
    positions.push_back(current_leaf->insert_entry(new_entry));
    size_entries_ += 1;
    keep_overflow(current_leaf, positions);
}

#include "config.h"

// Instanciaciones explícitas
template class m_tree<MTreeConfig>;
template class node<MTreeConfig>;
template class heuristic_split<MTreeConfig>; 