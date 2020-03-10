//  ==================================
//  Name: Edward (Eddie) Guo
//  ID: 1576381
//  CMPUT 275, Winter 2020
//
//  Weekly Exercise 5: Graph Concepts
//  ==================================

#include <iostream>
#include <queue>
#include <unordered_map>
#include <list>
#include "digraph.h"

using namespace std;

unordered_map<int, int> breadthFirstSearch(const Digraph& graph,
    int startVertex
) {
    unordered_map<int, int> searchTree; // map each vertex to its predecessor

    searchTree[startVertex] = -1;

    queue<int> q;
    q.push(startVertex);

    while (!q.empty()) {
        int v = q.front();
        q.pop();

        for (auto iter = graph.neighbours(v); iter != graph.endIterator(v);
            iter++
        ) {
            if (searchTree.find(*iter) == searchTree.end()) {
                searchTree[*iter] = v;
                q.push(*iter);
            }
        }
    }

    return searchTree;
}

int main() {
    Digraph graph;

    int n, m;
    cin >> n >> m;
    for (int i = 0; i < n; ++i) {
        int label;
        cin >> label;
        graph.addVertex(label);
    }
    for (int i = 0; i < m; ++i) {
        int u, v;
        cin >> u >> v;
        graph.addEdge(u, v);
    }
    int startVertex, endVertex;

    cin >> startVertex >> endVertex;

    unordered_map<int, int> searchTree = breadthFirstSearch(graph, startVertex);

    // print searchTree
    cout << "Search tree:" << endl;
    for (auto it : searchTree) {
        cout << it.first << ": " << it.second << endl;
    }
    cout << endl;

    list<int> path;
    if (searchTree.find(endVertex) == searchTree.end()) {
        cout << "Vertex " << endVertex << " not reachable from "
            << startVertex << endl;
    } else {
        int stepping = endVertex;
        while (stepping != startVertex) {
        path.push_front(stepping);
        stepping = searchTree[stepping]; // crawl up the search tree one step
        }
        path.push_front(startVertex);

        cout << "Path from " << startVertex << " to " << endVertex << ":";
        for (auto it : path) {
        cout << ' ' << it;
        }
        cout << endl;
    }

    return 0;
}
