using System;

namespace ConsistentHash.src {

    public class Node<T> where T : IComparable<T> {
        public T X { get; }          
        public int Y { get; }       
        public Node<T> Left { get; set; }
        public Node<T> Right { get; set; }

        public Node(T value)
        {
            X = value;
            Y = Utils.Random(1, 1000000);
        }

        // Métodos de comparación (solo devuelven bool como solicitaste)
        public bool CompX(Node<T> other) => X.CompareTo(other.X) < 0;
        public bool CompY(Node<T> other) => Y.CompareTo(other.Y) < 0;
        public override string ToString() => $"{X} (Priority: {Y})";

        public T NextVal() {
            if (typeof(T) == typeof(int)) {
                int next = (int)(object)X + 1;
                return (T)(object)next;
            }
            throw new NotSupportedException($"NextX is only supported for int, but T is {typeof(T)}");

        }
    }
}

