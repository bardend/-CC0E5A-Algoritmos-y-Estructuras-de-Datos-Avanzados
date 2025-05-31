using System;
using Proyect2.src;

class Program {
    public static void Main() {
        // Crear un mapa de string a int
        var map = new ScapegoatTree<int, string>();
        
        Console.WriteLine("=== Demo ScapegoatTree<string, int> ===\n");
        
        // Insertar algunos elementos
        Console.WriteLine("Insertando elementos...");
        map.Insert(25, "alice");
        map.Insert(30, "bob");

        Console.WriteLine("=====================================");
        map.PrintTree();
        map.Insert(35, "charlie");
        Console.WriteLine("=====================================");
        map.Insert(28, "david");
        map.PrintTree();
        Console.WriteLine($"ROOT: {map.root.Key}");

        //map.Insert(22, "eve");
        // Console.WriteLine("Elementos insertados: alice=25, bob=30, charlie=35, david=28, eve=22\n");

        int key = 30;
        var result = map.Search(key);
        if (result.HasValue) {
            Console.WriteLine($"  Encontrado: {key} = {result.Value}");
        } else {
            Console.WriteLine($"  No encontrado: {key}");
        }


    }
}
