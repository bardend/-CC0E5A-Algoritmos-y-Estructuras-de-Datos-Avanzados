using System;
using System.Collections;
using System.Collections.Generic;

namespace ConsistentHash.src {

    public class MurmuHash<T> : IFunctionHash<T> {
        private readonly int _seed;
        private readonly int _mod;
        
        // Constructor con seed y mod
        public MurmuHash(int mod, int seed) {
            _mod = mod;
            _seed = seed;
        }
               
        
        public int Transform(T value) {
            int hashCode = EqualityComparer<T>.Default.GetHashCode(value);
            uint hash = MurmurHash3(hashCode, _seed);
            return Math.Abs((int)(hash % _mod)); // Asegurar resultado positivo
        }

        // Implementación de MurmurHash3 adaptada para enteros
        private static uint MurmurHash3(int data, int seed) {
            const uint c1 = 0xcc9e2d51;
            const uint c2 = 0x1b873593;
            const int r1 = 15;
            const int r2 = 13;
            const uint m = 5;
            const uint n = 0xe6546b64;
            
            uint hash = (uint)seed;
            
            // Convertir el entero en 4 bytes
            uint k = (uint)data;
            
            // Procesar los 4 bytes del entero como un bloque único
            k *= c1;
            k = RotateLeft(k, r1);
            k *= c2;
            
            hash ^= k;
            hash = RotateLeft(hash, r2);
            hash = hash * m + n;
            
            // Finalizar el hash
            hash ^= 4; // Length es siempre 4 bytes para un entero
            hash ^= hash >> 16;
            hash *= 0x85ebca6b;
            hash ^= hash >> 13;
            hash *= 0xc2b2ae35;
            hash ^= hash >> 16;
            
            return hash;
        }


        private static uint RotateLeft(uint x, int r) {
            return (x << r) | (x >> (32 - r));
        }
    }

 }
