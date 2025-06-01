using System;
using System.Collections.Generic;


namespace ConsistentHash.src {

    // This is a typical Hash where (key, value)
    public interface IHash<T1, T2> {
        void Insert(T1 key, T2 value);
        T2 Get(T1 key);
    }
}
