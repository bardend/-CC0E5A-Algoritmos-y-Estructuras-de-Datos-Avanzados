using System;
using System.Text;

namespace ConsistentHash.src {
    public static class Utils {
        private static readonly Random rng = new Random((int)DateTime.Now.Ticks);

        public static int Random(int a, int b) {
            return a + rng.Next(0, b - a + 1);
        }
    }
}
