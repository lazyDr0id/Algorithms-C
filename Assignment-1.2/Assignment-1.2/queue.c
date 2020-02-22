#include <stdio.h>
#include <stdlib.h>
#include "queue.h"

void printQueue(Queue *q);

void newQueue(Queue **q) {
    *q = malloc(sizeof(Queue));
    (*q)->size = 0;
    // Head and Tail pointer will point to NULL intially
}

void enqueue(Queue *q, void *element) {
    
    // Create a new node, and save the element in it
    Node *new = malloc(sizeof(Node));
    new->element = element;
    new->next = NULL;
    new->previous = NULL;
    
    if (q->head == NULL) {
        // Queue is empty
        q->head = new;
        q->tail = new;
    } else {
        // Queue isn't empty
        q->tail->next = new;
        new->previous = q->tail;
        q->tail = new;
    }
    
    (q->size)++;
}

void *dequeue(Queue *q) {
    if (q->head == NULL) // quque is empty
        return NULL;
    
    // Store the old head which is to be dequeued
    Node *oldHead = q->head;
    
    // Point to the new head
    q->head = oldHead->next;
    
    // If the queue only has 1 element, head will point to NULL at this place
    if (q->head != NULL)
        q->head->previous = NULL;
    else
        q->tail = NULL;
    
    
    // Retrieve the element from the node
    void *element = oldHead->element;
    
    free(oldHead);
    (q->size)--;
    
    return element;
}

void *peek(Queue *q) {
    return q->head;
}

_Bool isEmpty(Queue *q) {
    return (q->size) == 0;
}

void printQueue(Queue *q) {
    Node *head = q->head; int *el;
    while (head != NULL) {
        el = (int *)head->element;
        printf("(%p, %d)-", head, *el);
        head = head->next;
    }
    printf("\n");
}
