#include <iostream>
#include <memory>
#include <vector>
#include <string>

// Forward declarations
template <typename Params> class node;
template <typename Params> class entry;

// MTreeParams
template <typename features = double, typename identifier = std::string, int capacity = 3>
struct MTreeParams {
    using feature_type = features;
    using identifier_type = identifier;
    static constexpr int cap_node = capacity;
};

// Clase node simple
template <typename Params>
class node {
public:
    std::string name;
    node(const std::string& n = "Node") : name(n) {}
};

// Clase entry base (NO abstracta)
template <typename Params>
class entry {
public:
    using feature_type = typename Params::feature_type;
    using identifier_type = typename Params::identifier_type;
    using node_ptr = std::shared_ptr<node<Params>>;
    
    identifier_type oid;
    int pos;
    
protected:
    std::vector<feature_type> features_;
    
public:
    entry(const std::vector<feature_type>& f, const identifier_type& o) 
        : features_(f), oid(o), pos(0) {}
    
    virtual ~entry() = default;
    
    const std::vector<feature_type>& get_features() const { return features_; }
    
    virtual void print_info() const {
        std::cout << "Entry - ID: " << oid << ", Features: [";
        for(size_t i = 0; i < features_.size(); ++i) {
            std::cout << features_[i];
            if(i < features_.size() - 1) std::cout << ", ";
        }
        std::cout << "]" << std::endl;
    }
    
    virtual std::string get_type() const { return "Entry"; }
};

// internal_entry
template <typename Params>
class internal_entry : public entry<Params> {
public: 
    using node_ptr = std::shared_ptr<node<Params>>;
    
    node_ptr cover_tree;
    double radio_covertura;
    
public: 
    internal_entry(const std::vector<typename Params::feature_type>& f, 
                   const typename Params::identifier_type& o, 
                   node_ptr c, double r)
        : entry<Params>(f, o), cover_tree(c), radio_covertura(r) {}
    
    void print_info() const override {
        std::cout << "Internal Entry - ID: " << this->oid << ", Features: [";
        const auto& features = this->get_features();
        for(size_t i = 0; i < features.size(); ++i) {
            std::cout << features[i];
            if(i < features.size() - 1) std::cout << ", ";
        }
        std::cout << "], Radio: " << radio_covertura;
        if(cover_tree) {
            std::cout << ", Node: " << cover_tree->name;
        }
        std::cout << std::endl;
    }
    
    std::string get_type() const override { return "Internal"; }
};

// leaf_entry
template <typename Params>
class leaf_entry : public entry<Params> { 
public:
    leaf_entry(const std::vector<typename Params::feature_type>& f,
               const typename Params::identifier_type& o) 
        : entry<Params>(f, o) {}
    
    void print_info() const override {
        std::cout << "Leaf Entry - ID: " << this->oid << ", Features: [";
        const auto& features = this->get_features();
        for(size_t i = 0; i < features.size(); ++i) {
            std::cout << features[i];
            if(i < features.size() - 1) std::cout << ", ";
        }
        std::cout << "]" << std::endl;
    }
    
    std::string get_type() const override { return "Leaf"; }
};

int main() {
    using MyParams = MTreeParams<double, std::string, 3>;
    
    std::cout << "=== Creando objetos básicos ===" << std::endl;
    
    // 1. Crear entry básico
    std::vector<double> features1 = {1.0, 2.0, 3.0};
    auto entry1 = std::make_shared<entry<MyParams>>(features1, "entry_001");
    
    // 2. Crear internal_entry
    auto node1 = std::make_shared<node<MyParams>>("Node_A");
    std::vector<double> features2 = {4.0, 5.0, 6.0};
    auto internal1 = std::make_shared<internal_entry<MyParams>>(features2, "internal_001", node1, 2.5);
    
    // 3. Crear leaf_entry
    std::vector<double> features3 = {7.0, 8.0, 9.0};
    auto leaf1 = std::make_shared<leaf_entry<MyParams>>(features3, "leaf_001");
    
    std::cout << "=== Vector polimórfico ===" << std::endl;
    
    // 4. Vector con todos los tipos
    std::vector<std::shared_ptr<entry<MyParams>>> entries;
    entries.push_back(entry1);
    entries.push_back(internal1);
    entries.push_back(leaf1);
    
    std::cout << "=== Iterando e imprimiendo ===" << std::endl;
    
    // 5. Iterar con índices
    for(size_t i = 0; i < entries.size(); ++i) {
        std::cout << "\n[" << i << "] Tipo: " << entries[i]->get_type() << std::endl;
        entries[i]->print_info();
    }
    
    std::cout << "\n=== Range-based for ===" << std::endl;
    
    // 6. Iterar con range-based for
    for(const auto& e : entries) {
        std::cout << "\nTipo: " << e->get_type() << std::endl;
        e->print_info();
        
        // Identificar tipo con dynamic_cast
        if (auto internal = std::dynamic_pointer_cast<internal_entry<MyParams>>(e)) {
            std::cout << "  -> Es internal, radio: " << internal->radio_covertura << std::endl;
        } else if (auto leaf = std::dynamic_pointer_cast<leaf_entry<MyParams>>(e)) {
            std::cout << "  -> Es leaf" << std::endl;
        } else {
            std::cout << "  -> Es entry base" << std::endl;
        }
    }
    
    std::cout << "\n=== Listo! ===" << std::endl;
    return 0;
}
