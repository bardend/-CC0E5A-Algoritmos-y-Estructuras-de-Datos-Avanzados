using System;
using NUnit.Framework;
using ConsistentHash.src;
using System.Collections.Generic;
using System.IO;


namespace ConsistentHash.Tests
{
    [TestFixture]
    public class PerfectHashTests
    {
        [Test]
        public void InsertAndRetrieveValues_ShouldReturnCorrectValue() {
            var perfectHash = new PerfectHash<string, int>(10);

            perfectHash.Insert("apple", 1);
            perfectHash.Insert("banana", 2);
            Assert.AreEqual(1, perfectHash.Get("apple"));
            Assert.AreEqual(2, perfectHash.Get("banana"));
        }

        [Test]
        public void Insert_DuplicateKey_OverwritesValue() {
            // Arrange
            var perfectHash = new PerfectHash<string, int>(1000);
            
            // Act
            perfectHash.Insert("apple", 1);
            perfectHash.Insert("apple", 42); // Misma clave, nuevo valor
            Assert.AreEqual(42, perfectHash.Get("apple"));
        }
    }
}
