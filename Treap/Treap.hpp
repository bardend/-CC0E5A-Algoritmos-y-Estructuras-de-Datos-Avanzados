#ifndef TREAP_HPP
#define TREAP_HPP

#include <bits/stdc++.h>
using namespace std;
class Treap {
    public:
        // Nodo del Treap
        struct Node {
            int key;
            int priority;
            Node* left;
            Node* right;
            
            // Constructor
            Node(int k, int p) : key(k), priority(p), left(nullptr), right(nullptr) {}
        };
        Node* root;


        pair<Node*, Node*> split(Node* v, int x) {
            if (v == nullptr) 
                return {nullptr, nullptr};
            if (v->key < x) { 
                auto p = split(v->right, x);
                v->right = p.first;
                return {v, p.second};
            }
            else {
                auto p = split(v->left, x);
                v->left = p.second;
                return {p.first, v};
            }
        }

        Node* merge(Node* a, Node* b) {
            if (a == nullptr) 
                return root = b;
            if (b == nullptr) 
                return root = a;
            if (a->priority > b->priority) { 
                a->right = merge(a->right, b);
                return root = a;
            }
            else {
                b->left = merge(a, b->left);
                return root = b;
            }
        }

        Node* add(Node* newNode) {
            Node* curNode = root;
            if (newNode->priority > curNode->priority) {
                auto p = split(curNode, newNode->key);
                newNode->left = p.first;
                newNode->right = p.second;
                return newNode;
            }
            return nullptr;
        }


};
#endif // TREAP_HPP
