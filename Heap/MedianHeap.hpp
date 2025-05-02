#include <bits/stdc++.h>
#include "Heap.hpp"
using namespace std;

template<typename T, typename P >
class MedianHeap {
    private :
        Heap<T, P> minHeap;
        Heap<T, P> maxHeap;
 
    public :
        MedianHeap() : minHeap([](P a, P b) { return a < b; }), maxHeap([](P a, P b) { return a > b; }) {}

        void push(const T& value, const P& priority) {
            if(maxHeap.empty()) maxHeap.push(value, priority);
            else if(minHeap.empty()) minHeap.push(value, priority);
            else {
                value <= maxHeap.topValue() ? maxHeap.push(value, priority) : minHeap.push(value, priority);
                while(minHeap.size() > maxHeap.size()) {
                    auto cur = minHeap.pop();
                    maxHeap.push(cur, cur);
                }
                while(maxHeap.size() > minHeap.size() + 1) {
                    auto cur = maxHeap.pop();
                    minHeap.push(cur, cur);
                }
            }
        }

        T get() { 
            return maxHeap.topValue();
        }

        int size() {
            return minHeap.size() + maxHeap.size();
        }
        bool empty() const {
            return (minHeap.empty() and maxHeap.empty());
        }
};
