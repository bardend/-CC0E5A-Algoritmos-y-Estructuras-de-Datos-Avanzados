using System;
using System.Collections;
using System.Collections.Generic;

namespace ConsistentHash.src {
    public class DSU {
        public int[] Server;
        private readonly int _size;

        public DSU(int size) {
            _size = size;
            Server = new int[size];
            Array.Fill(Server, -1);  
        }

        public void Insert(int hashValue) {
            var tmp = FindServer(hashValue);
        }

        public int FindServer(int hashWeb) {
            if (Server[hashWeb] == hashWeb)
                return hashWeb;

            Server[hashWeb] = (Server[hashWeb] == -1 
                ? FindServer((hashWeb + 1) % _size)
                : FindServer(Server[hashWeb]));

            return Server[hashWeb];
        }
    }
}

