using System;
using ConsistentHash.src;

public class Program {
    public static void Main() {
        var treap = new Treap<int>();
        treap.Insert(new Node<int>(6));
        treap.Insert(new Node<int>(5));
        treap.Insert(new Node<int>(100));
        treap.Insert(new Node<int>(233));
        treap.Insert(new Node<int>(0));
        treap.Insert(new Node<int>(20));
        treap.Insert(new Node<int>(-2));
        treap.Print();

        var filter = new BloomFilter<int>(1000, 3);

        // 2. Insertar algunos números
        int[] numbersToAdd = { 42, 123, 789, 555, 10 };
        
        foreach (var number in numbersToAdd)
        {
            filter.Insert(number);
        }

        Console.WriteLine("\nProbando números que DEBERÍAN estar:");
        foreach (var number in numbersToAdd)
        {
            bool exists = filter.Contains(number);
            Console.WriteLine($"¿Contiene {number}? {(exists ? "Sí" : "No")} (CORRECTO: Debería ser Sí)");
        }

        // 4. Verificar números que NO están en el filtro
        int[] numbersNotAdded = { 1, 2, 3, 999, 1000 };
        foreach (var number in numbersNotAdded)
        {
            bool exists = filter.Contains(number);
            Console.WriteLine($"¿Contiene {number}? {(exists ? "Sí" : "No")} (CORRECTO: Debería ser No)");
        }

        var hash = new PerfectHash<string, int>(100);
        string testKey = "test_key";
        int testValue = 42;
        hash.Insert(testKey, testValue);

    }
}
