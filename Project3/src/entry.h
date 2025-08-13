#ifndef ENTRY_HPP
#define ENTRY_HPP
#include <memory>
#include <vector>
#include <functional>
#include <cmath>
#include <string>  

template <typename T>
inline void m_tree_swap_helper(T& a, T& b) {
	using std::swap;
	swap(a, b);
}

template <typename Params> class node;
template <typename Params> class metric_space;
template <typename Params> class entry;

template <typename features = double, typename identifier = std::string, int capacity = 10>
struct MTreeParams {
    using feature_type = features;
    using identifier_type = identifier;
    static constexpr int cap_node = capacity;
    static void swap(std::shared_ptr<entry<MTreeParams>>& a, std::shared_ptr<entry<MTreeParams>>&b) {
        m_tree_swap_helper(a, b);
    }
};

template <typename Params>
class entry {
public:
    using feature_type = typename Params::feature_type;
    using identifier_type = typename Params::identifier_type;
    using distance_type = typename Params::feature_type;
    using metric_space_t = metric_space<Params>; 
    using node_ptr = std::shared_ptr<node<Params>>;

    identifier_type oid;
    std::vector<feature_type> features_;
    distance_type dis;
    int pos = 0;

    // Campos para internal_entry
    node_ptr cover_tree = nullptr;
    distance_type radio_covertura = 0.0;
    
    // Constructor para entry básico
    entry(const std::vector<feature_type>& f, const identifier_type& o) 
        : features_(f), oid(o), dis(0), pos(0) {}

    entry(entry base_entry, node_ptr new_cover, distance_type new_r)
        : features_(base_entry.features_), oid(base_entry.oid), dis(base_entry.dis), 
          pos(base_entry.pos), cover_tree(new_cover), radio_covertura(new_r) {}

    
    void set_pos(int new_pos) { pos = new_pos; }
    int get_pos() { return pos; }
    virtual ~entry() = default;
    
    void set_identifier(const identifier_type& new_o) noexcept { oid = new_o; }
    

    // Métodos para manejar cover_tree y radio_covertura
    node_ptr get_cover_tree() { return cover_tree; }
    void set_cover_tree(node_ptr new_cover) { cover_tree = new_cover; }
    distance_type get_cover_radius() { return radio_covertura; }
    void set_cover_radius(distance_type radius) { radio_covertura = radius; }
    
    // Método para verificar si es internal entry
    bool is_internal() const { return cover_tree != nullptr; }
    
    // Método para verificar si es leaf entry
    bool is_leaf() const { return cover_tree == nullptr; }

    distance_type distance_to(const entry& other, const metric_space_t& metric = metric_space_t()) const {
        return metric(features_, other.features_);
    }

};

template <typename Params>
class metric_space {
public:
    using feature_type = typename Params::feature_type;
    using distance_type = typename Params::feature_type;
    using feature_vector = std::vector<feature_type>;
    using distance_function = std::function<distance_type(const feature_vector&, const feature_vector&)>;
private:
    distance_function metric;
public:
    static distance_type euclidean(const feature_vector& a, const feature_vector& b) {
        distance_type sum = 0.0;
        for (size_t i = 0; i < a.size(); ++i) {
            auto diff = a[i] - b[i];
            sum += diff * diff;
        }
        return std::sqrt(sum);
    }
    static distance_type manhattan(const feature_vector& a, const feature_vector& b) {
        distance_type sum = 0.0;
        for (size_t i = 0; i < a.size(); ++i) {
            sum += std::abs(a[i] - b[i]);
        }
        return sum;
    }
    metric_space(distance_function func = euclidean) : metric(func) {}
    distance_type operator()(const feature_vector& a, const feature_vector& b) const {
        return metric(a, b);
    }
};

#endif // ENTRY_HPP
