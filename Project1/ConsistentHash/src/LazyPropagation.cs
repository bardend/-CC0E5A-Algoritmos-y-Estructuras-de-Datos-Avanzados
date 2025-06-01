// using System;
// using System.Collections.Generic;
//
//
// namespace ConsistentHash.src {
//     public class LazyPropagation {
//         private int len;
//         private int[] lazy;
//         private int[] server;
//
//         public void Init(int n) {
//             len = n;
//             lazy = new int[4 * n];
//             server = new int[4 * n];
//             Array.Fill(server, -1); // Initialize with -1 (no server assigned)
//         }
//
//         public void Build(int v, int lo, int hi) {
//             if (lo == hi) {
//                 server[v] = -1; // No server assigned initially
//                 return;
//             }
//             int mid = (lo + hi) / 2;
//             Build(2 * v, lo, mid);
//             Build(2 * v + 1, mid + 1, hi);
//         }
//
//         private void Push(int v, int newServer = -1) {
//             if (lazy[v] != 0) {
//                 lazy[2 * v] = lazy[2 * v + 1] = 1;
//
//                 if(update) {
//                     server[2 * v] = server[2 * v + 1] = newServer;
//                 }
//
//                 lazy[v] = 0; 
//             }
//         }
//
//         public void Update(int l, int r, int newServer, int v, int lo, int hi) {
//             if (l > hi || r < lo) return;
//
//             if (l <= lo && r >= hi) {
//                 server[v] = newServer;
//                 lazy[v] = 0; 
//                 return;
//             }
//
//             Push(v, newServer); 
//
//             int mid = (lo + hi) / 2;
//             Update(l, r, newServer, 2 * v, lo, mid);
//             Update(l, r, newServer, 2 * v + 1, mid + 1, hi);
//
//             // Combine if children agree and no pending lazy
//             if (server[2 * v] == server[2 * v + 1] && lazy[v] == 0) {
//                 server[v] = server[2 * v];
//             }
//         }
//         public int Query(int pos, int v, int lo, int hi) {
//             if (lo == hi) return server[v];
//
//             Push(v);
//
//             int mid = (lo + hi) / 2;
//             return pos <= mid 
//                 ? Query(pos, 2 * v, lo, mid) 
//                 : Query(pos, 2 * v + 1, mid + 1, hi);
//         }
//
//         // Public API
//         public void UpdateRange(int left, int right, int newServer) => 
//             Update(left, right, newServer, 1, 0, len - 1);
//
//         public int QueryPoint(int pos) => 
//             Query(pos, 1, 0, len - 1);
//     }
// }
