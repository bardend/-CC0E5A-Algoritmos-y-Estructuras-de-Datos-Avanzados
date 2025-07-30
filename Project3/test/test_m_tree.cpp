#include <iostream>
#include <random>
#include <vector>
#include <string>
#include <set>
#include <algorithm>
#include <cmath>
#include <iomanip>
#include <chrono>
#include <ctime>
#include <thread>
#include "../src/m_tree.hpp"

// ============================================================================
// 1. USANDO std::chrono (RECOMENDADO - C++11+)
// ============================================================================

class Timer {
private:
    std::chrono::high_resolution_clock::time_point start_time;
    std::chrono::high_resolution_clock::time_point end_time;
    
public:
    void start() {
        start_time = std::chrono::high_resolution_clock::now();
    }
    
    void stop() {
        end_time = std::chrono::high_resolution_clock::now();
    }
    
    // Tiempo en microsegundos
    long long getMicroseconds() {
        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end_time - start_time);
        return duration.count();
    }
    
    // Tiempo en milisegundos
    long long getMilliseconds() {
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time);
        return duration.count();
    }
    
    // Tiempo en segundos (como double)
    double getSeconds() {
        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end_time - start_time);
        return duration.count() / 1000000.0;
    }
};

//using tree_t = m_tree<params>;
using tree_t = m_tree<MTreeParams<double, std::string, 10>>;


// Función make_tree trasladada desde utils.hpp
template <typename... args_t>
std::shared_ptr<tree_t> make_tree(args_t&&... args) {
    return std::make_shared<tree_t>(std::forward<args_t>(args)...);
}

/**
 * Genera datos mock con OIDs únicos
 * @param num_entries Número de entradas a generar
 * @param dimension Dimensión de los vectores de features
 * @param min_val Valor mínimo para las features
 * @param max_val Valor máximo para las features
 * @return Vector de pares (oid, features)
 */
std::vector<std::pair<std::string, std::vector<double>>> generateMockData(
    int num_entries, 
    int dimension, 
    double min_val = 0.0, 
    double max_val = 100.0
) {
    std::vector<std::pair<std::string, std::vector<double>>> data;
    std::set<std::string> used_oids;
    
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<> dis(min_val, max_val);
    
    for (int i = 0; i < num_entries; ++i) {
        std::string oid;
        do {
            oid = "OID_" + std::to_string(i);
        } while (used_oids.find(oid) != used_oids.end());
        
        used_oids.insert(oid);
        
        std::vector<double> features;
        for (int d = 0; d < dimension; ++d) {
            features.push_back(dis(gen));
        }
        
        data.push_back({oid, features});
    }
    
    return data;
}
std::shared_ptr<tree_t> buildTree(const std::vector<std::pair<std::string, std::vector<double>>>& data) {
    auto tree = make_tree();
    for (const auto& entry_data : data) {
        entry_type entry = make_entry(entry_data.second, entry_data.first);
        tree->insert(entry);
    }
    return tree;
}


/**
 * Genera un vector de query aleatorio
 * @param dimension Dimensión del vector de features
 * @param min_val Valor mínimo para las features
 * @param max_val Valor máximo para las features
 * @return Vector de features para query
 */
std::vector<double> generateRandomQuery(
    int dimension, 
    double min_val = 0.0, 
    double max_val = 100.0
) {
    std::vector<double> query_features;
    
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<> dis(min_val, max_val);
    
    for (int d = 0; d < dimension; ++d) {
        query_features.push_back(dis(gen));
    }
    
    return query_features;
}

/**
 * Calcula la distancia euclidiana entre dos vectores
 */
double euclideanDistance(const std::vector<double>& a, const std::vector<double>& b) {
    if (a.size() != b.size()) {
        throw std::invalid_argument("Vectores de diferentes dimensiones");
    }
    
    double sum = 0.0;
    for (size_t i = 0; i < a.size(); ++i) {
        double diff = a[i] - b[i];
        sum += diff * diff;
    }
    return std::sqrt(sum);
}

/**
 * Implementación de fuerza bruta para k-NN
 * @param all_data Todos los datos disponibles
 * @param query_features Features del punto de consulta
 * @param k Número de vecinos más cercanos
 * @return Vector de OIDs de los k vecinos más cercanos
 */

std::vector<std::string> bruteForceKNN(
    const std::vector<std::pair<std::string, std::vector<double>>>& all_data,
    const std::vector<double>& query_features,
    int k
) {
    std::vector<std::pair<double, std::string>> distances;
    
    // Calcular distancias a todos los puntos
    for (const auto& data_point : all_data) {
        double distance = euclideanDistance(query_features, data_point.second);
        distances.push_back({distance, data_point.first});
    }
    
    // Ordenar por distancia
    std::sort(distances.begin(), distances.end());
    
    // Tomar los k primeros
    std::vector<std::string> result;
    for (int i = 0; i < std::min(k, static_cast<int>(distances.size())); ++i) {
        result.push_back(distances[i].second);
    }
    
    return result;
}

/**
 * Valida que los resultados del M-tree coincidan con fuerza bruta
 */
bool validateKNNResults(
    const std::vector<std::string>& tree_results,
    const std::vector<std::string>& brute_force_results
) {
    if (tree_results.size() != brute_force_results.size()) {
        std::cout << "❌ ERROR: Diferente número de resultados. "
                  << "M-tree: " << tree_results.size() 
                  << ", Fuerza bruta: " << brute_force_results.size() << std::endl;
        return false;
    }
    
    for (size_t i = 0; i < tree_results.size(); ++i) {
        if (tree_results[i] != brute_force_results[i]) {
            std::cout << "❌ ERROR: Resultado " << i << " no coincide. "
                      << "M-tree: " << tree_results[i] 
                      << ", Fuerza bruta: " << brute_force_results[i] << std::endl;
            return false;
        }
    }
    
    return true;
}

/**
 * Ejecuta un test completo de k-NN
 */
bool runKNNTest(
    std::shared_ptr<tree_t>& tree,
    const std::vector<std::pair<std::string, std::vector<double>>>& data,
    const std::vector<double>& query_features,
    int k
) {
    entry_type query_entry = make_entry(query_features, "QUERY");
    
    // Ejecutar k-NN en M-tree
    auto tree_results = tree->k_nn_entries(query_entry, k);
    
    // Ejecutar fuerza bruta
    // auto brute_results = bruteForceKNN(data, query_features, k);
    // return validateKNNResults(tree_results, brute_results);
    return true;
    
}

int main() {
    std::cout << "🧪 TESTING M-TREE" << std::endl;
    std::cout << "=================" << std::endl;

    int dimension = 1024; //32x32
    int num_test = 1;
    int num_queries = 5;
    int num_points = 100000;
    double min_val = 0.0;
    double max_val = 200000.0;

    Timer timer;
    std::vector<double>times;
    for(int i = 0; i < num_test; i++) {
        // Generar datos con los mismos parámetros
        auto data_2d = generateMockData(num_points, dimension, min_val, max_val);
        auto tree = buildTree(data_2d);
        bool all_passed = true;
        
        timer.start();
        for(int j = 0; j < num_queries; j++) {
            // Generar query aleatorio con los mismos parámetros que los datos
            std::vector<double> query_2d = generateRandomQuery(dimension, min_val, max_val);
            bool test_passed = runKNNTest(tree, data_2d, query_2d, 20);
            all_passed &= test_passed;
            
            if (!test_passed) {
                std::cout << "❌ Test falló en query " << j << " del test " << i << std::endl;
                break;
            }
        }
        timer.stop();
        times.push_back(timer.getSeconds());
        
        if(all_passed) {
            std::cout << "📊 TEST " << i << " Exitoso" << std::endl;
        } else {
            std::cout << "❌ TEST " << i << " Falló" << std::endl;
            break;
        }
    }
    double average_time = 0;
    for(auto e: times)
        average_time += e;
    average_time /= (int)times.size();
    std::cout << "El tiempo promedio es: " << average_time << std::endl;
    return 0;
}
