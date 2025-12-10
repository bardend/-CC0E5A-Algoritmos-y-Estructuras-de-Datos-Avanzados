#!/bin/bash

# Script para ejecutar análisis con múltiples capacidades
# Modifica config.h, compila y ejecuta para cada capacidad

echo "=== ANÁLISIS MULTI-CAPACIDAD DE DETECCIÓN DE ANOMALÍAS ==="

# Archivo CSV de salida
CSV_FILE="resultados_multi_capacidad.csv"

# Limpiar archivo CSV anterior
echo "tree_size,capacity,element_id,insertion_time_ms,anomaly_level,is_anomaly,distance_to_radius" > $CSV_FILE

# Capacidades a probar
CAPACIDADES=(4 6 8 10 )

# Tamaños a probar  
TAMANOS=(1000 5000 10000 50000)

# Backup del config.h original
cp ../src/config.h ../src/config.h.backup

echo "Iniciando análisis multi-capacidad..."
echo "Capacidades: ${CAPACIDADES[@]}"
echo "Tamanos: ${TAMANOS[@]}"

for CAPACITY in "${CAPACIDADES[@]}"; do
    echo ""
    echo "======= PROBANDO CAPACIDAD $CAPACITY ======="
    
    # Modificar config.h
    sed -i "s/static constexpr int CAPACITY = [0-9]\+;/static constexpr int CAPACITY = $CAPACITY;/" ../src/config.h
    
    # Verificar el cambio
    echo "Config actual:"
    grep "CAPACITY" ../src/config.h
    
    # Recompilar
    echo "Recompilando con CAPACITY=$CAPACITY..."
    g++ -std=c++17 -I../src -O2 -o detector_capacity_$CAPACITY detector_simple.cpp ../src/entry.cpp ../src/m_tree.cpp
    
    if [ $? -eq 0 ]; then
        echo "Compilación exitosa para CAPACITY=$CAPACITY"
        
        # Crear versión temporal del detector que agregue la capacidad al CSV
        cat > temp_detector_$CAPACITY.cpp << EOF
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
        
        m.distancia = 0.0;
        
        historial.push_back(m);
        return m;
    }
    
    int detectar_nivel(entry_type elem) {
        if (!arbol || !arbol->get_root()) {
            return 1;
        }
        return detectar_recursivo(elem, arbol->get_root(), 1);
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
            for (int i = 0; i < nodo->size_entry(); ++i) {
                auto entrada_existente = nodo->get_entry(i);
                double dist = elem->distance_to(*entrada_existente);
                if (dist <= entrada_existente->radio_covertura + 5.0) {
                    return 0;
                }
            }
            int altura_arbol = calcular_altura_arbol();
            int max_nivel_anomalia = static_cast<int>(std::log2(altura_arbol)) + 1;
            return std::min(nivel, max_nivel_anomalia);
        }
        
        for (int i = 0; i < nodo->size_entry(); ++i) {
            auto hijo = nodo->cover_tree(i);
            if (hijo) {
                int resultado = detectar_recursivo(elem, hijo, nivel + 1);
                if (resultado == 0) {
                    return 0;
                }
            }
        }
        
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

std::vector<std::pair<std::string, std::vector<double>>> generateMockData(
    int num_entries, 
    int dimension, 
    double min_val = 0.0, 
    double max_val = 100.0
) {
    std::vector<std::pair<std::string, std::vector<double>>> data;
    
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

std::shared_ptr<tree_t> buildTree(const std::vector<std::pair<std::string, std::vector<double>>>& data) {
    auto tree = make_tree();
    for (const auto& entry_data : data) {
        auto entry = make_entry(entry_data.second, entry_data.first);
        tree->insert(entry);
    }
    return tree;
}

void exportar_metricas_csv(const std::vector<Metricas>& metricas, 
                          int tamaño_arbol, 
                          int capacity,
                          const std::string& filename) {
    std::ofstream file(filename, std::ios::app);
    
    for (const auto& m : metricas) {
        file << tamaño_arbol << ","
             << capacity << ","
             << m.id << ","
             << std::fixed << std::setprecision(6) << m.tiempo_ms << ","
             << m.nivel_anomalia << ","
             << (m.es_anomalia ? 1 : 0) << ","
             << std::fixed << std::setprecision(6) << m.distancia << "\\n";
    }
    file.close();
}

void probar_tamaño_arbol(int tamaño_base, int capacity, const std::string& csv_file) {
    std::cout << "\\n=== PROBANDO " << tamaño_base << " ELEMENTOS (CAPACITY=" << capacity << ") ===" << std::endl;
    
    auto datos_base = generateMockData(tamaño_base, 2, 0.0, 100.0);
    
    std::cout << "  Construyendo M-Tree..." << std::endl;
    auto inicio_construccion = std::chrono::high_resolution_clock::now();
    auto arbol = buildTree(datos_base);
    auto fin_construccion = std::chrono::high_resolution_clock::now();
    
    auto tiempo_construccion = std::chrono::duration_cast<std::chrono::milliseconds>(
        fin_construccion - inicio_construccion);
    
    DetectorAnomalias detector(arbol);
    
    std::cout << "  Tiempo construcción: " << tiempo_construccion.count() << " ms" << std::endl;
    std::cout << "  Altura: " << detector.calcular_altura_arbol() << std::endl;
    
    std::cout << "  Insertando elementos de prueba..." << std::endl;
    
    auto nuevos_datos = generateMockData(10, 2, 0.0, 200.0);
    
    int anomalias_detectadas = 0;
    
    for (const auto& data : nuevos_datos) {
        auto entry = make_entry(data.second, data.first + "_N");
        auto metricas = detector.insertar_y_detectar(entry);
        if (metricas.es_anomalia) anomalias_detectadas++;
    }
    
    exportar_metricas_csv(detector.obtener_metricas(), tamaño_base, capacity, csv_file);
    std::cout << "  Altura final: " << detector.calcular_altura_arbol() << std::endl;
    std::cout << "  Anomalías: " << anomalias_detectadas << "/10" << std::endl;
}

int main() {
    const std::string csv_file = "$CSV_FILE";
    const int capacity = $CAPACITY;
    
    std::vector<int> tamanos = {1000, 5000, 10000};
    
    for (int tamano : tamanos) {
        probar_tamaño_arbol(tamano, capacity, csv_file);
    }
    
    return 0;
}
EOF
        
        # Compilar y ejecutar la versión temporal
        g++ -std=c++17 -I../src -O2 -o temp_detector_$CAPACITY temp_detector_$CAPACITY.cpp ../src/entry.cpp ../src/m_tree.cpp
        
        if [ $? -eq 0 ]; then
            echo "Ejecutando análisis con CAPACITY=$CAPACITY..."
            ./temp_detector_$CAPACITY
        else
            echo "Error compilando detector temporal para CAPACITY=$CAPACITY"
        fi
        
        # Limpiar archivos temporales
        rm -f temp_detector_$CAPACITY temp_detector_$CAPACITY.cpp
    else
        echo "Error de compilación para CAPACITY=$CAPACITY"
    fi
done

# Restaurar config.h original
cp ../src/config.h.backup ../src/config.h
rm ../src/config.h.backup

echo ""
echo "=== ANÁLISIS COMPLETADO ==="
echo "Datos exportados a: $CSV_FILE"
echo "Total de registros:"
wc -l $CSV_FILE
echo ""
echo "Para generar gráficas ejecuta:"
echo "  python3 graficar_multi_capacidad.py"