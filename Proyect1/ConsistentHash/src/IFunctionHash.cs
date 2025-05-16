using System;
using System.Collections.Generic;

// Concept C++
// Interface Requirements:
// - Any implementing class must define:
//     1. A method `Transform`.
//     2. The method must take a parameter of type `T` and return an `int`.

namespace ConsistentHash.src {
    public interface IFunctionHash<T> {
        int Transform(T x);
    }
}
