//m_tree.cpp
#include <iostream>
#include <random>
#include <fstream>
#include <sstream>
#include <iomanip>  // Necesario para std::fixed y std::setprecision

#include "m_tree.hpp"

//capacidad de los nodos 10.
using tree_t = m_tree<MTreeParams<double, std::string, 10>>;

// Función make_tree trasladada desde utils.hpp
template <typename... args_t>
std::shared_ptr<tree_t> make_tree(args_t&&... args) {
    return std::make_shared<tree_t>(std::forward<args_t>(args)...);
}

int main() {
    //
    // entry(const std::vector<feature_type>& f, const identifier_type& o) 
    //     : features_(f), oid(o), dis(0), pos(0) {}
    //
    // entry(const std::vector<feature_type>& f, const identifier_type& o) 
    //     : features_(f), oid(o), dis(0), pos(0) {}


    std::vector<double>v = {1.2, 2.0};
    std::string st = "hola";
    
    entry_type e12 = make_entry(v, st);

    /*

    auto tree = make_tree();
    std::ifstream infile("in.txt");
    std::vector<std::pair<std::string, std::vector<double>>> data;

    // Verificar si el archivo existe y no está vacío
    if (infile.is_open() && infile.peek() != std::ifstream::traits_type::eof()) {
        // Leer datos del archivo
        std::cout << "Leyendo datos desde in.txt..." << std::endl;
        
        int count;
        infile >> count;
        
        for (int i = 0; i < count; i++) {
            std::string word;
            double feature1, feature2;
            infile >> word >> feature1 >> feature2;
            
            std::vector<double> features = {feature1, feature2};
            data.push_back({word, features});
        }
        infile.close();
    } else {
        // Generar datos aleatorios y guardarlos
        std::cout << "Archivo in.txt vacío o no existe. Generando datos aleatorios..." << std::endl;
        
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_real_distribution<> dis(0.0, 20.0);
        
        auto random_letter = [&]() {
            std::uniform_int_distribution<> letter_dis('A', 'Z');
            return static_cast<char>(letter_dis(gen));
        };
        
        // Generar 10 entradas aleatorias
        int num_entries = 20;
        for (int i = 0; i < num_entries; i++) {
            std::string word = "";
            for (int j = 0; j < 2; j++) {
                word += random_letter();
            }
            std::vector<double> features = {dis(gen), dis(gen)};
            data.push_back({word, features});
        }
        
        // Guardar en archivo
        std::ofstream outfile("in.txt");
        outfile << num_entries << std::endl;
        outfile << std::fixed << std::setprecision(1);  // Formato: 1 decimal fijo
        for (const auto& entry : data) {
            outfile << entry.first << " ";
            outfile << entry.second[0] << " " << entry.second[1] << std::endl;
        }
        outfile.close();
        std::cout << "Datos guardados en in.txt" << std::endl;
    }

    // Procesar los datos (ya sean leídos o generados) y testear el árbol
    std::cout << "\n=== TESTEANDO EL ÁRBOL ===" << std::endl;
    for (const auto& entry : data) {
        std::cout << "\nVamos a añadir: " << entry.first 
                  << " con features: [" << entry.second[0] 
                  << ", " << entry.second[1] << "]" << std::endl;
        
        // Crear entry e insertar en el árbol
        entry_type e12 = make_entry(entry.second, entry.first);
        tree->insert(e12);
        tree->print_tree();
    }
    */

// std::cout << "\n=== ÁRBOL FINAL ===" << std::endl;
// tree->print_tree();
}
