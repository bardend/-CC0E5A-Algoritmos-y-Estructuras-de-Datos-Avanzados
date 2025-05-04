using System;
using System.Collections;
using System.Collections.Generic;

namespace ConsistentHash.src {
//     public class PerfectHash<T1, T2> where T1 : IComparable<T1> {
     public class PerfectHash<T1, T2> {  // Remove the constraint here


        private readonly MurmuHash<T1> hash;
        private readonly List<T2> mp;

        public PerfectHash(int m) {
            mp = new List<T2>(new T2[m]);
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
