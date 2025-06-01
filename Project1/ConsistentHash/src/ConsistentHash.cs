using System;
using System.Collections.Generic;


//Pending implementing some concepts about the _hash will be <T, int> and _revHash will be <int, T>
//in the class ConsistentHash you vallidate that because probably the _hash would be a input.
//
namespace ConsistentHash.src {
    public class ConsistentHash<T> {
        private int _size;
        private DSU _dsu;
        private int _currentNode = 0;
        private PerfectHash<T, int> _hash;
        private PerfectHash<int, T> _revHash;
        private Treap<int> _treap;
        
        public ConsistentHash(int size) {
            _size = size;
            GenerateDataStructure();
        }
        
        private void GenerateDataStructure() {
            _dsu = new DSU(_size);
            _hash = new PerfectHash<T, int>(_size);
            _revHash = new PerfectHash<int, T>(_size);
            _treap = new Treap<int>();
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
            var up = _treap.UpperBound(id);
            _dsu.Server[id] = up.HasValue ? up.Value : _treap.UpperBound(int.MinValue).Value;
        }

        public void InsertServer(T server) {
            _hash.Insert(server, _currentNode);
            _revHash.Insert(_currentNode++, server);

            if(_treap.Size == 0) {
                _treap.Insert(_hash.Get(server));
            }
            else {
                int currentPos = _hash.Get(server);
                var nearest = _treap.NearestMinor(currentPos);
                Optional<int> bound = nearest.HasValue ? nearest 
                                                       : _treap.NearestMinor(int.MaxValue);

                for(int node = bound.Value; ; node = (node+1)%_size) {
                    _dsu.Server[node] = currentPos;
                    if(node == currentPos) break;
                }
            }
        }
    }
}
