using System;

namespace ConsistentHash.src {

    public class Treap<T> where T : IComparable<T> {
        
        private Node<T> root;

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
        }
        
        public void Delete(Node<T> newNode) {
            if(root == null) 
                return;
            var (left, right) = Split(root, newNode.X);
            var (left2, right2) = Split(right, newNode.NextVal());
            root = Merge(left, right2);
            return ;
        }

        public void Print() => PrintRecursive(root, 0);

        public Node<T> UpperBound(T value) {
            return UpperBound(root, value);
        }

        private Node<T> UpperBound(Node<T> node, T value) {
            if(node == null) 
                return null;

            if (node.X.CompareTo(value) > 0) {
                Node<T> leftResult = UpperBound(node.Left, value);
                return leftResult ?? node;
            }
            else 
                return UpperBound(node.Right, value);
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
