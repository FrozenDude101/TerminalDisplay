#include <stdlib.h>


typedef struct Queue Queue;
typedef struct Node Node;


struct Queue {

    Node* head;
    Node* tail;

};
struct Node {

    Node* next;

    int x;
    int y;

};


Queue* newQueue();
void freeQueue();

Node* newNode();
void freeNode();

void enqueue(Queue* queue, int x, int y);
void dequeue(Queue* queue);
void push(Queue* queue, int x, int y);

int queueLength(Queue* queue);

int* getQueueData(Queue* queue);
void freeQueueData(int* queueData);


Queue* newQueue() {

    Queue* queue = malloc(sizeof(Queue));
    queue -> head = NULL;
    queue -> tail = NULL;

    return queue;

}
void freeQueue(Queue* queue) {

    Node* node = queue -> head;
    Node* next;
    while (node != NULL) {
        next = node -> next;
        freeNode(node);
        node = next;
    }

    free(queue);

}

Node* newNode(int x, int y) {

    Node* node = malloc(sizeof(Node));
    
    node -> x = x;
    node -> y = y;
    node -> next = NULL;

    return node;

}
void freeNode(Node* node) {

    free(node);

}

void enqueue(Queue* queue, int x, int y) {

    if (queue -> head == NULL) {
        queue -> head = newNode(x, y);
        queue -> tail = queue -> head;
        return;
    }

    queue -> tail -> next = newNode(x, y);
    queue -> tail = queue -> tail -> next;

}
void dequeue(Queue* queue) {

    Node* node = queue -> head;
    queue -> head = node -> next;
    freeNode(node);

}
void push(Queue* queue, int x, int y) {

    if (queue -> head == NULL) {
        queue -> head = newNode(x, y);
        queue -> tail = queue -> head;
        return;
    }

    Node* node = newNode(x, y);
    node -> next = queue -> head;
    queue -> head = node;

}

int queueLength(Queue* queue) {

    if (queue -> head == NULL) return 0;

    Node* node = queue -> head;
    int length = 1;
    while (node -> next != NULL) {
        node = node -> next;
        length += 1;
    }

    return length;

}

int* getQueueData(Queue* queue) {

    if (queue -> head == NULL) return NULL;

    int length = queueLength(queue);
    int* queueData = malloc(2 * length * sizeof *queueData);

    Node* node = queue -> head;
    for (int i = 0; i < length; i++) {
        queueData[2 * i] = node -> x;
        queueData[2 * i + 1] = node -> y;
        node = node -> next;
    }

    return queueData;

    
}
void freeQueueData(int* queueData) {

    free(queueData);

}