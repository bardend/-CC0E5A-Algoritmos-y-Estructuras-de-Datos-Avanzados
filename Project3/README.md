------

# **M-Tree distribuido (cliente-servidor ligero) con sincronización vía HTTP**

Una presentación completa sobre **M-Tree** (Árbol Métrico), estructura de datos fundamental para búsquedas eficientes en espacios métricos.

## 📋 Descripción

Esta presentación cubre de manera integral los M-Trees, desde los conceptos fundamentales de espacios métricos hasta implementaciones prácticas y aplicaciones del mundo real.

## 🎯 Contenido de la Presentación

### 📚 **Sección 1: Introducción**

Desde el punto de vista del rendimiento, si tenemos datos con un número alto de dimensiones, eso genera un costo en **CPU** respecto al disco (**I/O**). El `M-Tree` trabaja por indexación, generando datos preprocesados y realizando una poda a través de las propiedades de espacios métricos, como la **desigualdad triangular**, logrando una complejidad logarítmica.

### 🚀 Características Técnicas

### **Propiedades Métricas Implementadas**

- ✅ **Simetría**: `d(x, y) = d(y, x)`
- ✅ **No negatividad**: `d(x, y) ≥ 0`
- ✅ **Desigualdad triangular**: `d(x, z) ≤ d(x, y) + d(y, z)`

### 🏗️ **Sección 2: Estructura del Árbol**

- **Componentes del M-Tree**:
  - Entradas hoja: `entry(Oj) = [Oj, oid(Oj), d(Oj, P(Oj))]`
  - Entradas internas: `entry(Or) = [Or, ptr(T(Or)), r(Or), d(Or, P(Or))]`

Cabe resaltar que, en el *paper*, las **entradas internas deben actualizarse cada vez que se realiza una operación de split cuando un nodo está saturado (`overflow`).** En este proceso, se promueven dos nodos mediante diversas heurísticas que buscan **evitar el solapamiento** (*overlap*) entre las entradas internas.

La métrica utilizada consiste en **hallar los puntos más lejanos** entre todos los candidatos, con el fin de maximizar la separación.

Una vez realizada dicha elección, para asegurarnos de que el **radio del nodo padre** cubre completamente su subárbol, se debe aplicar la siguiente expresión:


$$
r(Op_1) = \max\{ d(O_j, Op_1) \mid O_j \in N_1 \}
$$


Por cada *split*, este radio puede actualizarse recursivamente hacia los niveles superiores, garantizando que **todos los nodos internos incluyan completamente a su subárbol** dentro del *cover tree*:


$$
r(Op_1) = \max\{ d(O_j, Op_1) + r(O_j) \mid O_j \in N_1 \}
$$


Una gráfica para tener una noción :

![](/home/bardend/Documents/Cursos/8/AlgortimosAvanzado/Algorithms/Project3/img/m_tree.png)

### Geometría de la Decisión

**Caso 1: Nodos Internos (primera rama)**

```
if dmin(T(Or)) ≤ dk
```

Pregunta: "¿La región más cercana de esta caja puede contener mejores candidatos?"

- Si `dmin > dk`: **Poda total** - toda la región es irrelevante
- Si `dmax < dk`: **Inclusión total** - todos los objetos en la región son mejores que los actuales



### 🏗️ **Sección 3: Poda en el Árbol**

Porciones de código de `k-NN NodeSearch(N: M-tree node, Q: query object, k: integer)`:

```python
{
let Op be the parent object of node N;
if N is not a leaf
then {
    for each entry(Or) in N do:
        if |d(Op, Q) − d(Or, Op)| ≤ dk + r(Or) ........1
        if d(Oj, Q) ≤ dk ...............2
```

Antes de empezar a buscar en cada nodo, verificamos esta condicional (1). Esto se debe a las propiedades de los espacios métricos. Además, la distancia preprocesada durante la construcción genera una forma eficiente de búsqueda, evitando instanciar el objeto y trabajando únicamente con **indexación**.



### **Algoritmos con Complejidad Optimizada**
- **Inserción**: `O(log n)` promedio.
- **Búsqueda**: `O(log n)` con poda por desigualdad triangular.
- **k-NN**: Eficiente con cola de 🛠️ Compilación y desigualdad triangular.

## 📞 Contacto

Para preguntas, sugerencias o colaboraciones:

- **Ricardo Andre Ulloa Vega** 
- **Jose Leon Tello** 

## 