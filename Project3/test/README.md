# M-Tree Test

Test de rendimiento para implementación de M-Tree en C++.

## Compilar

```bash
g++ -o mtree_test main.cpp ../src/entry.cpp ../src/m_tree.cpp
```

## Ejecutar

```bash
./mtree_test [opciones]
```

### Opciones

- `-d <num>` - Dimensión (default: 10)
- `-nt <num>` - Número de tests (default: 1) 
- `-nq <num>` - Queries por test (default: 5)
- `-np <num>` - Número de puntos (default: 100000)

### Ejemplos

```bash
# Default
./mtree_test

# Test personalizado
./mtree_test -d 32 -nt 3 -nq 10 -np 50000

# Test rápido
./mtree_test -d 5 -nt 1 -nq 2 -np 1000
```
