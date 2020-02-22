#ifndef _GQUEUE_
#define _GQUEUE_

typedef struct node {
    void *element;
    struct node *next;
    struct node *previous;
} Node;

typedef struct
{
  Node *head;
  Node *tail;
  int size;
} Queue;

void newQueue(Queue **q);
//void freeQueue(Queue *q);
void enqueue(Queue *q, void *element); // Add the element at the end
void *dequeue(Queue *q); // Remove the front element
void *peek(Queue *q);// Peek at the front element without removing it
_Bool isEmpty(Queue *q);

#endif // _GQUEUE_
