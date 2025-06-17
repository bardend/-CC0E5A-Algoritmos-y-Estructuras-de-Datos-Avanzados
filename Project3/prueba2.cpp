#include <bits/stdc++.h>
using namespace std;

class Animal;
class Perro;

using animal_type = unique_ptr<Animal>;
using perro_type = unique_ptr<Perro> ;

class Animal {
public:
    string nombre;
    int edad;
    Animal(string n = "", int e = 0) : nombre(n), edad(e) {}
    virtual ~Animal() = default;
};

class Perro {
public:
    animal_type mi_animal;
    string raza;
    Perro(animal_type&& a) : mi_animal(std::move(a)), raza("Perro_Fino") {}
    void razaaa() { cout << raza << endl; }
};

int main(){
    animal_type animal = make_unique<Animal>("Bobby", 2);
    perro_type perro = make_unique<Perro>(std::move(animal));
    // en este caso yo necesito un vector para guardar tanto perros_ptr como gatos_ptr
    // vector<animal_type>v;
    // v.push_back(std::move(perro));
    return 0;
}
