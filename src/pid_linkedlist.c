#include <stdbool.h>
#include "linked_list.h"

typedef struct specialized_linkedlist {
	LinkedList* kp;
	LinkedList* ki;
	LinkedList* kd;
} PIDLinkedList;

typedef struct pid_node {
	Node* kp_node;
	Node* ki_node;
	Node* kd_node;
} PIDNode;

bool PIDLL_hasNext(PIDLinkedList list) {
	return ;
}

bool PIDLL_isEmpty(PIDLinkedList* list) {
	return

PIDNode* PIDLL_getNext(PIDLinkedList* pid_data) {
    PIDNode* node = malloc(sizeof(PIDNode));
	return
}

void PIDLL_leftLLPop(PIDLinkedList* pid_data) {
	leftLLpop(pid_data->kp);
	leftLLpop(pid_data->ki);
	leftLLpop(pid_data->kd);
}

