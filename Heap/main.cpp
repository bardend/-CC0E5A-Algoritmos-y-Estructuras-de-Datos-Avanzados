#include <bits/stdc++.h>
#include "Heap.hpp"
#include "MedianHeap.hpp"
using namespace std;
int main() {
    MedianHeap<int, int> md;
    md.push(10, 10);
    md.push(5, 5);
    md.push(15, 15);
    
    // Get the median
    cout << "Median: " << md.get() << endl;

    md.push(4, 4);
    md.push(6, 6);

    cout << "Median: " << md.get() << endl;

    return 0;
}
