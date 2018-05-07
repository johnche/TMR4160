#pragma once

typedef struct node {
    double data;
    struct node* next;
} Node;

typedef struct context {
    Node* root;
    Node* tail;
} LinkedList;

LinkedList* newLinkedList();
void printLL(LinkedList* list);
void addLLNode(LinkedList* list, double data);
int leftLLPop(LinkedList* list);
void disposeLL(LinkedList* list);

