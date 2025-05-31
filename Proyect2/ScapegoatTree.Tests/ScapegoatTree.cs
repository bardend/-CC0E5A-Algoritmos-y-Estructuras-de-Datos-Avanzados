using NUnit.Framework;
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

            for (int i = 1; i <= n; i++)
            {
                Console.WriteLine(i);
                string[] tokens = lines[i].Split(' ');
                string command = tokens[0];
                int key = int.Parse(tokens[1]);

                if (command == "INSERT")
                {
                    string value = tokens[2];
                    tree.Insert(key, value);
                }
                else if (command == "SEARCH")
                {
                    string expected = tokens[2];
                    var result = tree.Search(key);
                    if (expected == "NO")
                    {
                        Assert.IsFalse(result.HasValue, $"Expected SEARCH {key} to be NO");
                    }
                    else
                    {
                        Assert.IsTrue(result.HasValue, $"Expected SEARCH {key} to find a value");
                        Assert.AreEqual(expected, result.Value, $"SEARCH {key} mismatch");
                    }
                }
                else if (command == "DELETE")
                {
                    string expected = tokens[2];
                    var result = tree.Delete(key);
                    if (expected == "NO")
                    {

                        Console.Write("AQUI");
                        Assert.IsFalse(result.HasValue, $"Expected DELETE {key} to be NO");
                    }
                    else
                    {
                        Assert.IsTrue(result.HasValue, $"Expected DELETE {key} to be YES");
                    }
                }
                Console.Write(i);
            }
        }
    }
}

