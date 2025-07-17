from typing import Any, List, Optional, Tuple


class DWayHeap(object):
    def __init__(self, elements: List[Any] = [], priorities: List[float] = [], branching_factor: int = 2) -> None:
        """Constructor

        Args:
            elements: Los elementos para inicializar el heap.
            priorities: Las prioridades de los elementos anteriores. Deben tener la misma longitud que `elements`.
            branching_factor: El número (máximo) de hijos por nodo en el heap. Debe ser al menos 2.
        """
        if len(elements) != len(priorities):
            raise ValueError(f'La longitud de la lista de elementos ({len(elements)})'
                             f' debe coincidir con la longitud de la lista de prioridades ({len(priorities)}).')
        if branching_factor < 2:
            raise ValueError(f'El factor de ramificación ({branching_factor}) debe ser mayor que 1.')
        self._pairs: List[Tuple[float, Any]] = []
        self.D = branching_factor

        if len(elements) > 0:
            self._heapify(elements, priorities)

    def __sizeof__(self) -> int:
        """Tamaño del heap.

        Returns: El número de elementos en el heap.
        """
        return len(self)

    def __len__(self) -> int:
        """Tamaño del heap.

        Returns: El número de elementos en el heap.
        """
        return len(self._pairs)

    def _validate(self) -> bool:
        """Verifica que se cumplan las tres invariantes del heap:
        1. Cada nodo tiene como máximo `D` hijos. (Garantizado por la construcción)
        2. El árbol del heap es completo y alineado a la izquierda. (También garantizado por la construcción)
        3. Cada nodo contiene la mayor prioridad en el subárbol con raíz en ese nodo.

        Returns: True si se cumplen todas las invariantes del heap.
        """
        current_index = 0
        first_leaf = self.first_leaf_index()
        while current_index < first_leaf:
            current_priority: float = self._pairs[current_index][0]
            first_child = self._first_child_index(current_index)
            last_child_guard = min(first_child + self.D, len(self))
            for child_index in range(first_child, last_child_guard):
                if current_priority < self._pairs[child_index][0]:
                    return False
            current_index += 1
        return True

    def _push_down(self, index: int) -> None:
        """Empuja hacia abajo la raíz de un sub-heap hacia sus hojas para restablecer las invariantes del heap.
        Si alguno de los hijos del elemento tiene mayor prioridad, se intercambia el elemento actual
        con su hijo de mayor prioridad C, y se verifica recursivamente el sub-heap que estaba previamente
        enraizado en ese C.

        Args:
            index: El índice de la raíz del sub-heap.
        """
        assert (0 <= index < len(self._pairs))
        input_pair = self._pairs[index]
        input_priority = input_pair[0]
        current_index = index
        first_leaf = self.first_leaf_index()
        while current_index < first_leaf:
            child_index = self._highest_priority_child_index(current_index)
            assert (child_index is not None)
            if self._pairs[child_index][0] > input_priority:
                self._pairs[current_index] = self._pairs[child_index]
                current_index = child_index
            else:
                break

        self._pairs[current_index] = input_pair

    def _bubble_up(self, index: int) -> None:
        """Eleva un elemento hacia la raíz para restablecer las invariantes del heap.
        Si el padre P de un elemento tiene menor prioridad, se intercambian el elemento actual y su padre,
        y se verifica recursivamente la posición que tenía P anteriormente.

        Args:
            index: El índice del elemento a elevar.
        """
        assert (0 <= index < len(self._pairs))
        input_pair = self._pairs[index]
        input_priority = input_pair[0]
        while index > 0:
            parent_index = self._parent_index(index)
            parent = self._pairs[parent_index]

            if input_priority > parent[0]:
                self._pairs[index] = parent
                index = parent_index
            else:
                break

        self._pairs[index] = input_pair

    def _first_child_index(self, index) -> int:
        """Calcula el índice del primer hijo de un nodo en el heap.

        Args:
            index: El índice del nodo actual, del cual se buscan los índices de sus hijos.

        Returns: El índice del hijo más a la izquierda del nodo actual del heap.
        """
        return index * self.D + 1

    def _parent_index(self, index) -> int:
        """Calcula el índice del padre de un nodo en el heap.

        Args:
            index: El índice del nodo actual, del cual se busca el índice de su padre.

        Returns: El índice del padre del nodo actual del heap.
        """
        return (index - 1) // self.D

    def _highest_priority_child_index(self, index) -> Optional[int]:
        """Encuentra, entre los hijos de un nodo del heap, el hijo con la mayor prioridad.
        En caso de que varios hijos tengan la misma prioridad, se devuelve el más a la izquierda.

        Args:
            index: El índice del nodo del heap cuyos hijos se examinan.

        Returns: El índice del hijo con mayor prioridad del nodo actual del heap, o None si
                 el nodo actual no tiene hijos.
        """
        first_index = self._first_child_index(index)
        size = len(self)
        last_index = min(first_index + self.D, size)

        if first_index >= size:
            return None

        highest_priority = -float('inf')
        index = first_index
        for i in range(first_index, last_index):
            if self._pairs[i][0] > highest_priority:
                highest_priority = self._pairs[i][0]
                index = i

        return index

    def first_leaf_index(self):
        return (len(self) - 2) // self.D + 1

    def _heapify(self, elements: List[Any], priorities: List[float]) -> None:
        """Inicializa el heap con una lista de elementos y prioridades.

        Args:
            elements: La lista de elementos a agregar al heap.
            priorities: Las prioridades correspondientes a esos elementos (en el mismo orden).
        """
        assert (len(elements) == len(priorities))
        self._pairs = list(zip(priorities, elements))
        last_inner_node_index = self.first_leaf_index() - 1
        for index in range(last_inner_node_index, -1, -1):
            self._push_down(index)

    def is_empty(self) -> bool:
        """Verifica si el heap está vacío.

        Returns: True si el heap está vacío.
        """
        return len(self) == 0

    def top(self) -> Any:
        """Elimina y devuelve el elemento con mayor prioridad en el heap.
        Si el heap está vacío, lanza una `RuntimeError`.

        Returns: El elemento con mayor prioridad en el heap.
        """
        if self.is_empty():
            raise RuntimeError('Se llamó al método top en un heap vacío.')
        if len(self) == 1:
            element = self._pairs.pop()[1]
        else:
            element = self._pairs[0][1]
            self._pairs[0] = self._pairs.pop()
            self._push_down(0)

        return element

    def peek(self) -> Any:
        """Devuelve, SIN eliminarlo, el elemento con mayor prioridad en el heap.
        Si el heap está vacío, lanza una `RuntimeError`.

        Returns: El elemento con mayor prioridad en el heap.
        """
        if self.is_empty():
            raise RuntimeError('Se llamó al método peek en un heap vacío.')
        return self._pairs[0][1]
    
    def peek_priority(self) -> float:
      if self.is_empty():
        raise RuntimeError('Se llamó al método peek en un heap vacío.')
      return self._pairs[0][0]
         


    def insert(self, element: Any, priority: float) -> None:
        """Agrega un nuevo par elemento/prioridad al heap

        Args:
            element: El nuevo elemento a agregar.
            priority: La prioridad asociada al nuevo elemento.
        """
        self._pairs.append((priority, element))
        self._bubble_up(len(self._pairs) - 1)
