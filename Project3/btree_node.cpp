#include <memory>
#include <array>
#include <functional>
#include <type_traits>
#include <cassert>

#include<bits/stdc++.h>
using namespace std;


namespace compact_btree {

template<typename Key, typename Value, int Order = 16>
class btree_node : public std::enable_shared_from_this<btree_node<Key, Value, Order>> {
//class btree_node {

    private:
        static constexpr int MAX_KEYS = Order;
        static constexpr int MIN_KEYS = Order / 2;

        using key_type = Key;
        using value_type = Value;
        using node_ptr = std::shared_ptr<btree_node>;

        std::array<Key, MAX_KEYS + 1> keys_;
        std::array<Value, MAX_KEYS + 1> values_;
        std::array<node_ptr, MAX_KEYS + 2> children_;  // Un hijo más que keys
                                                       //
    public:
        bool is_leaf_;
        int key_count_ = 0;
        int child_count_ = 0;
        // Constructor
        explicit btree_node(bool leaf = true) : is_leaf_(leaf) {}

        // Propiedades básicas
        node_ptr father;
        int child_pos_self;
        int prueba = -1;
        bool have_right_brother() const { 
            if(father)
                return child_pos_self < father->child_count_ -1;
            return false;
        }
        bool have_left_brother() const {
            if(father)
                return child_pos_self > 0;
            return false;
        }
        bool is_leaf() const noexcept { return is_leaf_; }
        bool is_full() const noexcept { return key_count_ == MAX_KEYS; }
        bool is_overflow() const noexcept { return key_count_ > MAX_KEYS; }
        bool is_minimal() const noexcept { return key_count_ < MIN_KEYS; }
        bool can_lend() const noexcept { return key_count_ > MIN_KEYS;}
        int size() const noexcept { return key_count_; }
        bool empty() const noexcept { return key_count_ == 0; }
        // Acceso a datos (implementar después)
        const Key key(int i) const { return keys_[i]; }
        const Value value(int i) const { return values_[i]; }
        node_ptr child(int i) { return children_[i]; }
        node_ptr right_brother() { return father->child(child_pos_self + 1); }
        node_ptr left_brother() { return father->child(child_pos_self -1); }

        void update_my_pos_child(int npos) { child_pos_self = npos;}
        void update_my_father(node_ptr newp) { father = newp; }
        void update_leaf(bool nleaf = false) { is_leaf_ = nleaf;}

        int jump_child(Key k) {
            cout << "key_count_ :" << key_count_ << endl;
            for(int i = 0; i < key_count_; i++) 
                cout << key(i) << " ";
            cout << endl;

            int pos = std::lower_bound(keys_.begin(), keys_.begin() + key_count_, k) - keys_.begin();

            cout << "pos :" << pos << endl;
            cout << "child_count :" << child_count_ << endl;
            assert(pos >= 0 and pos <= key_count_+1);
            return pos;
        }

        node_ptr jump_child(Key k, Value v) {
            int pos = std::lower_bound(keys_.begin(), keys_.begin() + key_count_, k) - keys_.begin();
            assert(pos >= 0 and pos <= child_count_);
            return child(pos);
        }

        std::tuple<std::pair<key_type, value_type>, node_ptr, node_ptr> split() {
            node_ptr first_half = std::make_shared<btree_node>();
            node_ptr second_half = std::make_shared<btree_node>();
            std::pair<key_type, value_type> mid;

            // Aca creo que va self??
            for(int i = 0; i < key_count_/2; i++) 
                first_half->insert_key_value(i, key(i), value(i));

            mid = std::make_pair(key(key_count_/2), value(key_count_/2));

            for(int i = (key_count_/2)+1, j = 0; i < key_count_; i++, j++) 
                second_half->insert_key_value(j, key(i), value(i));

            return std::make_tuple(mid, first_half, second_half);
        }

        std::tuple<std::vector<node_ptr>, std::vector<node_ptr>> split_child_two_sets() {
            std::vector<node_ptr> first_half;
            std::vector<node_ptr> second_half;
            for(int i = 0; i < (child_count_+1)/2; i++) 
                first_half.push_back(child(i));
            for(int i = (child_count_+1)/2; i < child_count_; i++) 
                second_half.push_back(child(i));
            return std::make_tuple(first_half, second_half);
        }
        // Métodos de modificación 
        void insert_key_value(int pos, Key k, Value v) {
            for(int i = key_count_; i > pos; i--) {
                keys_[i] = std::move(keys_[i-1]);
                values_[i] = std::move(values_[i-1]);
            }

            // Insertar nuevos valores
            keys_[pos] = std::move(k);
            values_[pos] = std::move(v);
            ++key_count_;
        }

        //This is good
        void insert_key_value(Key& key, Value& v) {
            int pos = std::lower_bound(keys_.begin(), keys_.begin() + key_count_, key) - keys_.begin();
            assert(pos >= 0 and pos <= key_count_);
            cout <<" Pos: " << pos << endl;
            insert_key_value(pos, std::move(key), std::move(v));
        }

        void remove_key_value(int pos) {
            // Mover elementos hacia la izquierda
            for (int i = pos; i < key_count_ - 1; ++i) {
                keys_[i] = std::move(keys_[i+1]);
                values_[i] = std::move(values_[i+1]);
            }
            --key_count_;
        }

        void insert_child(int pos, node_ptr& child) {
            // Mover punteros de hijos hacia la derecha
            for(int i = child_count_; i > pos; i--) {
                children_[i] = std::move(children_[i-1]);
                children_[i]->update_my_pos_child(i);
            }
            children_[pos] = child;
            child->update_my_pos_child(pos);
            child->update_my_father(this->shared_from_this());
            ++child_count_;
        }

        void remove_child(int pos) {
            if(pos < 0) return;
            assert(pos < child_count_);
            for(int i = pos; i < child_count_-1 ; i++) {
                children_[i] = std::move(children_[i+1]);
                children_[i]->update_my_pos_child(i);
            }
            children_[child_count_-1].reset();
            --child_count_;
        }

        void print_node() {
            for(int i = 0; i < key_count_; i++) 
                cout << keys_[i] << " ";
            cout << endl;
        }
};

template<typename Key, typename Value = Key, typename Compare = std::less<Key>, size_t Order = 16>
    class btree {
    public:
        // Tipos básicos
        using key_type = Key;
        using mapped_type = Value;
        using value_type = std::conditional_t<std::is_same_v<Key, Value>, Key, std::pair<Key, Value>>;
        using key_compare = Compare;
        // Tipos internos
        using node_type = btree_node<Key, Value, Order>;
        using node_ptr = std::shared_ptr<node_type>;  // Corregido typename

        // Constructores
        btree() = default;
        explicit btree(const Compare& comp) : comp_(comp) {}
        ~btree() = default;

        // Información básica
        bool empty() const noexcept { return size_ == 0; }
        int size() const noexcept { return size_; }
        void clear() noexcept { root_.reset(); size_ = 0; }

    /* 
        Keep_Invariant :
        case overflow :
            1. right_rotation for overflow
            2. left_rotation for overflow
            3. recursive split until the invariant is guaranteed
            4. split and create a new root 
        case
    */

        bool keep_invariant(node_ptr node) {

            if(node->is_overflow()) {
                //Rotation
                //Split
                node_ptr current_node = node;
                while(current_node->father and current_node->is_overflow())  {
                    current_node->print_node();
                    if(right_rotation(current_node)) return true;
                    if(left_rotation(current_node)) return true;
                    current_node = std::move(split_btree(current_node));
                }

                if(not (current_node->is_overflow())) return true;
                current_node->print_node();
                // No tienes padre y tienes que aumentar un nivel;
                ++height_; ++size_;
                root_ = std::move(split_btree(current_node)); //Generate a new root
                return true;
            }

            else if(node->is_minimal()) {  // Corregido node->is_minimal() por MIN_KEYS
                while(node->father and node->is_minimal()) { 
                    node->print_node();
                    if(right_rotation_add(node)) return true;
                    if(left_rotation_add(node)) return true;
                    //node = std::move(merge_btree(node));
                }
            }
            return true;;
        }

        bool insert(Key& key, Value& value) {  // Agregado método faltante
            cout <<"***********************************INSERT*********************" << endl;
            if(!root_) {
                root_ = std::make_shared<node_type>(true);
                root_->insert_key_value(0, key, value);
                size_ = height_ = 1;
                return true;
            }
            node_ptr current_node = root_;
            while(not(current_node->is_leaf())) 
                current_node = current_node->jump_child(key, value);

            cout << "Test insert_key_value 1" << endl;
            current_node->print_node();
            current_node->insert_key_value(key, value);
            cout << "Test insert_key_value 2" << endl;
            current_node->print_node();
            int zz = keep_invariant(current_node);
            print_tree();
            return zz;
        }

        std::optional<bool> delete_node(const Key key) { return delete_node(root_, key);}
        std::optional<Value> search(const Key key) const { 
            node_ptr aux_node = root_;
            return search(aux_node, key);
        }

        bool find(const Key& key) const;
        bool erase(const Key& key);

        // Utilidades para debugging
        bool is_valid() const;
        void print() const;

        // Comparador
        key_compare key_comp() const { return comp_; }

        void print_tree() { print_tree(root_, 0); }

       
    private:
        static constexpr int MAX_KEYS = Order;  // Agregado para usar en keep_invariant
        static constexpr int MIN_KEYS = Order / 2;  // Agregado para usar en keep_invariant
        int height_ = 0;
        node_ptr root_;
        int  size_ = 0;  // Corregido int por size_type
        Compare comp_;

        node_ptr split_btree(node_ptr node) {
            cout << "==================SPLIT=================" << endl;
            node_ptr ancestor = (node == root_) ? std::make_shared<node_type>(false) : node->father;
            int pos = (node == root_) ? 0 : node->child_pos_self;

            node->print_node();

            auto [median, left_split, right_split] = node->split(); // Ojo con la referencia aca
            ++size_;
            ancestor->insert_key_value(pos, median.first, median.second);
            if(node != root_) {
                ancestor->remove_child(pos);
            }
            ancestor->insert_child(pos, left_split);
            ancestor->insert_child(pos+1, right_split);

            auto [left_childs, right_childs] = node->split_child_two_sets();

            if((int)left_childs.size() > 0) left_split->update_leaf(false);
            if((int)right_childs.size() > 0) right_split->update_leaf(false);

            for(int i = 0; i < (int)left_childs.size(); i++)
                left_split->insert_child(i, left_childs[i]);
            for(int i = 0; i < (int)right_childs.size(); i++) 
                right_split->insert_child(i, right_childs[i]);

            ancestor->print_node();
            for(int i = 0; i < ancestor->child_count_; i++) {
                cout << "mis Hijos " <<endl;
                ancestor->child(i)->print_node();
                cout << "Mis nietos" << endl;
                for(int j = 0; j < ancestor->child(i)->child_count_; j++) {
                    ancestor->child(i)->child(j)->print_node();
                }
            }
            return ancestor;
        }


        node_ptr merge_btree(node_ptr node) {
            if(node->have_right_brother()) {
                int pos = node->child_pos_self;
                node_ptr right = node->right_brother();
                vector<node_ptr> save_childs;
                for(int i = 0; i < node->child_count_; i++) save_childs.push_back(node->child(i));
                for(int i = 0; i < right->child_count_; i++) save_childs.push_back(right->child(i));

                node->father->remove_key_value(pos);
                node->father->remove_child(pos);
                node->father->remove_child(pos+1);

                node_ptr new_node = std::make_shared<node_type>(false);
                //Como combinas :) facil /

                return new_node;
            }
            if(node->have_left_brother()) {

            }
        }

        std::optional<Value> search(node_ptr& node,Key key) const {
            while (node) {
                int pos = node->jump_child(key);
                if (pos < node->key_count_ && key == node->key(pos)) {
                    return node->value(pos); 
                }

                if (node->is_leaf())  return std::nullopt;
                node = node->child(pos);
            }
            return std::nullopt;
        }

        std::optional<bool> delete_node(node_ptr node, Key key) {

            auto replacement_value = [&](node_ptr node, int pos) -> node_ptr {
                node_ptr current_node = node->child(pos);
                node->remove_key_value(pos);
                while(not(current_node->is_leaf())) 
                    current_node = current_node->child((current_node->child_count_)-1);

                int last_k_pos = (current_node->key_count_)-1;
                node->insert_key_value(pos, current_node->key(last_k_pos), current_node->value(last_k_pos));
                current_node->remove_key_value(last_k_pos);
                return current_node;
            };

            auto exist_key = search(node, key);
            if(exist_key.has_value()) {
                int pos = node->jump_child(key);
                if(node->is_leaf()) {
                    node->remove_key_value(pos);
                    keep_invariant(node);
                    return true;
                }
                node_ptr leaf_helper = replacement_value(node,  pos);
                return keep_invariant(leaf_helper);
            }
            return std::nullopt;
        }

        bool right_rotation_add(node_ptr node) {
            if(node->have_right_brother() and not(node->right_brother()->can_lend())) 
                return left_rotation(node->right_brother());
            return false;
        }
        bool left_rotation_add(node_ptr node) {
            if(node->have_left_brother() and not(node->left_brother()->can_lend()))
                return right_rotation(node->left_brother());
            return false;
        }

        bool right_rotation(node_ptr node) {
            if(node->father) {
                node_ptr p = node->father;
                int child_pos = node->child_pos_self;

                node->print_node();
                cout << "size hijos: "<< node->child_count_ << endl;

                if(node->have_right_brother()  and not(node->right_brother()->is_full())) {
                    cout <<"right rotation" << endl;
                    node_ptr right = node->right_brother();
                    int size_childs = node->child_count_;

                    p->insert_key_value(child_pos, node->key(node->size()-1), node->value(node->size()-1)); 
                    node->remove_key_value(node->size()-1);
                    if(not(node->is_leaf())) {
                        node_ptr jump_son = node->child(size_childs-1);
                        right->insert_child(0, jump_son);
                        node->remove_child(size_childs-1);
                    }
                    right->insert_key_value(0, p->key(child_pos+1), p->value(child_pos+1)); 
                    p->remove_key_value(child_pos+1);
                    return true;
                }
            }
            return false;
        }

        bool left_rotation(node_ptr node) {
            if(node->father) {
                node_ptr p = node->father;
                int child_pos = node->child_pos_self;
                if(node->have_left_brother() and not(node->left_brother()->is_full())) {
                    cout << "left_rotation " << endl;
                    node_ptr left = node->left_brother();
                    p->insert_key_value(child_pos, node->key(0), node->value(0)); 
                    node->remove_key_value(0);
                    cout << "-------------left_rotation------------" << endl;
                    if(not(node->is_leaf())) { 
                        node_ptr jump_son = node->child(0);
                        jump_son->print_node();
                        left->insert_child(left->child_count_, jump_son);
                        node->remove_child(0);
                    }
                    left->insert_key_value(left->size(), p->key(child_pos-1), p->value(child_pos-1));
                    p->remove_key_value(child_pos-1);
                    return true;                    
                }
            }
            return false;
        }

        void print_tree(node_ptr node, int depth) const noexcept{
            if(!node) return;
            string indent(depth * 4, ' ');
            cout << indent << "[";
            for(int i = 0; i < node->key_count_; i++) {
                cout << node->key(i) << " ";
            }
            cout << "]" << endl;
            if(!node->is_leaf()) {
                for(int i = 0; i < node->child_count_; i++) {
                    cout << "i-simo: " << i << endl;
                    print_tree(node->child(i), depth+1);
                }
            }
        }

        // Validaciones en tiempo de compilación
        static_assert(std::is_default_constructible_v<Key>);
        static_assert(std::is_copy_constructible_v<Key>);
};
}

int main() {
    compact_btree::btree<int, int, std::less<int>, 2> btree;
    std::set<int> st;

    std::vector<int> datos = {
    1, 2, 5, 7,                          // hijos de 10
    12, 15, 16, 17,                      // hijos de 19
    20, 21, 22, 23, 24,                  // hijos de 25
    27, 29, 30, 31,                      // hijos de 35
    40, 41, 42, 47,                      // hijos de 50
    51, 53,                              // hijos de 54
    55, 66,                              // hijos de 67
    68, 69,                              // hijos de 70
    71, 75, 76, 77,                      // hijos de 80
    81, 82,                              // hijos de 83
    84, 85, 88, 89,                      // hijos de 90
    91, 92,                              // hijos de 95
    97, 99, 103, 104,                    // hijos de 105
    106, 108, 200                        // hijos de último nodo
    };

    for (int v : datos) {
        int xd = v*10;
        btree.insert(v, xd);  // valor asociado es v * 10
        st.insert(v);
    }

    // Verifica que todos los valores existan en el árbol y coincidan
    for (int v : datos) {
        auto ans = btree.search(v);
        assert(ans.has_value());                   // debe existir
        assert(ans.value() == v * 10);             // debe coincidir el valor
    }

    auto del = btree.delete_node(56);
    if(del.has_value()) {
        cout << "OK" << endl;
    }
    else cout << "NO OK" << endl;


    return 0;
}
