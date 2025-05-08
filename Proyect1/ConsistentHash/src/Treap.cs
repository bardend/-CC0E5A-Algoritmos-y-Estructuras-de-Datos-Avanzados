using System;

namespace ConsistentHash.src {

    public class Treap<T> where T : IComparable<T> {
        
        private Node<T> root;
        private int _size = 0;


        public Node<T> Split(T newX){
            Node<T> current = root;  
            var (left, right) = Split(current, newX);
            return Merge(left, right);
        }

        private (Node<T> left, Node<T> right) Split(Node<T> current, T newX) {
            if (current == null) {
                return (null, null);
            }

            if (current.X.CompareTo(newX) < 0) { //current.X < newX
                var (left, right) = Split(current.Right, newX);
                current.Right = left;
                return (current, right);
            }
            else {
                var (left, right) = Split(current.Left, newX);
                current.Left = right;
                return (left, current);
            }
        }

        private Node<T> Merge(Node<T> a, Node<T> b) {
            // Get the order -> a.x < b.x
            if(a == null) 
                return b;
            if(b == null) 
                return a;
            if(b.Y.CompareTo(a.Y) < 0) {
                a.Right = Merge(a.Right, b);
                return a;
            }
            else {
                b.Left = Merge(a, b.Left);
                return b;
            }
        }

        public void Insert(Node<T> newNode) {
            if(root == null) 
                root = newNode;
            else {
                var (left, right) = Split(root, newNode.X);
                root = Merge(Merge(left, newNode), right);
            }
            _size += 1;
        }

        public void Insert(T newVal) {
            Insert(new Node<T>(newVal));
        }
        
        public void Delete(Node<T> newNode) {
            if(root == null) 
                return;
            var (left, right) = Split(root, newNode.X);
            var (left2, right2) = Split(right, newNode.NextVal());
            root = Merge(left, right2);
            _size -= 1;
            return ;
        }

        public void Print() => PrintRecursive(root, 0);

        public T UpperBound(T value) {
            return UpperBound(root, value);
        }

        private T UpperBound(Node<T> node, T value) {
            if(node == null) 
                return default(T);  // Changed from null to default(T) for value types

            if (node.X.CompareTo(value) > 0) {
                T leftResult = UpperBound(node.Left, value);
                return leftResult ?? node.X;
            }
            else 
                return UpperBound(node.Right, value);
        }

        public int Size {
            get { return _size; }
        }

        private void PrintRecursive(Node<T> node, int depth) {
            if (node == null) return;

            Console.WriteLine(new string(' ', 8 * depth) + node);
            if(node.Right != null) {
                Console.WriteLine("Right");
                PrintRecursive(node.Right, depth + 1);
            }
            if(node.Left != null) { 
                Console.WriteLine("Left");
                PrintRecursive(node.Left, depth + 1);
            }
        }

    }
}
