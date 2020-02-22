#include <stdio.h>
#include <stdlib.h>
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
void breadFirstTraversal(Queue *q, Vertex **vertices,char *isVisited, VertexInfo **vertexInfos, int numberOfVertices);

_Bool isCycleFound = 0;
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
    startingVertex--; // The index starts from 0. The inputfile has it from 1. So adjusting by 1.
    
    // Form a adjacency list of the graph
    // (All the index are starting from 0, adjusted by 1 from inputfile)
    // (This is just done in code. The output will again be adjusted to match the indexing in inputfile.)
    Vertex **vertices = malloc(sizeof(Vertex *) * numberOfVertices);
    char *buffer = NULL; size_t bufferSize = 0;
    int n, bytesread; Vertex **v;
    for (int i = 0; i < numberOfVertices; i++) {
        getline(&buffer, &bufferSize, inputFile);
        char *temp = buffer;
        v = vertices + i;
        while (sscanf(buffer, "%d%n", &n, &bytesread) > 0) {
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
    
    char *isVisited = calloc(sizeof(char) , numberOfVertices);
    isVisited[p->value] = 1;
    
    VertexInfo **vertexInfos = malloc(sizeof(VertexInfo *) * numberOfVertices);
    vertexInfos[p->value] = p;
    
    // Do a BFS Traversal to know if the graph has any cycle, and collect relevant info
    breadFirstTraversal(q, vertices, isVisited, vertexInfos, numberOfVertices);
    
    // Print the cycle
    if (isCycleFound) {
        VertexInfo *head = NULL;
        printf("Yes\n");
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
            printf("%d ", head->value + 1);
            head = head->leftChild;
        }
    } else {
        printf("No\n");
    }
    
    return 0;
}

// This does a BFS traversal, but does not print the nodes while traversing,
// just checks if the graph has any cycle
void breadFirstTraversal(Queue *q, Vertex **vertices, char *isVisited, VertexInfo **vertexInfos, int numberOfVertices) {
    if (isEmpty(q)) {
        // If cycle is not found.
        // Check if the all the vertices are visited or not.
        // If the graph is not connected, the queue might become empty prematurely
        // Hence we check for any unvisited vertex and start the BFS again from there
        if (isCycleFound) {
            // We have found one cycle, we don't need to traverse the graph any further
            // even if it is disconnected
            // Early exit
            return;
        } else {
            // check for any unvisited vertex and start the BFS again from there
            int unvisitedVertex = -1;
            for (int i = 0; i < numberOfVertices; i++) {
                if(!isVisited[i]) {
                    unvisitedVertex = i; 
                    break;
                }
            }
            
            if (unvisitedVertex != -1) {
                // unvisited vertex found
                VertexInfo *p = malloc(sizeof(VertexInfo));
                p->value = unvisitedVertex;
                enqueue(q, p);
                isVisited[p->value] = 1;
                vertexInfos[p->value] = p;
                breadFirstTraversal(q, vertices, isVisited, vertexInfos, numberOfVertices);
                return;
            } else {
                // There is no other unvisited vertex remaining
                // The BFS is complete . This is the base case of recursion
                return;
            }
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
            isVisited[value] = 1;
            
            // Save the vertex info for later usage of printing the cycle
            vertexInfos[value] = c;
        } else {
            // Check if the already visited neighbour is parent of the current node in BFS tree
            if (v->parent->value != value) {
                // IF not, that means there is a cycle
                // Save the relevant info
                if (!isCycleFound) {
                    isCycleFound = 1;
                    cycleLeftNodeIndex = value;
                    cycleRightNodeIndex = v->value;
                }
            }
        }
        ver = ver->next;
    }
    
    breadFirstTraversal(q, vertices, isVisited, vertexInfos, numberOfVertices);
}

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
