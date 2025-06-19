#ifndef M_TREE_H
#define M_TREE_H

#include <vector>
#include <string>
#include <memory>
#include <cmath>
#include <limits>
#include <algorithm>
#include <queue>
#include <unordered_set>

class CoverTree;

class Entry {
public:
    double x, y;
    double radio_cobertura;
    std::shared_ptr<CoverTree> cover_tree;
    std::string identifier;

    Entry(double x, double y, double r, const std::string& id)
        : x(x), y(y), radio_cobertura(r), identifier(id), cover_tree(nullptr) {}

    double distance(const Entry& other) const {
        double dx = x - other.x;
        double dy = y - other.y;
        return std::sqrt(dx*dx + dy*dy);
    }

    bool operator<(const Entry& other) const {
        return this->identifier < other.identifier;
    }
};

class CoverTree {
private:
    std::shared_ptr<class Node> root_;

public:
    CoverTree() : root_(nullptr) {}

    void set_root(std::shared_ptr<class Node> root) {
        root_ = root;
    }

    std::shared_ptr<class Node> get_root() const {
        return root_;
    }
};

class Node {
public:
    std::vector<Entry> entries;
    std::shared_ptr<Node> father;
    std::shared_ptr<CoverTree> cover_tree;

    Node() : father(nullptr), cover_tree(nullptr) {}

    std::shared_ptr<Node> get_father() const { return father; }
    std::shared_ptr<CoverTree> get_cover_tree() const { return cover_tree; }

    void insert_entry(const Entry& entry) {
        entries.push_back(entry);
    }

    bool is_leaf() const {
        for (const auto& entry : entries) {
            if (entry.cover_tree != nullptr) {
                return false;
            }
        }
        return true;
    }
};

class M_tree {
private:
    std::shared_ptr<Node> root_;
    size_t size_nodes;
    size_t size_entries;
    size_t max_entries;

    struct EntryDistance {
        Entry entry;
        double distance;
        
        EntryDistance(const Entry& e, double d) : entry(e), distance(d) {}
        
        bool operator<(const EntryDistance& other) const {
            return distance < other.distance;
        }
    };

public:
    M_tree(size_t max_entries_per_node = 3) 
        : root_(std::make_shared<Node>()), size_nodes(1), size_entries(0), max_entries(max_entries_per_node) {}

    void insert(const Entry& entry) {
        if (!root_) {
            root_ = std::make_shared<Node>();
            size_nodes = 1;
        }
        
        std::shared_ptr<Node> node = choose_leaf(root_, entry);
        node->insert_entry(entry);
        size_entries++;
        
        if (node->entries.size() > max_entries) {
            split_node(node);
        }
    }

    std::vector<Entry> knn_search(const Entry& query, int k) {
        std::priority_queue<EntryDistance> pq;
        std::unordered_set<std::string> visited;
        
        knn_search_recursive(root_, query, k, pq, visited);
        
        std::vector<Entry> results;
        while (!pq.empty() && results.size() < static_cast<size_t>(k)) {
            results.push_back(pq.top().entry);
            pq.pop();
        }
        
        std::reverse(results.begin(), results.end());
        return results;
    }

    size_t get_size_nodes() const { return size_nodes; }
    size_t get_size_entries() const { return size_entries; }

private:
    std::shared_ptr<Node> choose_leaf(std::shared_ptr<Node> node, const Entry& entry) {
        if (node->is_leaf()) {
            return node;
        }
        
        double min_increase = std::numeric_limits<double>::max();
        std::shared_ptr<Node> best_node = nullptr;
        
        for (auto& e : node->entries) {
            if (e.cover_tree) {
                double dist = e.distance(entry);
                double increase = std::max(0.0, dist - e.radio_cobertura);
                
                if (increase < min_increase) {
                    min_increase = increase;
                    best_node = e.cover_tree->get_root();
                }
            }
        }
        
        if (best_node) {
            return choose_leaf(best_node, entry);
        }
        
        return node;
    }

    void split_node(std::shared_ptr<Node> node) {
        auto new_node = std::make_shared<Node>();
        new_node->father = node->father;
        
        size_t split_pos = node->entries.size() / 2;
        new_node->entries.insert(new_node->entries.end(), 
                               node->entries.begin() + split_pos, 
                               node->entries.end());
        node->entries.erase(node->entries.begin() + split_pos, node->entries.end());
        
        size_nodes++;
        
        if (!node->father) {
            auto new_root = std::make_shared<Node>();
            
            Entry e1(0, 0, calculate_covering_radius(node), "router1");
            e1.cover_tree = std::make_shared<CoverTree>();
            e1.cover_tree->set_root(node);
            new_root->insert_entry(e1);
            
            Entry e2(0, 0, calculate_covering_radius(new_node), "router2");
            e2.cover_tree = std::make_shared<CoverTree>();
            e2.cover_tree->set_root(new_node);
            new_root->insert_entry(e2);
            
            node->father = new_root;
            new_node->father = new_root;
            root_ = new_root;
            size_nodes++;
        } else {
            Entry e(0, 0, calculate_covering_radius(new_node), "router_" + std::to_string(size_nodes));
            e.cover_tree = std::make_shared<CoverTree>();
            e.cover_tree->set_root(new_node);
            node->father->insert_entry(e);
            
            if (node->father->entries.size() > max_entries) {
                split_node(node->father);
            }
        }
    }

    double calculate_covering_radius(std::shared_ptr<Node> node) {
        double max_radius = 0.0;
        for (const auto& entry : node->entries) {
            if (entry.cover_tree) {
                double radius = entry.radio_cobertura + calculate_covering_radius(entry.cover_tree->get_root());
                if (radius > max_radius) {
                    max_radius = radius;
                }
            }
        }
        return max_radius;
    }

    void knn_search_recursive(std::shared_ptr<Node> node, const Entry& query, int k,
                            std::priority_queue<EntryDistance>& pq,
                            std::unordered_set<std::string>& visited) {
        if (!node) return;
        
        for (const auto& entry : node->entries) {
            if (visited.count(entry.identifier)) continue;
            visited.insert(entry.identifier);
            
            double dist = entry.distance(query);
            pq.push(EntryDistance(entry, dist));
            
            if (pq.size() > static_cast<size_t>(k)) {
                pq.pop();
            }
            
            if (entry.cover_tree) {
                double bound = dist + entry.radio_cobertura;
                if (pq.size() < static_cast<size_t>(k) || bound <= pq.top().distance) {
                    knn_search_recursive(entry.cover_tree->get_root(), query, k, pq, visited);
                }
            }
        }
    }
};

#endif // M_TREE_H
