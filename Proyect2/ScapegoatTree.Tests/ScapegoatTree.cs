using NUnit.Framework;
using System;
using System.Diagnostics;
using System.IO;
using Proyect2.src;

namespace ScapegoatTree.Tests
{
    public class ScapegoatTests
    {
        [Test]
        public void TestFromInputFile()
        {
            var tree = new ScapegoatTree<int, string>();
            string[] lines = File.ReadAllLines("in.txt");
            int n = int.Parse(lines[0]);

            // Métricas
            Stopwatch totalSw = Stopwatch.StartNew();
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

            totalSw.Stop();


            // Mostrar resultados
            Console.WriteLine("\n====== Resultados de Tiempo ======");
            Console.WriteLine($"INSERTs: {insertCount}, Promedio: {TicksToMicroseconds(insertTime / Math.Max(1, insertCount))} μs");
            Console.WriteLine($"SEARCHs: {searchCount}, Promedio: {TicksToMicroseconds(searchTime / Math.Max(1, searchCount))} μs");
            Console.WriteLine($"DELETEs: {deleteCount}, Promedio: {TicksToMicroseconds(deleteTime / Math.Max(1, deleteCount))} μs");
        }

        private static double TicksToMicroseconds(long ticks)
        {
            return ticks * 1_000_000.0 / Stopwatch.Frequency;
        }
    }
}

