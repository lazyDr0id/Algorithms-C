#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "queue.h"

// This is used to store the information of the graph in the adjacency list
struct Vertex {
    int value;
    struct Vertex *next;
};
typedef struct Vertex Vertex;

// This is used to store the information of vertices in the Queue while performing BFS
// This is also used as a node for creating the BFS Tree
struct VertexInfo {
    int value;
    struct VertexInfo *parent;
    struct VertexInfo *leftChild;
    struct VertexInfo *rightSibling;
};
typedef struct VertexInfo VertexInfo;

void printGraphAdjacencyList(Vertex **vertices, int numberOfVertices);
void breadFirstTraversal(Queue *q, Vertex **vertices, bool *isVisited, VertexInfo **vertexInfos, int numberOfVertices, bool *independentSetData);

// Global vars
bool isGraphBipartite = true;
int cycleLeftNodeIndex;
int cycleRightNodeIndex;

int main(int argc, char **argv)
{
    // Check if the command line arguments are correct
    if (argc != 2) {
        printf("Please run the program by the command - 'program_name input_file'\n");
        return 0;
    }
    
    // Open the input file
    char *inputFileName = argv[1];
    FILE *inputFile = fopen(inputFileName, "r");
    
    // Check if the file is valid
    if (inputFile == NULL) {
        printf("The file %s is invalid. Please check and try again.\n", inputFileName);
        return 0;
    }
    
    // Read the number of vertices and the starting vertex of the BF traversal
    int numberOfVertices; int startingVertex;
    fscanf(inputFile, "%d %d\n", &numberOfVertices, &startingVertex);
    
    // Check if given input is valid or not
    if (numberOfVertices < 1) {
        printf("The graph needs to have atleast 1 vertex. Please enter a valid input and try again.\n");
        return 0;
    }
    if (startingVertex > numberOfVertices) {
        printf("Vertex %d is given as the starting vertex in the input, but the graph has only %d vertices in total. Please enter a valid input and try again.\n", startingVertex, numberOfVertices);
        return 0;
    }
    
    startingVertex--; // The index starts from 0. The inputfile has it from 1. So adjusting by 1.
    
    // Form a adjacency list of the graph
    // (All the index are starting from 0, adjusted by 1 from inputfile)
    // (This is just done in code. The output will again be adjusted to match the indexing in inputfile.)
    Vertex **vertices = calloc(sizeof(Vertex *) , numberOfVertices);
    char *buffer = NULL; size_t bufferSize = 0;
    int n, bytesread; Vertex **v;
    for (int i = 0; i < numberOfVertices; i++) {
        getline(&buffer, &bufferSize, inputFile);
        char *temp = buffer;
        v = vertices + i;
        while (sscanf(buffer, "%d%n", &n, &bytesread) > 0) {
            // Check if the vertex is within the given range
            if (n > numberOfVertices || n < 1) {
                printf("Vertex %d is given as the neighbour of vertex %d, but the graph has only %d vertices in total. Please enter a valid input and try again.\n", n, i + 1, numberOfVertices);
                return 0;
            }
            if (i == n - 1) {
                // Ignore if the vertex is connected to itself
                buffer += bytesread;
                continue;
            }
            Vertex *new = malloc(sizeof(Vertex));
            new->value = n-1;
            new->next = NULL;
            *v = new;
            v = &(new->next);
            buffer += bytesread;
        }
        buffer = temp;
        free(temp);
        buffer = NULL;
        bufferSize = 0;
    }
    
    Queue *q;
    newQueue(&q);
    
    VertexInfo *p = malloc(sizeof(VertexInfo));
    p->value = startingVertex;
    enqueue(q, p);
    
    bool *isVisited = calloc(sizeof(bool) , numberOfVertices);
    isVisited[p->value] = true;
    
    // Saves which independent set a perticular vertex belongs to
    // There are two available independent sets, membership of whose is signified by either 0(false) or 1(true) entry in this array for the corresponding vertex
    bool *independentSetData = calloc(sizeof(bool), numberOfVertices);
    independentSetData[p->value] = true;
    
    VertexInfo **vertexInfos = malloc(sizeof(VertexInfo *) * numberOfVertices);
    vertexInfos[p->value] = p;
    
    // Do a BFS Traversal to know if the graph has any cycle, and collect relevant info
    breadFirstTraversal(q, vertices, isVisited, vertexInfos, numberOfVertices, independentSetData);
    
    // If the graph is found to be bipartite, print the vertices in both the independent sets.
    // Note: The index in code starts from 0. The inputfile has it from 1. So incrementing by 1 while printing.
    if (isGraphBipartite) {
        printf("Yes (");
        
        int lastVertexIn1stIndependentSet = -1;
        int lastVertexIn2ndIndependentSet = -1;
        // Find the last vertices in both the independent sets
        for (int i = numberOfVertices - 1; i >= 0; i--) {
            if (lastVertexIn1stIndependentSet == -1) {
                if (independentSetData[i] == true) {
                    lastVertexIn1stIndependentSet = i;
                }
            }
            if (lastVertexIn2ndIndependentSet == -1) {
                if (independentSetData[i] == false) {
                    lastVertexIn2ndIndependentSet = i;
                }
            }
            
            if (lastVertexIn1stIndependentSet != -1 &&
                lastVertexIn2ndIndependentSet != -1) {
                break;
            }
        }
        
        // Print all the vertex of the 1st independent set
        for (int i = 0; i < numberOfVertices; i++) {
            
            if(independentSetData[i]) {
                if (i == lastVertexIn1stIndependentSet) {
                    printf("%d", i+1);
                } else {
                    printf("%d ", i+1);
                }
            }
        }
        
        printf(") (");
        
        // Print all the vertex of the 2nd independent set
        for (int i = 0; i < numberOfVertices; i++) {
            if(!independentSetData[i]) {
                if (i == lastVertexIn2ndIndependentSet) {
                    printf("%d", i+1);
                } else {
                    printf("%d ", i+1);
                }
            }
        }
        printf(")\n");
    } else {
        // Print the witness cycle
        printf("No (");
        VertexInfo *head = NULL;
        while (cycleLeftNodeIndex != cycleRightNodeIndex) {
            printf("%d ", cycleLeftNodeIndex + 1);
            cycleLeftNodeIndex = vertexInfos[cycleLeftNodeIndex]->parent->value;
            if (cycleLeftNodeIndex == cycleRightNodeIndex) break;
            VertexInfo *new = malloc(sizeof(VertexInfo));
            new->value = cycleRightNodeIndex;
            new->leftChild = NULL;
            
            if (head != NULL) {
                new->leftChild = head;
            }
            head = new;
            
            cycleRightNodeIndex = vertexInfos[cycleRightNodeIndex]->parent->value;
        }
        
        printf("%d ", cycleLeftNodeIndex + 1);
        
        while(head != NULL) {
            if (head->leftChild == NULL) { // i.e this is the last vertex in the cycle
                printf("%d)", head->value + 1);
            } else {
                printf("%d ", head->value + 1);
            }
            
            head = head->leftChild;
        }
        
    }
    
    printf("\n");
    
    return 0;
}

// This does a BFS traversal, but does not print the nodes while traversing,
// just checks if the graph has any cycle
void breadFirstTraversal(Queue *q, Vertex **vertices, bool *isVisited, VertexInfo **vertexInfos, int numberOfVertices, bool *independentSetData) {
    if (isEmpty(q)) {
        // Check if the all the vertices are visited or not.
        // If the graph is not connected, the queue might become empty prematurely
        // Hence we check for any unvisited vertex and start the BFS again from there
        int unvisitedVertex = -1;
        int anyVisited = -1;
        for (int i = 0; i < numberOfVertices; i++) {
            if(!isVisited[i]) {
                unvisitedVertex = i;
                anyVisited = i - 1;
                break;
            }
        }
        
        if (unvisitedVertex != -1) {
            // unvisited vertex found
            VertexInfo *p = malloc(sizeof(VertexInfo));
            p->value = unvisitedVertex;
            enqueue(q, p);
            isVisited[p->value] = true;
            independentSetData[p->value] = !independentSetData[anyVisited];
            vertexInfos[p->value] = p;
            breadFirstTraversal(q, vertices, isVisited, vertexInfos, numberOfVertices, independentSetData);
            return;
        } else {
            // There is no other unvisited vertex remaining
            // The BFS is complete . This is the base case of recursion
            return;
        }
        
        
    }
    
    VertexInfo *v = dequeue(q);
    
    // Add the unvisisted neighbours to the queue
    Vertex *ver = vertices[v->value];
    VertexInfo **leftChild = NULL;
    while(ver != NULL) {
        int value = ver->value;
        if (!isVisited[value]) {
            VertexInfo *c = malloc(sizeof(VertexInfo));
            c->value = value;
            c->parent = v;
            
            // Form a BFS Tree
            if (leftChild == NULL) {
                v->leftChild = c;
            } else {
                (*leftChild)->rightSibling = c;
            }
            leftChild = &c;
            
            enqueue(q, c);
            isVisited[value] = true;
            independentSetData[value] = !independentSetData[v->value];
            
            // Save the vertex info for later usage of printing the cycle
            vertexInfos[value] = c;
        } else {
            // If the vertex is already visited,
            // Check if belongs to the same independent set or not
            if (independentSetData[value] == independentSetData[v->value]) {
                // Two connected vertex found belonging to the same independent set, which means the graph is not bipartite
                isGraphBipartite = false;
                cycleLeftNodeIndex = value;
                cycleRightNodeIndex = v->value;
                return; // No need to traverse the graph any further
            }

        }
        ver = ver->next;
    }
    
    breadFirstTraversal(q, vertices, isVisited, vertexInfos, numberOfVertices, independentSetData);
}

// Debug helper function
void printGraphAdjacencyList(Vertex **vertices, int numberOfVertices) {
    for (int i = 0; i < numberOfVertices; i++) {
        Vertex *v = vertices[i];
        printf("%d - ", i+1);
        while(v != NULL) {
            printf("%d ", v->value + 1);
            v = v->next;
        }
        printf("\n");
    }
}
