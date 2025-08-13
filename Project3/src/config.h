#ifndef CONFIG_H
#define CONFIG_H

// Configuración centralizada del M-Tree
namespace mtree_config {
    // Capacidad de los nodos del árbol
    static constexpr int CAPACITY = 15;
    // Tipos de datos
    using feature_type = double;
    using identifier_type = std::string;
}

// Definición centralizada de los parámetros del M-Tree
using MTreeConfig = MTreeParams<mtree_config::feature_type, mtree_config::identifier_type, mtree_config::CAPACITY>;

#endif // CONFIG_H 
