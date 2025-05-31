using System;
using System.Collections.Generic;

namespace Proyect2.src {

    public class BinaryTreeBalanced<K, V> where K : IComparable<K> {
        private Node<K, V> root;
        private float alpha = 0.7f;

        public BinaryTreeBalanced(Node<K, V> node, float alp) {
            root = node;
            alpha = alp;
        }

        public List<Node<K, V>> InorderTraversal() {
            List<Node<K, V>> nodes = new List<Node<K, V>>();
            Stack<Node<K, V>> stack = new Stack<Node<K, V>>();
            Node<K, V> current = root;

            while (current != null || stack.Count > 0) {
                while (current != null) {
                    stack.Push(current);
                    current = current.Left;
                }

                current = stack.Pop();
                nodes.Add(current);
                current = current.Right;
            }

            foreach(var x in nodes) {
                x.Right = null;
                x.Left = null;
                x.Parent = null;
            }

            return nodes;
        }

        public Node<K, V> BuildBalancedTree(List<Node<K, V>> nodes) {

            return BuildBalancedTreeHelper(nodes, 0, nodes.Count - 1);
        }

        private Node<K, V> BuildBalancedTreeHelper(List<Node<K, V>> nodes, int start, int end) {
            if (start > end)
                return null;

            int mid = start + (end - start) / 2;
            Node<K, V> node = nodes[mid];

            node.Left = BuildBalancedTreeHelper(nodes, start, mid - 1);
            if (node.Left != null)
                node.Left.Parent = node;

            node.Right = BuildBalancedTreeHelper(nodes, mid + 1, end);
            if (node.Right != null)
                node.Right.Parent = node;

            node.Update(alpha);
            return node;
        }
    }
}

