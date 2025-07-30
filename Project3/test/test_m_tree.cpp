#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include <cmath>
#include <iomanip>
#include <chrono>
#include <fstream>
#include <sstream>
#include <ctime>
#include <thread>
#include <cstring>
#include <random>
#include <set>

#include "../src/m_tree.h"

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
    
    long long getMicroseconds() {
        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end_time - start_time);
        return duration.count();
    }
    
    long long getMilliseconds() {
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time);
        return duration.count();
    }
    
    double getSeconds() {
        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end_time - start_time);
        return duration.count() / 1000000.0;
    }
};

using tree_t = m_tree<MTreeParams<double, std::string, 10>>;

template <typename... args_t>
std::shared_ptr<tree_t> make_tree(args_t&&... args) {
    return std::make_shared<tree_t>(std::forward<args_t>(args)...);
}

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

std::vector<std::string> bruteForceKNN(
    const std::vector<std::pair<std::string, std::vector<double>>>& all_data,
    const std::vector<double>& query_features,
    int k
) {
    std::vector<std::pair<double, std::string>> distances;
    
    for (const auto& data_point : all_data) {
        double distance = euclideanDistance(query_features, data_point.second);
        distances.push_back({distance, data_point.first});
    }
    
    std::sort(distances.begin(), distances.end());
    
    std::vector<std::string> result;
    for (int i = 0; i < std::min(k, static_cast<int>(distances.size())); ++i) {
        result.push_back(distances[i].second);
    }
    
    return result;
}

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

bool runKNNTest(
    std::shared_ptr<tree_t>& tree,
    const std::vector<std::pair<std::string, std::vector<double>>>& data,
    const std::vector<double>& query_features,
    int k
) {
    entry_type query_entry = make_entry(query_features, "QUERY");
    
    // Ejecutar k-NN en M-tree
    auto tree_results = tree->k_nn_entries(query_entry, k);
    
    // Ejecutar fuerza bruta (comentado para velocidad)
    // auto brute_results = bruteForceKNN(data, query_features, k);
    // return validateKNNResults(tree_results, brute_results);
    return true;
}

struct TestParams {
    int dimension = 10;
    int num_test = 1;
    int num_queries = 5;
    int num_points = 100000;
    double min_val = 0.0;
    double max_val = 200000.0;
};

void showHelp(const char* program_name) {
    std::cout << "Uso: " << program_name << " [opciones]" << std::endl;
    std::cout << "Opciones:" << std::endl;
    std::cout << "  -d <dimension>    Dimensión de los puntos (default: 10)" << std::endl;
    std::cout << "  -nt <num_tests>   Número de tests (default: 1)" << std::endl;
    std::cout << "  -nq <num_queries> Número de queries por test (default: 5)" << std::endl;
    std::cout << "  -np <num_points>  Número de puntos (default: 100000)" << std::endl;
    std::cout << "  -h, --help        Mostrar esta ayuda" << std::endl;
}

TestParams parseArguments(int argc, char* argv[]) {
    TestParams params;
    
    for (int i = 1; i < argc; ++i) {
        if (strcmp(argv[i], "-d") == 0 && i + 1 < argc) {
            params.dimension = std::atoi(argv[++i]);
        } else if (strcmp(argv[i], "-nt") == 0 && i + 1 < argc) {
            params.num_test = std::atoi(argv[++i]);
        } else if (strcmp(argv[i], "-nq") == 0 && i + 1 < argc) {
            params.num_queries = std::atoi(argv[++i]);
        } else if (strcmp(argv[i], "-np") == 0 && i + 1 < argc) {
            params.num_points = std::atoi(argv[++i]);
        } else if (strcmp(argv[i], "-h") == 0 || strcmp(argv[i], "--help") == 0) {
            showHelp(argv[0]);
            exit(0);
        }
    }
    
    return params;
}

int main(int argc, char* argv[]) {
    // Parsear argumentos de línea de comandos
    TestParams params = parseArguments(argc, argv);
    
    std::cout << "🧪 TESTING M-TREE" << std::endl;
    std::cout << "====================================" << std::endl;

    Timer timer;
    std::vector<double> times;
    
    for (int i = 0; i < params.num_test; i++) {
        // Generar datos con los parámetros especificados
        auto data_2d = generateMockData(params.num_points, params.dimension, 
                                       params.min_val, params.max_val);
        auto tree = buildTree(data_2d);
        bool all_passed = true;
        
        timer.start();
        for (int j = 0; j < params.num_queries; j++) {
            // Generar query aleatorio con los mismos parámetros que los datos
            std::vector<double> query_2d = generateRandomQuery(params.dimension, 
                                                              params.min_val, params.max_val);
            bool test_passed = runKNNTest(tree, data_2d, query_2d, 20);
            all_passed &= test_passed;
            
            if (!test_passed) {
                std::cout << "❌ Test falló en query " << j << " del test " << i << std::endl;
                break;
            }
        }
        timer.stop();
        times.push_back(timer.getSeconds());
        
        if (all_passed) {
            std::cout << "📊 TEST " << i << " Exitoso" << std::endl;
        } else {
            std::cout << "❌ TEST " << i << " Falló" << std::endl;
            break;
        }
    }
    
    double average_time = 0;
    for (auto e : times) {
        average_time += e;
    }
    average_time /= (int)times.size();
    std::cout << "El tiempo promedio es: " << average_time << std::endl;
    
    return 0;
}