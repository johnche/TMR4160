#include <stdlib.h>
#include <stdio.h>
#include "linked_list.h"

static Node* createNode(double data) {
    Node* new_node = malloc(sizeof(Node));
    new_node->data = data;
    new_node->next = NULL;
    return new_node;
}

LinkedList* newLinkedList() {
    LinkedList* new_list = malloc(sizeof(LinkedList));
    new_list->root = NULL;
    new_list->tail = NULL;
    return new_list;
}

void printLL(LinkedList* list) {
    if (list->root == NULL) {
        printf("[]\n");
        return;
    }
    printf("[");
    Node* current_node = list->root;
    while (current_node->next != NULL) {
        printf("%f, ", current_node->data);
        current_node = current_node->next;
    }
    printf("%f", current_node->data);
    printf("]\n");
}

void addLLNode(LinkedList* list, double data) {
    if (list->root == NULL) {
        list->root = createNode(data);
        list->tail = list->root;
    }
    else {
        list->tail->next = createNode(data);
        list->tail = list->tail->next;
    }
}

int leftLLPop(LinkedList* list) {
    if (list->root == NULL) {
        return 1;
    }
    if (list->root->next == NULL) {
        free(list->root);
        list->root = NULL;
        return 0;
    }
    Node* old_root = list->root;
    Node* new_root = list->root->next;
    list->root = new_root;
    free(old_root);
    return 0;
}

void disposeLL(LinkedList* list) {
    while (leftLLPop(list) != 1) {}
    free(list);
}

