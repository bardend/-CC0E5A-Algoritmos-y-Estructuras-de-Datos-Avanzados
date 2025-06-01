using NUnit.Framework;
using System;
using System.Diagnostics;
using System.IO;
using Proyect2.src;
using System.Globalization;


namespace ScapegoatTree.Tests
{
    public class ScapegoatTests
    {
        public List<List<double>> Create(float alpha = 0.7f, string namefield = "random_case.txt")  {
            var tree = new ScapegoatTree<int, string>(alpha);
            string[] lines = File.ReadAllLines(namefield);
            int n = int.Parse(lines[0]);

            var v = new List<List<double>>();
            for (int i = 0; i <= n; i++)
                v.Add(Enumerable.Repeat(0.0, 7).ToList()); // Usamos double para segundos

            for (int i = 1; i <= n; i++)
            {
                string line = lines[i].Trim();
                if (string.IsNullOrEmpty(line)) continue;

                string[] tokens = line.Split(' ');
                string command = tokens[0];
                int key = int.Parse(tokens[1]);
                string thirdToken = tokens[2];

                v[i][0] = i; // Índice de iteración
                for (int j = 1; j < 7; j++)
                    v[i][j] = v[i - 1][j]; // Copiar valores acumulados previos

                if (command == "INSERT")
                {
                    string value = thirdToken;
                    var sw = Stopwatch.StartNew();
                    tree.Insert(key, value);
                    sw.Stop();
                    v[i][1] = sw.Elapsed.TotalSeconds; // Tiempo individual en segundos
                    v[i][2] += sw.Elapsed.TotalSeconds; // Tiempo acumulado en segundos
                }
                else if (command == "SEARCH")
                {
                    var sw = Stopwatch.StartNew();
                    var result = tree.Search(key);
                    sw.Stop();
                    v[i][3] = sw.Elapsed.TotalSeconds; // Tiempo individual en segundos
                    v[i][4] += sw.Elapsed.TotalSeconds; // Tiempo acumulado en segundos
                }
                else if (command == "DELETE")
                {
                    var sw = Stopwatch.StartNew();
                    var result = tree.Delete(key);
                    sw.Stop();
                    v[i][5] = sw.Elapsed.TotalSeconds; // Tiempo individual en segundos
                    v[i][6] += sw.Elapsed.TotalSeconds; // Tiempo acumulado en segundos
                }
            }
            return v;
        }


        [Test]
        public void BenchmarkScapegoatTree() {
    string projectFolder = Directory.GetParent(Directory.GetCurrentDirectory()).Parent.Parent.Parent.FullName;
        string folder = Path.Combine(projectFolder, "input_csv", "random_input");
        Directory.CreateDirectory(folder);
        Console.WriteLine($"📁 Ruta absoluta: {Path.GetFullPath(folder)}"); // ← Añade esto


            for (float alpha = 0.6f; alpha <= 0.9f; alpha += 0.05f) {
                string alphaStr = alpha.ToString("0.00", CultureInfo.InvariantCulture).Replace('.', '_');
                string filePath = Path.Combine(folder, $"ScapegoatTree_alpha_{alphaStr}.csv");

                var data = Create(alpha, "random_case.txt");

                using var writer = new StreamWriter(filePath);
                writer.WriteLine("Index,InsertOp,InsertAcc,SearchOp,SearchAcc,DeleteOp,DeleteAcc");

                foreach (var row in data.Skip(1)) // omitir fila 0
                    writer.WriteLine(string.Join(",", row));

                Console.WriteLine($"✅ Archivo generado: {filePath}");
            }
        }


        [Test]
        public void BenchmarkScapegoatTreeWorstCase() {

    string projectFolder = Directory.GetParent(Directory.GetCurrentDirectory()).Parent.Parent.Parent.FullName;
        string folder = Path.Combine(projectFolder, "input_csv", "worst_input");
        Directory.CreateDirectory(folder);
        Console.WriteLine($"📁 Ruta absoluta: {Path.GetFullPath(folder)}"); // ← Añade esto

            for (float alpha = 0.6f; alpha <= 0.9f; alpha += 0.05f) {
                string alphaStr = alpha.ToString("0.00", CultureInfo.InvariantCulture).Replace('.', '_');
                string filePath = Path.Combine(folder, $"ScapegoatTree_alpha_{alphaStr}.csv");

                var data = Create(alpha, "worst_case.txt");

                using var writer = new StreamWriter(filePath);
                writer.WriteLine("Index,InsertOp,InsertAcc,SearchOp,SearchAcc,DeleteOp,DeleteAcc");

                foreach (var row in data.Skip(1)) // omitir fila 0
                    writer.WriteLine(string.Join(",", row));

                Console.WriteLine($"✅ Archivo generado: {filePath}");
            }
        }

        [Test]
        public void TestFromInputFile() {
            float alpha = 0.7f;
            var tree = new ScapegoatTree<int, string>(alpha);
            string[] lines = File.ReadAllLines("in.txt");
            int n = int.Parse(lines[0]);

            long insertTime = 0, deleteTime = 0, searchTime = 0;
            int insertCount = 0, deleteCount = 0, searchCount = 0;

            for (int i = 1; i <= n; i++)
            {
                string line = lines[i].Trim();
                if (string.IsNullOrEmpty(line)) continue;

                string[] tokens = line.Split(' ');
                string command = tokens[0];
                int key = int.Parse(tokens[1]);
                string thirdToken = tokens[2];

                if (command == "INSERT")
                {
                    string value = thirdToken;
                    var sw = Stopwatch.StartNew();
                    tree.Insert(key, value);
                    sw.Stop();
                    insertTime += sw.ElapsedTicks;
                    insertCount++;
                }
                else if (command == "SEARCH")
                {
                    var sw = Stopwatch.StartNew();
                    var result = tree.Search(key);
                    sw.Stop();
                    searchTime += sw.ElapsedTicks;
                    searchCount++;

                    if (thirdToken == "NO")
                    {
                        Assert.IsFalse(result.HasValue, $"Expected SEARCH {key} to be NO");
                    }
                    else
                    {
                        Assert.IsTrue(result.HasValue, $"Expected SEARCH {key} to find a value");
                        Assert.AreEqual(thirdToken, result.Value, $"SEARCH {key} mismatch");
                    }
                }
                else if (command == "DELETE")
                {
                    var sw = Stopwatch.StartNew();
                    var result = tree.Delete(key);
                    sw.Stop();
                    deleteTime += sw.ElapsedTicks;
                    deleteCount++;

                    if (thirdToken == "NO")
                    {
                        Assert.IsFalse(result.HasValue, $"Expected DELETE {key} to be NO (not found)");
                    }
                    else if (thirdToken == "YES")
                    {
                        Assert.IsTrue(result.HasValue, $"Expected DELETE {key} to be YES (found and deleted)");
                    }
                    else
                    {
                        Assert.Fail($"DELETE command expects YES or NO, got: {thirdToken}");
                    }
                }
            }

        }


        private static double TicksToMicroseconds(long ticks)
        {
            return ticks * 1_000_000.0 / Stopwatch.Frequency;
        }



    }
}

