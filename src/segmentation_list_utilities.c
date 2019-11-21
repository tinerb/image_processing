#include <stdio.h>
#include <stdlib.h>
#include "segmentation_list_utilities.h"


node *new_node(float data, float equivalent) {
	node *np = (node*) malloc(sizeof(node));
	np->data = data;
	np->equivalent = equivalent;
	np->prev = NULL;
	np->next = NULL;
	return np;
}

void insert_end(node **headp, node **tailp, node *new_np) {
	 if (*headp == NULL){
		 *headp = new_np;
		 *tailp = new_np;
	 }
	 else{
		 (*tailp)->next = new_np;
		 new_np ->prev = *tailp;
		 *tailp = new_np;
	 }
}

void display_forward(node *head) {
	node *np;
	for (np = head; np != NULL; np = np->next){
		printf("%.2f, %.2f\n", np->data, np->equivalent);
	}
}

int len(node *head) {
	int len = 0;
	node *np;
	for (np = head; np != NULL; np = np->next){
		len++;
	}

	return len;
}

float get_eq_value_at_index(node *head, float num){
	while ((head != NULL) && (head->data != num)) {
		head = head->next;

	}

	return head->equivalent;
}

// returns the node that we want to change
node* search(node *head, int num) {
	while ((head != NULL) && (head->data != num)) {
		head = head->next;
	}

	return head;
}

// when we find a pixel with set top and left values
// we set that pixel to its lowest value
void update_list(node *tail, int num, int equivalent) {
	while ((tail != NULL) && (tail->data != num)) {
		tail = tail->prev;
	}

	tail->equivalent = equivalent;
	return;
}

int is_unique(node *head, int num){
	int unique = 0;
	while (head != NULL) {
		if(head->equivalent == num){
			// number is not unique
			unique = 1;
		}
		head = head->next;
	}

	return unique;
}


// LOOPING THROUGH ONCE IS NOT ENOUGH; MAKE IT SO THAT WE KNOW FOR SURE EACH VALUE HAS BEEN CHANGED
int get_regions(node *temp){
	int regions = 1;
	node *temp_head=NULL, *temp_tail=NULL;
	node *n = new_node(temp->data, temp->equivalent);
	insert_end(&temp_head, &temp_tail, n); // insert to start the list which avoids errors when we search for unique
	temp = temp->next;
	while (temp != NULL) {
		if(is_unique(temp_head, temp->equivalent) == 0){
			node *n = new_node(temp->data, temp->equivalent);
			insert_end(&temp_head, &temp_tail, n);
//			printf("temp->data = %f -- temp->equivalent = %f\n",temp->data, temp->equivalent);
			regions++;
		}
		temp = temp->next;
	}

	return regions;
}

void clean(node **headp) {
	node** np = headp;
	node** temp;

	while ((*np) -> next != NULL){
		temp = np;
		free(temp);
		(*np) = (*np) -> next;
	}
}

segment_data *seg_node(float seg_value,  int min_x, int min_y, int max_x, int max_y){
	segment_data *np = (segment_data*) malloc(sizeof(segment_data));
	np->seg_value = seg_value;
	np->min_x = min_x;
	np->min_y = min_y;
	np->max_x = max_x;
	np->max_y = max_y;
	np->prev = NULL;
	np->next = NULL;
	return np;
}

void seg_insert_end(segment_data **headp, segment_data **tailp, segment_data *new_np) {
	 if (*headp == NULL){
		 *headp = new_np;
		 *tailp = new_np;
	 }
	 else{
		 (*tailp)->next = new_np;
		 new_np ->prev = *tailp;
		 *tailp = new_np;
	 }
}

void display_seg_forward(segment_data *head) {
	segment_data *np;
	for (np = head; np != NULL; np = np->next){
		printf("seg_value: %f, min_x: %d, min_y: %d, max_x: %d, max_y: %d w: %d h: %d\n", np->seg_value, np->min_x, np->min_y, np->max_x, np->max_y, np->max_x - np->min_x, np->max_y - np->min_y);
	}
}

segment_data* search_seg(segment_data *head, float num) {
	while ((head != NULL) && (head->seg_value != num)) {
		head = head->next;
	}

	return head;
}

int is_seg_unique(segment_data *head, float num){
	int unique = 0;
	while (head != NULL) {
		if(head->seg_value == num){
			// number is not unique
			unique = 1;
		}
		head = head->next;
	}

	return unique;
}

int seg_len(segment_data *head) {
	int len = 0;
	segment_data *np;
	for (np = head; np != NULL; np = np->next){
		len++;
	}

	return len;
}

segment_data* get_seg_value_at_index(segment_data *head, int seg_num){
	int i = 0;
	while ((head != NULL) && (i < seg_num)) {
		head = head->next;
		i++;

	}

	return head;
}

void seg_clean(segment_data **headp) {
	segment_data** np = headp;
	segment_data** temp;

	while ((*np) -> next != NULL){
		temp = np;
		free(temp);
		(*np) = (*np) -> next;
	}
}
