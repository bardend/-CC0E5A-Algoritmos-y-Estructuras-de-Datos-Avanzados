using System;

//Node.cs
namespace Proyect2.src {

    public class Node<K, V> {
        public K Key {get; set;}
        public V Value {get; set;}
        public Node<K, V> Left {get; set;} = null;
        public Node<K, V> Right {get; set;} = null;
        public Node<K, V> Parent{get; set;} = null;
        public int Size{get; set;}
        public int Height {get; set;}
        public bool isScapegoat{get; set;} = false;

        public Node(K key, V value) {
            Key = key;
            Value = value;
            Size = 1;
        }

        //We need to climb the tree,
        // I update the state of the sub-tre
        public void Update(float alpha) {
            Size = 1 + (Left?.Size ?? 0) + (Right?.Size ?? 0);
            isScapegoat = (Left?.Size > Size * alpha) || (Right?.Size > Size * alpha);
        }
    }
}
