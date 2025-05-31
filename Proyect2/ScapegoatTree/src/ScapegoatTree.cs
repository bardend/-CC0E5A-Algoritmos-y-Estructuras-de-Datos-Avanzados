using System;
//ScapegoatTree.cs

namespace Proyect2.src {

    /* 
     *Implementation of ScapegoatTree acording to the paper in Doc
     */
    public class ScapegoatTree<K, V> where K: IComparable<K> {
        public Node<K, V> root {get; set;} = null;
        private List<Node<K, V>> targetNodes = new List<Node<K, V>>();
        private float alpha = 0.7f;

        public class Optional<T> {
            public T Value { get; }
            public bool HasValue { get; }

            private Optional(T value, bool hasValue)
            {
                Value = value;
                HasValue = hasValue;
            }

            public static Optional<T> Some(T value, bool hasValue = true) => new Optional<T>(value, hasValue);
            public static Optional<T> None() => new Optional<T>(default, false);
        }

        public void Insert(K key, V value) {
            if(root == null) {
                root = new Node<K, V>(key, value);
                return;
            }
            root = InsertRecursive(root, key, value);
            root.Update(alpha);
            if (root.isScapegoat) targetNodes.Add(root);
            if (targetNodes.Count > 0) Rebuild(targetNodes[targetNodes.Count - 1]);

            foreach (var node in targetNodes)
                node.isScapegoat = false;

            targetNodes = new List<Node<K, V>>();
        }

         private Node<K, V> InsertRecursive(Node<K, V> node, K key, V value) {
            if (node == null)
                return new Node<K, V>(key, value);

            if (key.CompareTo(node.Key) < 0) {
                var tmp = InsertRecursive(node.Left, key, value);
                node.Left = tmp;
                tmp.Parent = node;
            } else if (key.CompareTo(node.Key) > 0) {
                var tmp = InsertRecursive(node.Right, key, value);
                node.Right = tmp;
                tmp.Parent = node;
            } else {
                node.Value = value; // Update the value
            }

            node.Update(alpha);
            if (node.isScapegoat) targetNodes.Add(node);
            return node;
        }
        
        /*
        To rebuild a subtree is to replace it with a 1/2-weight-balanced subtree containing
        the same nodes. This can be done easily in time O(Size(node)).  
        */

        private void Rebuild(Node<K, V> node) {
            var fixNode = node;
            BinaryTreeBalanced<K, V> auxTree = new BinaryTreeBalanced<K, V>(node, alpha);
            Node<K, V> newNode = auxTree.BuildBalancedTree(auxTree.InorderTraversal());

            if(fixNode != root) {
                newNode.Parent = fixNode.Parent;
                if(fixNode.Parent.Right == fixNode) fixNode.Parent.Right = newNode;
                else if(fixNode.Parent.Left == fixNode) fixNode.Parent.Left = newNode;
            }
            else 
                root = newNode;
        }
        
        public Optional<V> Search(K key) {
            var node = SearchRecursive(root, key);
            return node.HasValue ? Optional<V>.Some(node.Value.Value) : Optional<V>.None();
        }

        private Optional<Node<K, V>> SearchRecursive(Node<K, V> node, K key) {
            if (node == null)
                return Optional<Node<K, V>>.None();

            if (key.CompareTo(node.Key) < 0)
                return SearchRecursive(node.Left, key);
            else if (key.CompareTo(node.Key) > 0)
                return SearchRecursive(node.Right, key);
            else
                return Optional<Node<K, V>>.Some(node);
        }

        public Optional<V> Delete(K key) {
            if(root == null) return Optional<V>.None();
            var nodeToDelete = SearchRecursive(root, key);
            if (!nodeToDelete.HasValue) return Optional<V>.None();

            var result = DeleteRecursive(root, key);
            root = result.Value;
            root?.Update(alpha);
            return Optional<V>.Some(default(V));
        }

        private Optional<Node<K, V>> DeleteRecursive(Node<K, V> node, K key) {
            if (node == null)
                return Optional<Node<K, V>>.None();

            if (key.CompareTo(node.Key) < 0) {
                var tmp = DeleteRecursive(node.Left, key);
                node.Left = tmp.Value;
                if (tmp.Value != null) tmp.Value.Parent = node;
                node.Update(alpha);
                return Optional<Node<K, V>>.Some(node);
            } 
            else if (key.CompareTo(node.Key) > 0) {
                var tmp = DeleteRecursive(node.Right, key);
                node.Right = tmp.Value;
                if (tmp.Value != null) tmp.Value.Parent = node;
                node.Update(alpha);
                return Optional<Node<K, V>>.Some(node);
            } 
            else {
                if (node.Left == null && node.Right == null)
                    return Optional<Node<K, V>>.Some(null);

                else if (node.Left != null && node.Right != null) {

                    var successor = FindMin(node.Right);
                    var originalLeft = node.Left;
                    var tmp = DeleteRecursive(node.Right, successor.Key);

                    node.Key = successor.Key;
                    node.Value = successor.Value;
                    node.Left = originalLeft;
                    node.Right = tmp.Value;

                    if (tmp.Value != null) 
                        tmp.Value.Parent = node;

                    if (originalLeft != null) 
                        originalLeft.Parent = node;

                    node.Update(alpha);
                    return Optional<Node<K, V>>.Some(node);
                } 
                else {
                    var child = node.Left ?? node.Right;
                    child.Parent = node.Parent;
                    child.Update(alpha);
                    return Optional<Node<K, V>>.Some(child);
                }
            }
        }

        private Node<K, V> FindMin(Node<K, V> node) {

            Console.WriteLine($"baja-> :");
            Console.WriteLine("-----------------------");
            Console.WriteLine($"EL SUCCESOR ES :{node.Key}");
            Console.WriteLine("-----------------------");


            while (node?.Left != null) {
                node = node.Left;
            }

            Console.WriteLine("-----------------------");
            Console.WriteLine($"EL SUCCESOR ES :{node.Key}");
            Console.WriteLine("-----------------------");

            return node;
        }

        public void PrintTree() {
            PrintRecursive(root, 0);
        }

        private void PrintRecursive(Node<K, V> node, int level) {
            if (node == null) return;

            PrintRecursive(node.Right, level + 1); // Primero la derecha (para que aparezca arriba)

            Console.WriteLine(new string(' ', 20 * level) + $"{node.Key} => {node.Value}");

            PrintRecursive(node.Left, level + 1); // Luego la izquierda
        }
    }
}
