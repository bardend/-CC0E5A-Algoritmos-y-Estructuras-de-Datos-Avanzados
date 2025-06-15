#include <iostream>
#include <memory> // Para std::unique_ptr

// Parámetros configurables del árbol (ahora con valores por defecto)
template <typename KeyType = int, typename CountType = size_t>
struct BinaryTreeParams {
    using key_type = KeyType;
    using node_count_type = CountType;
};

// Clase plantilla para un nodo de árbol binario
template <typename Params>
class binary_tree_node {
public:
    // Usamos 'using' para simplificar los tipos
    using key_type = typename Params::key_type;
    using node_count_type = typename Params::node_count_type;
    using node_ptr = std::unique_ptr<binary_tree_node>; // Smart pointer para manejo automático de memoria

    // Estructura de metadatos del nodo
    struct base_fields {
        bool is_leaf;
        node_count_type key_count;
        binary_tree_node* parent;  // Puntero crudo para padre (no es dueño de la memoria)
        node_ptr left;            // Hijo izquierdo (manejado con unique_ptr)
        node_ptr right;           // Hijo derecho (manejado con unique_ptr)
    };

    // Constructor explícito (evita conversiones implícitas no deseadas)
    explicit binary_tree_node(const key_type& key_val)
        : fields{true, 1, nullptr, nullptr, nullptr}, key(key_val) {}

    // Métodos básicos
    bool is_leaf() const noexcept { return fields.is_leaf; } // 'noexcept' para optimización
    const key_type& get_key() const noexcept { return key; }  // Getter seguro

    // Eliminamos copias no deseadas
    binary_tree_node(const binary_tree_node&) = delete;
    binary_tree_node& operator=(const binary_tree_node&) = delete;

    // Campos públicos (en producción podrían ser privados con getters)
    base_fields fields;
    key_type key;
};

int main() {
    // Uso con parámetros personalizados
    using MyParams = BinaryTreeParams<int, uint32_t>; // Clave: int, Contador: uint32_t
    binary_tree_node<MyParams> root_node(42);

    // Insertar hijos (usando smart pointers)
    root_node.fields.left = std::make_unique<binary_tree_node<MyParams>>(30);
    root_node.fields.left->fields.parent = &root_node; // Establecer padre

    root_node.fields.right = std::make_unique<binary_tree_node<MyParams>>(50);
    root_node.fields.right->fields.parent = &root_node;

    // Verificar
    std::cout << "Nodo raíz: " << root_node.get_key() << std::endl;
    std::cout << "Hijo izquierdo: " << root_node.fields.left->get_key() << std::endl;
    std::cout << "¿Es hoja la raíz? " << std::boolalpha << root_node.is_leaf() << std::endl; // Output: false

    return 0;
}
