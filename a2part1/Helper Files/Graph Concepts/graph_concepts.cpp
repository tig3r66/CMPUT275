//  ==================================
//  Name: Edward (Eddie) Guo
//  ID: 1576381
//  CMPUT 275, Winter 2020
//
//  Weekly Exercise 5: Graph Concepts
//  ==================================

#include <iostream>
#include <string>
#include <fstream>
#include <vector>
#include <unordered_map>
#include "digraph.h"

using namespace std;


/*
    Description: DFS algorithm that marks all visited vertices as true.

    Arguments:
        graph (Digraph const*): the digraph instance to traverse.
        startVertex (int): the start vertex of the search tree.
        reached (unordered_map<int, bool>): boolean map of if vertices are
            reached - true = reached, false = not reached.
*/
void depthFirstSearch(Digraph const* graph,
    int startVertex, unordered_map<int, bool>* reached
) {
    // base case: if we have already reached a vertex, return
    // otherwise, we have not searched startVertex
    if (!(*reached)[startVertex]) (*reached)[startVertex] = true;
    else return;

    // performing recursive search for all adjacent vertices
    for (auto iter = graph->neighbours(startVertex);
        iter != graph->endIterator(startVertex); iter++
    ) {
    	depthFirstSearch(graph, *iter, reached);
    }
}


/*
    Description: counts all the connected components in a given graph instance.

    Arguments:
        graph (Digraph*): the digraph instance to traverse.
    Returns:
        num_connected (int): the number of connected components in the graph.
*/
int count_components(Digraph* graph) {
    int num_connected = 0;
    unordered_map<int, bool> reached;
    vector<int> vertices = graph->vertices();

    // none of the vertices have been traversed yet
    for (auto v : vertices) {
        reached[v] = false;
    }

    // the number of connected components (num_connected) is the number of
    // search trees that DFS needs to create to mark all vertices as true
    for (auto v : vertices) {
        if (!reached[v]) {
            depthFirstSearch(graph, v, &reached);
            num_connected++;
        }
    }

    return num_connected;
}


/*
    Description: reads in a .txt file in "digraph" format. If the file is not
    found, then the program prints the correct usage to the command-line and
    exits the program.

    Arguments:
        filename (char []): the file to read.
    Returns:
        digraphPtr (Digraph*): the built graph instance.
    Notes:
    	Assumes the graph file follows the digraph format.
*/
Digraph* read_city_graph_undirected(char filename[]) {
    char graphID, comma;
    int ID, start, end;
    float lat, lon;
    string name;

    Digraph* digraphPtr = new Digraph;

    // input validation
    ifstream textIn(filename);
    if (!textIn.good()) {
        cout << "error: digraph text file not found" << endl;
        cout << "usage: ./graph_concepts <digraph_file_name>.txt" << endl;
        exit(EXIT_FAILURE);
    }

    // parsing
    while (textIn >> graphID >> comma) {
        if (graphID == 'V') {
            // vertex format: V,ID,Lat,Lon
            textIn >> ID >> comma >> lat >> comma >> lon;
            digraphPtr->addVertex(ID);
        } else if (graphID == 'E') {
            // edge format: E,start,end,name
            textIn >> start >> comma >> end >> comma;
            getline(textIn, name);
            digraphPtr->addEdge(start, end);
            digraphPtr->addEdge(end, start);
        }
    }

    textIn.close();
    // must be deleted in the function that calls this function
    return digraphPtr;
}


/*
    Description: given a command-line argument of the digraph .txt file to read,
    it prints the number of connected components in the graph.

    Arguments:
        argc (int): the number of command-line arguments.
        argv (char* []): argv[1] is the supplied .txt digraph file.
*/
int main(int argc, char *argv[]) {
    Digraph* digraphPtr = read_city_graph_undirected(argv[1]);
    cout << count_components(digraphPtr) << endl;

    delete digraphPtr;
    return 0;
}
