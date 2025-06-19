# M-Tree: Presentación Académica 🌳

Una presentación completa sobre **M-Tree** (Árbol Métrico), estructura de datos fundamental para búsquedas eficientes en espacios métricos.

## 👥 Autores

- **Ricardo Andre Ulloa Vega**
- **Jose Joaquin Leon Tello**

## 📋 Descripción

Esta presentación cubre de manera integral los M-Trees, desde los conceptos fundamentales de espacios métricos hasta implementaciones prácticas y aplicaciones del mundo real

## 🎯 Contenido de la Presentación

### 📚 **Sección 1: Introducción**
- **Espacios Métricos**: Fundamentos matemáticos y propiedades
- **Métrica de Hausdorff**: Ejemplo práctico con fórmulas
- **Intuición práctica**: Aplicaciones en la vida real
- **Definición de M-Tree**: Concepto y propósito

### 🏗️ **Sección 2: Estructura del Árbol**
- **Componentes del M-Tree**:
  - Entradas Hoja: `entry(Oj) = [Oj, oid(Oj), d(Oj, P(Oj))]`
  - Entradas Internas: `entry(Or) = [Or, ptr(T(Or)), r(Or), d(Or, P(Or))]`



### 🚀 Características Técnicas
### **Propiedades Métricas Implementadas**
- ✅ **Simetría**: `d(x, y) = d(y, x)`
- ✅ **No negatividad**: `d(x, y) ≥ 0`
- ✅ **Desigualdad triangular**: `d(x, z) ≤ d(x, y) + d(y, z)`

### **Algoritmos con Complejidad Optimizada**
- **Inserción**: `O(log n)` promedio
- **Búsqueda**: `O(log n)` con poda por desigualdad triangular
- **k-NN**: Eficiente con cola de 🛠️ Compilación

## 📞 Contacto

Para preguntas, sugerencias o colaboraciones:

- **Ricardo Andre Ulloa Vega** 
- **Jose Leon Tello** 

