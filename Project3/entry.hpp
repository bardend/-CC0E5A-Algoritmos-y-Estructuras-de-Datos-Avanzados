#pragma once
#include <memory>
#include <vector>
#include <functional>
#include <cmath>
#include "m_tree.hpp"

template <typename features = double, typename identifier = std::string, int cap_node = 3>
struct MTreeParams {
    using feature_type = features;
    using identifier_type = identifier;
};

template <typename Params> class node;
template <typename Params> class metric_space;

template <typename Params>
class entry {
public:
    using feature_type = typename Params::feature_type;
    using identifier_type = typename Params::identifier_type;
    using distance_type = typename Params::feature_type; // Retorna lo mismo que los features :)
    using metric_space_t = metric_space<Params>; 
    identifier_type oid;

protected:
    std::vector<feature_type> features_;
    distance_type dis;

public:
    entry(const std::vector<feature_type>& f, const identifier_type& o) 
        : features_(f), oid(o), dis(0) {}

    virtual ~entry() = default;

    void set_identifier(const identifier_type& new_o) noexcept { oid = new_o; }

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

template <typename Params>
class internal_entry : public entry<Params> {
public: 
    using node_ptr = std::unique_ptr<node<Params>>;
    using distance_type = typename entry<Params>::distance_type;

private:
    node_ptr cover_tree;
    distance_type radio_covertura;

public: 
    internal_entry(const entry<Params>& base_entry, node_ptr new_cover, distance_type new_r)
        : entry<Params>(base_entry), cover_tree(std::move(new_cover)), radio_covertura(new_r) {}
};

template <typename Params>
class leaf_entry : public entry<Params> { 
public:
    leaf_entry(const entry<Params>& base_entry) : entry<Params>(base_entry) {}
};
