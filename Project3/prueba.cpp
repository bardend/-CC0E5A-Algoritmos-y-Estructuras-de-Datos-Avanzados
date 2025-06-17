#include <bits/stdc++.h>
using namespace std;

// Declaraciones forward para los type aliases
class Animal;
class Perro;

// Type aliases (mejor definirlos antes de las clases)
using animal_type = unique_ptr<Animal>;
using perro_type = unique_ptr<Perro>;

class Animal {
public:
    string nombre;
    int edad;
    Animal(string n = "", int e = 0) : nombre(n), edad(e) {}
};

class Perro : public Animal {
public:
    string raza;
    animal_type mi_animal; 

    Perro(animal_type& a) : mi_animal(std::move(a)), raza("Fino"){}
    void razaaa() { cout << raza << endl; }
};

int main(){
    animal_type animal_ptr2 = make_unique<Animal>("Bobby", 2);
    perro_type perro_ptr2 = make_unique<Perro>(animal_ptr2); 
    cout << "Nombre: " << perro_ptr2->mi_animal->nombre << endl;
    perro_ptr2->razaaa();
    return 0;
}
