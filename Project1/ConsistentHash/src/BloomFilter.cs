using System;
using System.Collections;
using System.Collections.Generic;

namespace ConsistentHash.src  {
    //public class BloomFilter<T> where T : IComparable<T>  {
    public class BloomFilter<T> {

        public BitArray Mask { get; set; }
        private List<MurmuHash<T>> hashFamily = new List<MurmuHash<T>>();

        public BloomFilter(int m, int k)  {
            Mask = new BitArray(m);
            for(int i = 0; i < k; i++) 
            {
                hashFamily.Add(new MurmuHash<T>(Utils.Random(1, 100000), m));
            }
        }

        public void Insert(T value) {
            foreach(var hashFunc in hashFamily) {
                Mask[hashFunc.Transform(value)] = true;
            }
        }

        public bool Contains(T value) {
            foreach(var hashFunc in hashFamily) {
                if(!Mask[hashFunc.Transform(value)])
                    return false;
            }
            return true;
        }
    }
}
