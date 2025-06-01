import threading
from typing import Optional, Generic, TypeVar
from math import log2
from collections import deque

K = TypeVar('K')
V = TypeVar('V')

class ReadWriteLock:
    def __init__(self):
        self._read_ready = threading.Condition(threading.RLock())
        self._readers = 0

    def acquire_read(self):
        self._read_ready.acquire()
        self._readers += 1
        self._read_ready.release()

    def release_read(self):
        self._read_ready.acquire()
        self._readers -= 1
        if self._readers == 0:
            self._read_ready.notify_all()
        self._read_ready.release()

    def acquire_write(self):
        self._read_ready.acquire()
        while self._readers > 0:
            self._read_ready.wait()

    def release_write(self):
        self._read_ready.release()


class SplayTree(Generic[K, V]):
    """
    Implementación de SplayTree basada en código de Treaps de la clase
    Uso exclusivo para probar la desamortización
    """
    def __init__(self):
        self.root: Optional['SplayTree.Node'] = None
        self.lock = ReadWriteLock()
        self.num_elems = 0

    ##############################################################

    class Entry(Generic[K, V]):
        """
        Representación de entrada de un nodo (par clave, valor)
        """
        def __init__(self, key: K, value: V):
            self.key = key
            self.value = value

        def __repr__(self):
            return f"Entry(key={self.key}, value={self.value})"

    class Node:
        """
        Nodo del SplayTree
        """
        def __init__(self, key: K, value: V):
            self.key = key
            self.value = value
            self.left: Optional['SplayTree.Node'] = None
            self.right: Optional['SplayTree.Node'] = None
            self.parent: Optional['SplayTree.Node'] = None

        def rotateRight(self) -> 'SplayTree.Node':
            """
            Rotación a la derecha
            """
            leftChild = self.left
            if leftChild is None:
                raise Exception("rotateRight llamado sin hijo izquierdo")
            self.left = leftChild.right
            if leftChild.right:
                leftChild.right.parent = self
            leftChild.right = self
            leftChild.parent = self.parent
            self.parent = leftChild
            if leftChild.parent is not None:
                if leftChild.parent.left == self:
                    leftChild.parent.left = leftChild
                else:
                    leftChild.parent.right = leftChild
            return leftChild

        def rotateLeft(self) -> 'SplayTree.Node':
            """
            Rotación a la izquierda
            """
            rightChild = self.right
            if rightChild is None:
                raise Exception("rotateLeft llamado sin hijo derecho")
            self.right = rightChild.left
            if rightChild.left:
                rightChild.left.parent = self
            rightChild.left = self
            rightChild.parent = self.parent
            self.parent = rightChild
            if rightChild.parent is not None:
                if rightChild.parent.left == self:
                    rightChild.parent.left = rightChild
                else:
                    rightChild.parent.right = rightChild
            return rightChild
        
        def __repr__(self):
            return f"Node(key={self.key}, value={self.value})"

    ########################################################

    def _splay(self, node: 'SplayTree.Node'):
        """
        Implementación de splay, genera rotaciones de un nodo hasta llevarlo a la raiz
        """
        count = 0
        while node.parent:
            count+=2
            parent = node.parent
            grandparent = parent.parent
            #self.print_tree()
            if grandparent is None:
                # Zig (1 rotacion derecha o izquierda)
                count-=1
                if parent.left == node:
                    node = parent.rotateRight()
                else:
                    node = parent.rotateLeft()
            #El nodo tiene nodo padre y abuelo a la izquierda (o ambos a la derecha)
            elif grandparent.left == parent and parent.left == node:
                # Zig-Zig
                parent.rotateRight()
                grandparent.rotateRight()
            elif grandparent.right == parent and parent.right == node:
                # Zig-Zig
                parent.rotateLeft()
                grandparent.rotateLeft()
            #El nodo tiene nodo padre y abuelo en sentidos opuestos
            elif grandparent.left == parent and parent.right == node:
                # Zig-Zag
                parent.rotateLeft()
                grandparent.rotateRight()
            elif grandparent.right == parent and parent.left == node:
                # Zig-Zag
                parent.rotateRight()
                grandparent.rotateLeft()
            
        self.root = node
        #print(count)
        
        


    def insert(self, entry: 'SplayTree.Entry', splay=True):
        """
        Inserción de BST estándar
        Al realizar la inserción, se hace splay del nodo insertado hasta
        llevarlo a la raiz
        """
        key = entry.key
        value = entry.value
        self.lock.acquire_write()
        try:
            if not self.root:
                self.root = self.Node(key, value)
                return

            current = self.root
            while True:
                if key < current.key:
                    if current.left:
                        current = current.left
                    else:
                        current.left = self.Node(key, value)
                        current.left.parent = current
                        self.num_elems+=1
                        if splay:
                            self._splay(current.left)
                        break
                elif key > current.key:
                    if current.right:
                        current = current.right
                    else:
                        current.right = self.Node(key, value)
                        current.right.parent = current
                        self.num_elems+=1
                        if splay:
                            self._splay(current.right)
                        break
                else:
                    # Duplicate keys are not allowed, but we could update value if needed
                    if splay:
                        self._splay(current)
                    break
        finally:
            self.lock.release_write()

    def search(self, key: K) -> Optional[V]:
        """
        Búsqueda de BST estándar
        Al encontrar el nodo, le hace splay para llevarlo a la raiz
        """
        self.lock.acquire_read()
        try:
            current = self.root
            while current:
                if key < current.key:
                    current = current.left
                elif key > current.key:
                    current = current.right                    
                else:
                    self._splay(current)
                    return current
            return None
        finally:
            self.lock.release_read()

    def size(self) -> int:
        """Cuenta nodos de forma iterativa (no recursiva)."""
        self.lock.acquire_read()
        try:
            if not self.root:
                return 0
            count = 0
            stack = [self.root]
            while stack:
                node = stack.pop()
                count += 1
                if node.left: stack.append(node.left)
                if node.right: stack.append(node.right)
            return count
        finally:
            self.lock.release_read()

    def height(self) -> int:
        """
        Calcula altura (número de niveles) con BFS iterativo.
        """
        self.lock.acquire_read()
        try:
            if not self.root:
                return 0
            
            queue = [self.root]
            height = 0
            while queue:
                level_size = len(queue)
                for _ in range(level_size):
                    node = queue.pop(0)
                    if node.left: queue.append(node.left)
                    if node.right: queue.append(node.right)
                height += 1
            return height
        finally:
            self.lock.release_read()

    def isEmpty(self) -> bool:
        self.lock.acquire_read()
        try:
            return self.root is None
        finally:
            self.lock.release_read()

    def clear(self):
        self.lock.acquire_write()
        try:
            self.root = None
        finally:
            self.lock.release_write()

    def print_tree(self, node=None, indent="", last=True, child_ref="L"):
      """
      Funcion auxiliar para imprimir el arbol
      """
      if node is None:
          # Solo al principio: si no se pasa un nodo, usamos la raíz
          if indent == "":
              node = self.root
          else:
              return  # Evita continuar si llegamos a una hoja

      if node is not None:
          print(indent + ("`- " if last else "|- ") + child_ref +f" - (K: {node.key})")
          indent += "   " if last else "|  "
          # Calculamos si hay hijos para saber cómo dibujar
          children = [child for child in [node.left, node.right] if child is not None]
          if children:
              self.print_tree(node.left, indent, node.right is None, "L")
              self.print_tree(node.right, indent, True, "R")

class DeamortizedSplayTree(SplayTree, Generic[K, V]):
    """
    Versión desamortizada de SplayTree
    """
    def __init__(self):
        super().__init__()
        self.splay_queue = []

    def _get_queue(self):
        return list(self.splay_queue)
    
    def _splay(self, node):
        """
        Sobrecarga de la función _splay() del SplayTree
        Se añaden los nodos a hacer splay a una cola con los nodos en espera
        En lugar de hacer rotaciones hasta llegar a la raiz, se hacen
        rotaciones hasta llegar a un máximo de 2*log(N) rotaciones
        
        Si el nodo no llegó a la raiz hasta entonces, se añade al final de la cola
        """
        #Añade nodo a la cola
        self.splay_queue.append(node)   #O(1)
        
        #Implementación para vaciar la cola de nodos en espera
        #self._partial_splay(self.splay_queue.popleft())
        #for _ in range(curr_len//2+1):
        #    self._partial_splay(self.splay_queue.popleft())
        num_splays = 0
        
        #PEOR CASO  =  2*log(N)
        while len(self.splay_queue)>0 and num_splays<max(1,2*log2(self.num_elems)):
            #Se obtiene el siguiente nodo de la cola
            node = self.splay_queue.pop(0)
            
                
            while node.parent and num_splays<max(1,2*log2(self.num_elems)):
                parent = node.parent
                grandparent = parent.parent
                #Cuenta el numero de rotaciones realizadas
                num_splays+=2
                if grandparent is None:
                    # Zig
                    num_splays-=1
                    if parent.left == node:
                        node = parent.rotateRight()
                    else:
                        node = parent.rotateLeft()
                elif grandparent.left == parent and parent.left == node:
                    # Zig-Zig
                    parent.rotateRight()
                    grandparent.rotateRight()
                    
                elif grandparent.right == parent and parent.right == node:
                    # Zig-Zig
                    parent.rotateLeft()
                    grandparent.rotateLeft()
                    
                elif grandparent.left == parent and parent.right == node:
                    # Zig-Zag
                    parent.rotateLeft()
                    grandparent.rotateRight()
                elif grandparent.right == parent and parent.left == node:
                    # Zig-Zag
                    parent.rotateRight()
                    grandparent.rotateLeft()
            
            if not node.parent:
                #Si el nodo ya es una raiz, se actualiza en el arbol
                self.root = node
            else:
                #Si no se logro llevar a la raiz, se añade el nodo a la cola
                self.splay_queue.append(node)
            #print(num_splays)
            

    
        
        

if __name__ == "__main__":
    # Prueba básica
    splay_tree = DeamortizedSplayTree[int, int]()
    for i in range(1, 50):
        splay_tree.insert(SplayTree.Entry(50, 30))
        if i%50==0:
            splay_tree.print_tree()
            print(splay_tree.splay_queue)
    
    print("Tamaño del tree:", splay_tree.size())
    print("Altura del tree:", splay_tree.height())
    print("Tamaño luego de top:", splay_tree.size())

