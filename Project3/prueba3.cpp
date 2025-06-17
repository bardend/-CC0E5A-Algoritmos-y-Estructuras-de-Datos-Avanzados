#include <bits/stdc++.h>
using namespace std;

class Animal;
class Mascota;
class Perro;

using animal_type = unique_ptr<Animal>;
using mascota_type = unique_ptr<Mascota>;
using perro_type = unique_ptr<Perro>;

class Animal {
public:
    string nombre;
    int edad;
    Animal(string n = "", int e = 0) : nombre(n), edad(e) {}
    virtual ~Animal() = default;
};

// Base class for pets
class Mascota {
public:
    animal_type mi_animal;
    string raza;
    
    Mascota(animal_type&& a, string r) : mi_animal(std::move(a)), raza(r) {}
    virtual ~Mascota() = default;
    virtual void razaaa() = 0; // Pure virtual function
};

class Perro : public Mascota {
public:
    Perro(animal_type&& a) : Mascota(std::move(a), "Perro_Fino") {}
    void razaaa() override { 
        cout << "Perro: " << raza << endl; 
    }
};

int main(){
    // Create animals
    animal_type animal1 = make_unique<Animal>("Bobby", 2);
    perro_type perro = make_unique<Perro>(std::move(animal1));
    vector<mascota_type> mascotas;

    mascotas.push_back(std::move(perro));

    for(int i = 0; i < (int)mascotas.size(); i++)
        cout << mascotas[i]->mi_animal->nombre << endl;
       
    return 0;
}
