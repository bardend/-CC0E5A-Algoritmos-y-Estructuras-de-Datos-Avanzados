using NUnit.Framework;
using System;
using System.IO;
using ConsistentHash.src;

namespace ConsistentHash.Tests
{
    [TestFixture]
    public class ConsistentHashTests {

        private static Random _random = new Random();
        private string GenerateRandomString(int length)
        {
            const string chars = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789";
            char[] buffer = new char[length];
            for (int i = 0; i < length; i++)
                buffer[i] = chars[_random.Next(chars.Length)];
            return new string(buffer);
        }

        [Test]
        public void InsertCache_WithRandomString_InsertsSuccessfully()
        {
            // Arrange
            int size = 1000000;
            var hashSystem = new ConsistentHash<string>(size);

        }

    }
}
