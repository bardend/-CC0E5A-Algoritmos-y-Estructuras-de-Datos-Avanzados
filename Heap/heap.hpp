#include <vector>
#include <functional>
#include <algorithm>

using namespace std;

template <typename T, typename P = int, typename Compare = std::less<P>>
class Heap {
    private:
    // Node as a pair of value and priority
    struct Node {
        T value;
        P priority;
        // Constructor
        Node(const T& v, const P& p) : value(v), priority(p) {}
    };

    vector<Node> heap;
    Compare comp;
    int bf = 2; //branching factor


    int parent(int i) { return (i - 1) / bf; }
    int first_child_index(int i) { return  bf*i + 1; }

    void pushDown(int id) {
        while(first_child_index(id) < size()) { //I'm not leaf
            int selectChild = first_child_index(id);
            for(int i = first_child_index(id); i < min(size(), first_child_index(id) + bf); i++) {
                if(comp(heap[i].priority, heap[selectChild].priority))
                selectChild = i;
            }

            if(!comp(heap[id].priority, heap[selectChild].priority)) {
                swap(heap[id], heap[selectChild]);
                id = selectChild;
            }
            else 
                break;
        }
    }

    void bubbleUp(int id) {
        while(id > 0 and comp(heap[id].priority, heap[parent(id)].priority)) {
            swap(heap[id], heap[parent(id)]);
            id = parent(id);
        }
    }


    public:

    //Constructors
    Heap() = default;
    Heap(int branching_factor): bf(branching_factor){}

    void push(const T& value, const P& priority) {
        heap.push_back(Node(value, priority));
        bubbleUp(heap.size() - 1);
    }

    // Remove and return top element (with highest/lowest priority)
    T pop() {
        T top = heap[0].value;
        heap[0] = heap.back();
        heap.pop_back();

        if(!heap.empty()) 
            pushDown(0);

        return top;
    }

    const T& topValue() const {
        return heap[0].value;
    }

    // Check if heap is empty
    bool empty() const {
        return heap.empty();
    }

    int size() const {
        return heap.size();
    }
};
