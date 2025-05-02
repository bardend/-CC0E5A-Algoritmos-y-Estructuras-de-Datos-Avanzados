using System;
using System.Collections;
using System.Collections.Generic;


namespace ConsistentHash.src {
    
    public class MurmuHash<T> {
        private readonly int _seed;
        private readonly int _mod;
        
        // Constructor con seed y mod
        public MurmuHash(int seed, int mod) {
            _seed = seed;
            _mod = mod;
        }
        
        public int Hash(T value) {
            int hashCode = EqualityComparer<T>.Default.GetHashCode(value);
            byte[] bytes = BitConverter.GetBytes(hashCode);
            uint hash = MurmurHash3(bytes, _seed);
            return Math.Abs((int)(hash % _mod)); // Asegurar resultado positivo
        }
        
        // Implementación de MurmurHash3
        private static uint MurmurHash3(byte[] data, int seed) {
            const uint c1 = 0xcc9e2d51;
            const uint c2 = 0x1b873593;
            const int r1 = 15;
            const int r2 = 13;
            const uint m = 5;
            const uint n = 0xe6546b64;
            
            uint hash = (uint)seed;
            int length = data.Length;
            int blockCount = length / 4;
            
            for (int i = 0; i < blockCount; i++)
            {
                uint k = BitConverter.ToUInt32(data, i * 4);
                k *= c1;
                k = RotateLeft(k, r1);
                k *= c2;
                hash ^= k;
                hash = RotateLeft(hash, r2);
                hash = hash * m + n;
            }
            
            int remainder = length & 3;
            if (remainder > 0)
            {
                uint k = 0;
                for (int i = 0; i < remainder; i++)
                {
                    k ^= (uint)data[blockCount * 4 + i] << (i * 8);
                }
                k *= c1;
                k = RotateLeft(k, r1);
                k *= c2;
                hash ^= k;
            }
            
            hash ^= (uint)length;
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
