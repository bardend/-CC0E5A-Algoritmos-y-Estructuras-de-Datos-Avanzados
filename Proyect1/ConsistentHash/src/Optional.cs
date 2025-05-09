using System;

namespace ConsistentHash.src {
    public class Optional<T> {
        public T Value {get;}
        public bool HasValue {get;}

        private Optional(T value, bool hasValue) {
            Value  = value;
            HasValue = hasValue;
        }
        public static Optional<T> Some(T value) => new Optional<T>(value, true);
        public static Optional<T> None() => new Optional<T>(default, false);
    }
}
