#include <iostream>
#include <vector>
#include <string>
#include <cmath>
#include <random>
#include <chrono>
#include <map>
#include <algorithm>
#include <iomanip>
#include <set>
#include <fstream>

// Incluir el M-Tree existente
#include "../src/m_tree.h"

// Usar los tipos definidos en el proyecto (config.h ya incluido por m_tree.h)
using tree_t = m_tree<MTreeConfig>;

// Función helper del test original
template <typename... args_t>
std::shared_ptr<tree_t> make_tree(args_t&&... args) {
    return std::make_shared<tree_t>(std::forward<args_t>(args)...);
}

// Estructura para métricas de anomalías
struct Metricas {
    double tiempo_ms;
    int nivel_anomalia;
    double distancia;
    bool es_anomalia;
    std::string id;
};

// ===== DETECTOR DE ANOMALÍAS =====

class DetectorAnomalias {
private:
    std::shared_ptr<tree_t> arbol;
    std::vector<Metricas> historial;
    
public:
    DetectorAnomalias(std::shared_ptr<tree_t> a) : arbol(a) {}
    
    Metricas insertar_y_detectar(entry_type elem) {
        Metricas m;
        m.id = elem->oid;
        
        // Detectar nivel de anomalía antes de insertar
        m.nivel_anomalia = detectar_nivel(elem);
        m.es_anomalia = (m.nivel_anomalia > 0);
        
        // Medir tiempo de inserción
        auto inicio = std::chrono::high_resolution_clock::now();
        arbol->insert(elem);
        auto fin = std::chrono::high_resolution_clock::now();
        
        auto duracion = std::chrono::duration_cast<std::chrono::microseconds>(fin - inicio);
        m.tiempo_ms = duracion.count() / 1000.0;
        
        // Calcular distancia aproximada (simplificado)
        m.distancia = 0.0;
        
        historial.push_back(m);
        return m;
    }
    
    int detectar_nivel(entry_type elem) {
        if (!arbol || !arbol->get_root()) {
            return 1; // Anomalía si no hay árbol
        }
        return detectar_recursivo(elem, arbol->get_root(), 1);
    }
    
    void imprimir_estadisticas() {
        if (historial.empty()) {
            std::cout << "No hay métricas." << std::endl;
            return;
        }
        
        std::cout << "=== ESTADÍSTICAS ===" << std::endl;
        std::cout << "Total: " << historial.size() << std::endl;
        
        // Contar por niveles
        std::map<int, int> conteo;
        double tiempo_total = 0;
        
        for (const auto& m : historial) {
            conteo[m.nivel_anomalia]++;
            tiempo_total += m.tiempo_ms;
        }
        
        std::cout << "\nNiveles de anomalía:" << std::endl;
        for (const auto& par : conteo) {
            double porcentaje = (100.0 * par.second) / historial.size();
            std::cout << "  Nivel " << par.first << ": " << par.second 
                      << " (" << std::fixed << std::setprecision(1) << porcentaje << "%)" << std::endl;
        }
        
        std::cout << "\nTiempo promedio: " 
                  << std::fixed << std::setprecision(3) 
                  << tiempo_total / historial.size() << " ms" << std::endl;
        
        std::cout << "Altura del árbol: " << calcular_altura_arbol() << std::endl;
    }
    
    std::vector<Metricas> obtener_metricas() {
        return historial;
    }
    
    int calcular_altura_arbol() {
        if (!arbol || !arbol->get_root()) return 0;
        return calcular_altura_recursiva(arbol->get_root());
    }

private:
    using node_ptr = std::shared_ptr<node_t>;
    
    int detectar_recursivo(entry_type elem, node_ptr nodo, int nivel) {
        if (!nodo) return nivel;
        
        if (nodo->is_leaf()) {
            // En hoja, verificar si puede encajar
            for (int i = 0; i < nodo->size_entry(); ++i) {
                auto entrada_existente = nodo->get_entry(i);
                double dist = elem->distance_to(*entrada_existente);
                if (dist <= entrada_existente->radio_covertura + 5.0) { // Tolerancia
                    return 0; // No es anomalía
                }
            }
            // No encaja en ningún elemento existente
            int altura_arbol = calcular_altura_arbol();
            int max_nivel_anomalia = static_cast<int>(std::log2(altura_arbol)) + 1;
            return std::min(nivel, max_nivel_anomalia);
        }
        
        // En nodo interno, buscar en hijos
        for (int i = 0; i < nodo->size_entry(); ++i) {
            auto hijo = nodo->cover_tree(i);
            if (hijo) {
                int resultado = detectar_recursivo(elem, hijo, nivel + 1);
                if (resultado == 0) {
                    return 0; // Encontró lugar normal
                }
            }
        }
        
        // No encontró lugar en ningún hijo
        int altura_arbol = calcular_altura_arbol();
        int max_nivel_anomalia = static_cast<int>(std::log2(altura_arbol)) + 1;
        return std::min(nivel, max_nivel_anomalia);
    }
    
    int calcular_altura_recursiva(node_ptr nodo) {
        if (!nodo || nodo->is_leaf()) {
            return 1;
        }
        
        int max_altura = 0;
        for (int i = 0; i < nodo->size_entry(); ++i) {
            auto hijo = nodo->cover_tree(i);
            if (hijo) {
                max_altura = std::max(max_altura, calcular_altura_recursiva(hijo));
            }
        }
        return max_altura + 1;
    }
};

// Función para generar datos como en test_m_tree.cpp
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
        std::string oid = "ID_" + std::to_string(i);
        
        std::vector<double> features;
        for (int d = 0; d < dimension; ++d) {
            features.push_back(dis(gen));
        }
        
        data.push_back({oid, features});
    }
    
    return data;
}

// Construir árbol como en test_m_tree.cpp
std::shared_ptr<tree_t> buildTree(const std::vector<std::pair<std::string, std::vector<double>>>& data) {
    auto tree = make_tree();
    for (const auto& entry_data : data) {
        auto entry = make_entry(entry_data.second, entry_data.first);
        tree->insert(entry);
    }
    return tree;
}

// Función para exportar métricas a CSV
void exportar_metricas_csv(const std::vector<Metricas>& metricas, 
                          int tamaño_arbol, 
                          const std::string& filename) {
    std::ofstream file(filename, std::ios::app); // Agregar al archivo
    
    // Escribir header solo si el archivo está vacío
    file.seekp(0, std::ios::end);
    if (file.tellp() == 0) {
        file << "tree_size,element_id,insertion_time_ms,anomaly_level,is_anomaly,distance_to_radius\n";
    }
    
    for (const auto& m : metricas) {
        file << tamaño_arbol << ","
             << m.id << ","
             << std::fixed << std::setprecision(6) << m.tiempo_ms << ","
             << m.nivel_anomalia << ","
             << (m.es_anomalia ? 1 : 0) << ","
             << std::fixed << std::setprecision(6) << m.distancia << "\n";
    }
    file.close();
}

// Función para probar un tamaño de árbol específico
void probar_tamaño_arbol(int tamaño_base, const std::string& csv_file) {
    std::cout << "\n=== PROBANDO ÁRBOL DE " << tamaño_base << " ELEMENTOS ===" << std::endl;
    
    // Generar población base
    std::cout << "  Generando " << tamaño_base << " elementos base..." << std::endl;
    auto datos_base = generateMockData(tamaño_base, 2, 0.0, 100.0);
    
    // Construir árbol
    std::cout << "  Construyendo M-Tree..." << std::endl;
    auto inicio_construccion = std::chrono::high_resolution_clock::now();
    auto arbol = buildTree(datos_base);
    auto fin_construccion = std::chrono::high_resolution_clock::now();
    
    auto tiempo_construccion = std::chrono::duration_cast<std::chrono::milliseconds>(
        fin_construccion - inicio_construccion);
    
    // Crear detector
    DetectorAnomalias detector(arbol);
    
    std::cout << "  Tiempo de construcción: " << tiempo_construccion.count() << " ms" << std::endl;
    std::cout << "  Altura del árbol: " << detector.calcular_altura_arbol() << std::endl;
    
    // Generar 50 elementos de prueba (25 normales + 25 anómalos)
    std::cout << "  Insertando 50 elementos de prueba..." << std::endl;
    
    auto nuevos_datos = generateMockData(10, 2, 0.0, 200.0); // 25 normales
    
    int anomalias_detectadas = 0;
    
    // Probar elementos normales
    for (const auto& data : nuevos_datos) {
        auto entry = make_entry(data.second, data.first + "_N");
        auto metricas = detector.insertar_y_detectar(entry);
        if (metricas.es_anomalia) anomalias_detectadas++;
    }
    
    
    // Exportar métricas
    exportar_metricas_csv(detector.obtener_metricas(), tamaño_base, csv_file);
    std::cout << "  Altura final: " << detector.calcular_altura_arbol() << std::endl;
}

// ===== MAIN =====

int main() {
    std::cout << "=== ANÁLISIS MULTI-TAMAÑO DE DETECCIÓN DE ANOMALÍAS ===" << std::endl;
    
    const std::string csv_file = "resultados_multitamaño.csv";
    
    // Limpiar archivo CSV anterior
    std::ofstream clear_file(csv_file, std::ios::trunc);
    clear_file.close();
    
    // Tamaños de árbol a probar
    //std::vector<int> tamaños = {1000, 5000, 10000, 50000, 100000, 500000, 1000000};
    std::vector<int> tamaños = {1000, 5000, 10000, 50000};

    auto inicio_total = std::chrono::high_resolution_clock::now();
    
    for (int tamaño : tamaños) {
        probar_tamaño_arbol(tamaño, csv_file);
    }
    
    auto fin_total = std::chrono::high_resolution_clock::now();
    auto tiempo_total = std::chrono::duration_cast<std::chrono::seconds>(fin_total - inicio_total);
    
    std::cout << "\n=== RESUMEN FINAL ===" << std::endl;
    std::cout << "Tiempo total de pruebas: " << tiempo_total.count() << " segundos" << std::endl;
    std::cout << "Datos exportados a: " << csv_file << std::endl;
    std::cout << "\nPara generar gráficas ejecuta:" << std::endl;
    std::cout << "  python3 graficar_resultados.py" << std::endl;
    
    return 0;
}