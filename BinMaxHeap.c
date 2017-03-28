#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "queue.h"



/*if a node's value is greater than its father, it needs to be bubbled up*/
void ShiftUp (heap_node*);


/*a local function that swaps keys and values for a node and its father*/
heap_node* node_swap (heap_node* node) {

	heap_node tmp;

	tmp.key = malloc((strlen(node->key) + 1) * sizeof(char));
	strcpy(tmp.key, node->key);
	free (node->key);
	tmp.value = node->value;

	node->key = malloc((strlen(node->father->key)+1)*sizeof(char));
	strcpy(node->key, node->father->key);
	free(node->father->key);
	node->value = node->father->value;
	
	node->father->key = malloc((strlen(tmp.key)+1)*sizeof(char));
	strcpy(node->father->key, tmp.key);
	node->father->value = tmp.value;

	free(tmp.key);

	return node->father;

}


/*a local function able to free a node recursively*/
void node_destroy (heap_node* node) {

	if (!node)
		return;
	else {

		free(node->key);
		node->father = NULL;

		node_destroy(node->left_child);
		node_destroy(node->right_child);

		free (node);

	}

}


/**********************************************************************************************************************************/


max_heap* HP_create() {

	max_heap* heap;

	if (!(heap = (max_heap*) malloc (sizeof (max_heap)))) {

		perror ("\tError: ");
		return NULL;

	}

	heap->total_heap_amount = 0;
	heap->root = NULL;

	return heap;

}


void HP_search (heap_node** node, char* value, int* found, char* newChild, heap_node** arg, Queue* q) {

	qnode newNode;
	static int round = 0;

	if (!strcmp((*node)->key, value)) {

		printf("jjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjjj\n");
		*found = 1;
		*arg = *node;
		return;

	}
	else {

		if (!(*node)->left_child) {

			*newChild = 'L';
			*arg = *node;
			return;

		}
		queue_push(q, (*node)->left_child);

		if (!(*node)->right_child) {

			*newChild = 'R';
			*arg = *node;
			return;

		}
		queue_push(q, (*node)->right_child);

		while (1)
		{
			if (q->first != q->last)
				newNode = queue_pop(q);
			else
				break;
			if ((newNode->left_child==NULL)|| (newNode->right_child==NULL))
				break;
		}

		HP_search(&newNode, value, found, newChild, arg, q);
			
	}

}


int HP_insert (max_heap* heap, char* key, float value, Queue* q) {

	int found;
	heap_node*  inserted ;
	heap_node** arg;
	arg = malloc(sizeof(heap_node*));
	char newChild = ' ';
	


	if (!heap->root) {

		if (!(heap->root = (heap_node*) malloc (sizeof(heap_node)))) {

			perror("\tError: ");
			return -1;

		}

		if (!(heap->root->key = (char*) malloc ((strlen(key) + 1) * sizeof(char)))) {

			perror("\tError: ");
			return -1;

		}

		strcpy(heap->root->key, key);
		heap->root->value = value;
		heap->root->left_child = heap->root->right_child = heap->root->father = NULL;
		heap->total_heap_amount += value;

		queue_push(q, heap->root);

		return 0;

	}

	found = 0;
	
	HP_search (&heap->root, key, &found, &newChild, arg, q);
	
	if (found) {

		printf("pro %f\n", (*arg)->value);
		(*arg)->value += value;
		printf("meta %f\n", (*arg)->value);
		heap->total_heap_amount += value;

		inserted = (*arg);

	}
	else {
		if (newChild == 'L') {

			if (!((*arg)->left_child = (heap_node*) malloc (sizeof(heap_node)))) {

				perror("\tError: ");
				return -1;

			}

			if (!(((*arg))->left_child->key = (char*) malloc ((strlen(key) + 1) * sizeof(char)))) {

				perror("\tError: ");
				return -1;

			}
			
			strcpy ((*arg)->left_child->key, key);
			(*arg)->left_child->value = value;
			
			(*arg)->left_child->left_child = NULL;
			(*arg)->left_child->right_child = NULL;
			(*arg)->left_child->father = (*arg);

			inserted = (*arg)->left_child;
			printf(" L:  father %s --  o %s\n",inserted->father->key, inserted->key );

		}
		else {

			if (!((*arg)->right_child = (heap_node*) malloc (sizeof(heap_node)))) {

				perror("\tError: ");
				return -1;

			}

			if (!((*arg)->right_child->key = (char*) malloc ((strlen(key) + 1) * sizeof(char)))) {

				perror("\tError: ");
				return -1;

			}

			strcpy((*arg)->right_child->key, key);
			(*arg)->right_child->value = value;
			
			(*arg)->right_child->left_child = NULL;
			(*arg)->right_child->right_child = NULL;
			(*arg)->right_child->father = (*arg);

			inserted = (*arg)->right_child;
			printf("R:::  father %s --- o %s\n",inserted->father->key, inserted->key );

		}

		heap->total_heap_amount += value;

	}

	if (inserted->value > inserted->father->value) 
	  	ShiftUp(inserted);
	
	free(arg);
}


void ShiftUp (heap_node* node) {

	heap_node* swapped;

	if (!node->father)
		return;
	else {

		swapped = node_swap(node);
		if (swapped->father) {

			if (swapped->value > swapped->father->value)
				ShiftUp (swapped);

		}

	}

}


int HP_destroy (max_heap* heap) {

	node_destroy(heap->root);
	free (heap);

}


void HP_top (max_heap* heap, char* buff) {

	int percentage = atoi (buff);
	heap_node* traverse;
	float goal, compare_value;
	int round=0;

	goal = (float) percentage / 100;
	goal *= heap->total_heap_amount;
	compare_value = 0;

	if (!heap->root)
		printf("No calls found.\n");
	else {

		traverse = heap->root;
		printf("Top %d%% of company's income is due to :\n",percentage );

		while (traverse) {

			round++;
			compare_value += traverse->value;

			printf("\t  %s calls \n",traverse->key );
			if (compare_value >= goal)
				return;

			
			if (round == 1)
				traverse = traverse->left_child;
			else if (round ==2 )
				traverse = traverse->father->right_child;
			

		}

	}

}