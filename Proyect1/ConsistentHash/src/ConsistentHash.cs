using System;
using System.Collections.Generic;


namespace ConsistentHash.src {
    public class ConsistentHash<T> {
        private int _size;
        private DSU _dsu;
        private int _currentNode = 0;
        private PerfectHash<T, int> _hash;
        private PerfectHash<int, T> _revHash;
        private Treap<int> _treap = new Treap<int>();
        
        public ConsistentHash(int size) {
            _size = size;
            GenerateDataStructure();
        }
        
        private void GenerateDataStructure() {
            _dsu = new DSU(_size);
            _hash = new PerfectHash<T, int>(_size);
            _revHash = new PerfectHash<int, T>(_size);
        }

        public void InsertCache(T cache) {
            _hash.Insert(cache, _currentNode);
            _revHash.Insert(_currentNode++, cache);
        }

        public T FindServer(T cache) { 
            if(_hash.Get(cache) == null)
            {
                throw new KeyNotFoundException("Key not found");
            }
            return _revHash.Get(_dsu.FindServer(_hash.Get(cache)));
        }

        public void DeleteServer(T server) {
            int id = _hash.Get(server);
            _dsu.Server[id] = _treap.UpperBound(id);
        }

        public void InsertServer(T server) {

            _hash.Insert(server, _currentNode);
            _revHash.Insert(_currentNode++, server);

            if(_treap.Size() == 0) {
               //_treap.Insert(server);
            }
        }

        // public void InsertServer(T server) {
        //     _hash.Insert(server, _currentNode);
        //     _revHash.Insert(_currentNode++, server);
        //     // int id = _hash.Get(server);
        //     // lazy.Update(NextPos(_treap.UpperBound(id)), BeforePos(id), -1);
        //     // _dsu.Server[id] = id;
        // }
        
    }
}
