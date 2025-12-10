# Sistema de Detección de Anomalías en M-Tree

Este sistema implementa un detector de anomalías basado en M-Tree que puede identificar elementos anómalos según su nivel jerárquico en el árbol.

## Características Principales

### Detección Jerárquica de Anomalías
- **Nivel 0**: Elementos normales que encajan en el radio de cobertura
- **Nivel 1**: Anomalías en el nodo raíz (no entran en el radio del root)
- **Nivel 2+**: Anomalías en niveles más profundos, hasta log(altura) máximo

### Métricas Registradas por Inserción
- ⏱️ **Tiempo de inserción** (milisegundos)
- 📊 **Nivel de anomalía** (0 = normal, 1+ = anómalo)
- 🔍 **Número de nodos explorados**
- 📏 **Distancia al radio** del nodo más cercano
- 🌳 **Altura actual del árbol**

## Estructura de Archivos

```
metrics/
├── anomaly_detector.h/.cpp     # Detector principal de anomalías
├── population_generator.h/.cpp # Generador de poblaciones de prueba
├── metrics_analyzer.h/.cpp     # Analizador y generador de reportes
├── main_anomaly_test.cpp       # Prueba completa (10,000 elementos)
├── small_anomaly_test.cpp      # Prueba rápida (1,000 elementos)
├── CMakeLists.txt             # Configuración de compilación
└── README.md                  # Este archivo
```

## Compilación

```bash
# Desde la carpeta metrics/
mkdir build && cd build
cmake ..
make

# Esto genera los ejecutables:
# - anomaly_test      (prueba completa)
# - small_test        (prueba rápida)
```

## Uso Básico

### Ejecutar Pruebas
```bash
# Prueba rápida (1,000 elementos)
./small_test

# Prueba completa (10,000 elementos)
./anomaly_test
```

### Generar Gráficas
```bash
# Después de ejecutar las pruebas
cd metrics_output/
python3 plot_anomaly_results.py    # o plot_small_test.py
```

## Archivos de Salida

El sistema genera automáticamente:

### Archivos CSV
- `*_histogram.csv` - Distribución de niveles de anomalía
- `*_time_analysis.csv` - Tiempo vs altura del árbol  
- `*_distance_analysis.csv` - Distancia vs nivel de anomalía
- `*_nodes_analysis.csv` - Nodos explorados por inserción

### Reportes
- `*_summary.txt` - Resumen estadístico completo
- `plot_*.py` - Script de Python para generar gráficas

### Gráficas (generadas por Python)
- Histograma de niveles de anomalía
- Tiempo de inserción vs altura del árbol
- Boxplot de distancia al radio por nivel
- Nodos explorados vs altura del árbol

## API Programática

### Uso del Detector de Anomalías

```cpp
#include "anomaly_detector.h"

// Crear M-Tree
auto tree = std::make_shared<m_tree<MTreeConfig>>();

// Crear detector
AnomalyDetector<MTreeConfig> detector(tree);

// Insertar con detección de anomalías
auto entry = make_entry(features, id);
auto metrics = detector.insert_with_anomaly_detection(entry);

// Verificar si es anomalía
if (metrics.is_anomaly) {
    std::cout << "Anomalía nivel " << metrics.anomaly_level 
              << " detectada!" << std::endl;
}
```

### Generación de Población

```cpp
#include "population_generator.h"

// Crear generador (2D, rango [0,100])
PopulationGenerator<MTreeConfig> generator(2, 0.0, 100.0);

// Generar población mixta
auto population = generator.generate_mixed_population(
    8000,  // elementos normales
    2000,  // elementos anómalos
    5,     // número de clusters normales
    8.0,   // desviación estándar de clusters
    25.0   // distancia mínima para anomalías
);
```

### Análisis de Métricas

```cpp
#include "metrics_analyzer.h"

// Crear analizador
MetricsAnalyzer<MTreeConfig> analyzer;

// Cargar métricas desde el detector
analyzer.load_metrics(detector);

// Generar reportes completos
analyzer.generate_complete_report("mi_experimento");

// Generar script de gráficas
analyzer.generate_python_plotting_script("plot_mi_experimento.py");
```

## Metodología de Pruebas

1. **Generación de Población Base**: Se crean 8,000+ elementos normales distribuidos en clusters gaussianos
2. **Construcción del M-Tree**: Se construye el árbol con la población base
3. **Inserción de Elementos de Prueba**: Se insertan 2,000+ elementos (mezcla de normales y anómalos)
4. **Registro de Métricas**: Cada inserción registra tiempo, nivel de anomalía, nodos explorados, etc.
5. **Análisis de Resultados**: Se generan estadísticas y gráficas automáticamente

## Interpretación de Resultados

### Niveles de Anomalía
- **0**: Normal - El elemento encaja naturalmente en el árbol
- **1**: Anomalía leve - No encaja en el nodo raíz pero podría en hijos
- **2+**: Anomalía severa - Requiere descender varios niveles o no encaja

### Métricas de Eficiencia
- **Tiempo de inserción**: Indica el costo computacional
- **Nodos explorados**: Mide la eficiencia del algoritmo de búsqueda
- **Distancia al radio**: Cuantifica qué tan "anómalo" es un elemento

## Configuración

El sistema usa la configuración definida en `../src/config.h`:
- **Capacidad de nodo**: 15 entradas por nodo
- **Tipo de características**: `double`
- **Tipo de identificador**: `std::string`
- **Métrica de distancia**: Euclidiana

## Dependencias

### Para compilar:
- C++17 o superior
- CMake 3.10+

### Para gráficas (opcional):
- Python 3.x
- pandas
- matplotlib  
- seaborn
- numpy

```bash
pip install pandas matplotlib seaborn numpy
```

## Ejemplo de Salida

```
=== ESTADÍSTICAS DE DETECCIÓN DE ANOMALÍAS ===
Total de inserciones: 2000

Distribución de niveles de anomalía:
  Nivel 0: 1650 (82.5%)
  Nivel 1: 280 (14.0%)  
  Nivel 2: 70 (3.5%)

Estadísticas generales:
  Tiempo promedio de inserción: 0.125 ms
  Nodos explorados promedio: 3.2
  Distancia promedio al radio: 12.45
  Altura actual del árbol: 4
```