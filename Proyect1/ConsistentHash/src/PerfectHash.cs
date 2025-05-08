using System;
using System.Collections;
using System.Collections.Generic;

namespace ConsistentHash.src {

     public class PerfectHash<T1, T2> { 

        // This is a typical Hash where (key, value) are stored in a list with size m.
        // Allowing O(1) access time.
        //
        // Warning:
        // This class does not yet handle the case when a unknow key is requested.

        private readonly MurmuHash<T1> hash;
        private readonly List<T2> mp;

        public PerfectHash(int m) {
            mp = new List<T2>(new T2[m]);
            Console.Write($"Longitud del Hash {m}");
            hash = new MurmuHash<T1>(m, Utils.Random(1, 100000));
        }

        public void Insert(T1 key, T2 value) {
            mp[hash.Hash(key)] = value;
        }
        public T2 Get(T1 key) {
            int index = hash.Hash(key);
            if (index >= 0 && index < mp.Count) {
                return mp[index];
            }
            throw new KeyNotFoundException("Key not found");
        }
    }
}
