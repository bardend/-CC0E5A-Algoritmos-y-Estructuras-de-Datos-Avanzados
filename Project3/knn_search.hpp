#ifndef KNN_SEARCH_HPP
#define KNN_SEARCH_HPP

#include <queue>
#include <limits>
#include <algorithm>
// Asumiendo que tienes m_tree.hpp incluido también

// Estructura para mantener los k vecinos más cercanos
template<typename Params>
struct NearestNeighbor {
    using identifier_type = typename Params::identifier_type;
    using distance_type = typename Params::feature_type;
    
    identifier_type oid;
    distance_type distance;
    bool is_defined;
    
    NearestNeighbor() : distance(std::numeric_limits<distance_type>::infinity()), is_defined(false) {}
    NearestNeighbor(identifier_type id, distance_type d) : oid(id), distance(d), is_defined(true) {}
    
    // Constructor para bounds (sin oid definido)
    explicit NearestNeighbor(distance_type d) : distance(d), is_defined(false) {}
};

// Estructura para la cola de prioridad de nodos
template<typename Params>
struct PriorityEntry {
    using node_ptr = std::shared_ptr<node<Params>>;
    using distance_type = typename Params::feature_type;
    
    node_ptr node_ptr_val;
    distance_type dmin;
    
    PriorityEntry(node_ptr n, distance_type d) : node_ptr_val(n), dmin(d) {}
    
    // Para que funcione con priority_queue (orden inverso)
    bool operator<(const PriorityEntry& other) const {
        return dmin > other.dmin; // Min-heap
    }
};

// Extensión de la clase m_tree con funcionalidad k-NN
template<typename Params>
class knn_search_extension {
public:
    using node_ptr = std::shared_ptr<node<Params>>;
    using entry_ptr = std::shared_ptr<entry<Params>>;
    using distance_type = typename Params::feature_type;
    using identifier_type = typename Params::identifier_type;
    using metric_space_t = metric_space<Params>;
    using NN = NearestNeighbor<Params>;
    using PR_Entry = PriorityEntry<Params>;
    
private:
    m_tree<Params>* tree_ptr;
    metric_space_t metric;
    
public:
    explicit knn_search_extension(m_tree<Params>* tree) : tree_ptr(tree) {}
    
    // Método principal de búsqueda k-NN
    std::vector<NN> knn_search(const entry<Params>& query, int k) {
        std::vector<NN> nn_array(k);
        std::priority_queue<PR_Entry> pr;
        
        // Inicializar array NN con infinito
        for(int i = 0; i < k; i++) {
            nn_array[i] = NN();
        }
        
        // Obtener la raíz del árbol (necesitarás agregar un getter público)
        node_ptr root = get_root();
        if(!root) return nn_array;
        
        // Inicializar cola de prioridad con la raíz
        pr.push(PR_Entry(root, 0.0));
        
        distance_type dk = std::numeric_limits<distance_type>::infinity();
        
        while(!pr.empty()) {
            PR_Entry next_node = choose_node(pr);
            dk = knn_node_search(next_node.node_ptr_val, query, k, nn_array, pr, dk);
        }
        
        return nn_array;
    }
    
private:
    // Método para elegir el siguiente nodo de la cola de prioridad
    PR_Entry choose_node(std::priority_queue<PR_Entry>& pr) {
        PR_Entry next = pr.top();
        pr.pop();
        return next;
    }
    
    // Implementación del método k-NN NodeSearch
    distance_type knn_node_search(node_ptr N, const entry<Params>& Q, int k, 
                                 std::vector<NN>& nn_array, std::priority_queue<PR_Entry>& pr, 
                                 distance_type dk) {
        
        entry_ptr parent_entry = N->get_father_entry(); // Op
        
        if(!N->is_leaf()) {
            // Nodo interno
            for(int i = 0; i < Params::cap_node + 1; i++) {
                entry_ptr Or = N->get_entry(i);
                if(!Or) continue;
                
                // Optimización usando distancia pre-computada del padre
                distance_type d_parent_query = 0.0;
                distance_type d_or_parent = 0.0;
                
                if(parent_entry) {
                    d_parent_query = parent_entry->distance_to(Q, metric);
                    d_or_parent = Or->distance_to(*parent_entry, metric);
                }
                
                // Verificar condición de poda
                if(!parent_entry || std::abs(d_parent_query - d_or_parent) <= dk + Or->get_cover_radius()) {
                    // Computar d(Or, Q)
                    distance_type d_or_query = Or->distance_to(Q, metric);
                    
                    // Calcular dmin para el sub-árbol
                    distance_type dmin_subtree = std::max(0.0, d_or_query - Or->get_cover_radius());
                    
                    if(dmin_subtree <= dk) {
                        // Agregar sub-árbol a PR
                        pr.push(PR_Entry(Or->get_cover_tree(), dmin_subtree));
                        
                        // Calcular dmax para el sub-árbol
                        distance_type dmax_subtree = d_or_query + Or->get_cover_radius();
                        
                        if(dmax_subtree < dk) {
                            // Actualizar NN array con bound
                            dk = nn_update(nn_array, NN(dmax_subtree), k);
                            
                            // Remover entradas de PR que ya no son relevantes
                            remove_irrelevant_entries(pr, dk);
                        }
                    }
                }
            }
        }
        else {
            // Nodo hoja
            for(int i = 0; i < Params::cap_node + 1; i++) {
                entry_ptr Oj = N->get_entry(i);
                if(!Oj) continue;
                
                // Optimización usando distancia pre-computada del padre
                distance_type d_parent_query = 0.0;
                distance_type d_oj_parent = 0.0;
                
                if(parent_entry) {
                    d_parent_query = parent_entry->distance_to(Q, metric);
                    d_oj_parent = Oj->distance_to(*parent_entry, metric);
                }
                
                // Verificar condición de poda
                if(!parent_entry || std::abs(d_parent_query - d_oj_parent) <= dk) {
                    // Computar d(Oj, Q)
                    distance_type d_oj_query = Oj->distance_to(Q, metric);
                    
                    if(d_oj_query <= dk) {
                        // Actualizar NN array con objeto real
                        dk = nn_update(nn_array, NN(Oj->oid, d_oj_query), k);
                        
                        // Remover entradas de PR que ya no son relevantes
                        remove_irrelevant_entries(pr, dk);
                    }
                }
            }
        }
        
        return dk;
    }
    
    // Método para actualizar el array de vecinos más cercanos
    distance_type nn_update(std::vector<NN>& nn_array, const NN& new_entry, int k) {
        // Inserción ordenada en el array NN
        int insert_pos = -1;
        
        // Encontrar posición de inserción
        for(int i = 0; i < k; i++) {
            if(new_entry.distance < nn_array[i].distance) {
                insert_pos = i;
                break;
            }
        }
        
        // Si encontramos posición, hacer inserción ordenada
        if(insert_pos != -1) {
            // Desplazar elementos hacia la derecha
            for(int i = k - 1; i > insert_pos; i--) {
                nn_array[i] = nn_array[i-1];
            }
            // Insertar nuevo elemento
            nn_array[insert_pos] = new_entry;
        }
        
        // Retornar nuevo dk (distancia del k-ésimo vecino más cercano)
        return nn_array[k-1].distance;
    }
    
    // Método para remover entradas irrelevantes de la cola de prioridad
    void remove_irrelevant_entries(std::priority_queue<PR_Entry>& pr, distance_type dk) {
        std::priority_queue<PR_Entry> new_pr;
        
        while(!pr.empty()) {
            PR_Entry entry = pr.top();
            pr.pop();
            
            if(entry.dmin <= dk) {
                new_pr.push(entry);
            }
        }
        
        pr = std::move(new_pr);
    }
    
    // Método auxiliar para obtener la raíz (necesitarás agregarlo a tu clase m_tree)
    node_ptr get_root() {
        // Necesitarás hacer público el campo root_ en tu clase m_tree
        // o agregar un método get_root() público
        // Por ahora asumo que tienes acceso a root_
        return tree_ptr->root_; // Esto requerirá hacer root_ público o agregar getter
    }
};

// Función de conveniencia para usar la extensión
template<typename Params>
std::vector<NearestNeighbor<Params>> perform_knn_search(m_tree<Params>& tree, 
                                                        const entry<Params>& query, 
                                                        int k) {
    knn_search_extension<Params> knn_searcher(&tree);
    return knn_searcher.knn_search(query, k);
}

#endif // KNN_SEARCH_HPP
