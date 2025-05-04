using System;
using System.Collections;
using System.Collections.Generic;

namespace ConsistentHash.src {
    public class DSU {
        private readonly int[] _server;
        private readonly int _size;

        public DSU(int size) {
            _size = size;
            _server = new int[size];
            Array.Fill(_server, -1);  
        }

        public void Insert(int hashValue) {
            var tmp = FindServer(hashValue);
        }

        public int FindServer(int hashWeb) {
            if (_server[hashWeb] == hashWeb)
                return hashWeb;
            //Lazy.query(hashWeb);
            _server[hashWeb] = (_server[hashWeb] == -1 ? FindServer((hashWeb+1)%_size)
                                                     : FindServer(_server[hashWeb]));
            return _server[hashWeb];
        }
    }
}
