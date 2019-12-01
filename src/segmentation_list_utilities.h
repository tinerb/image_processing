#ifndef SEGMENTATION_LIST_UTILITIES_H_
#define SEGMENTATION_LIST_UTILITIES_H_

typedef struct node {
	float data;
	float equivalent;
	struct node *next;
	struct node *prev;
} node;

typedef struct segment_data {
	float seg_value;
	int min_x;
	int min_y;
	int max_x;
	int max_y;
	struct segment_data *next;
	struct segment_data *prev;
} segment_data;


node *new_node(float data, float equivalent);
void insert_end(node **headp, node **tailp, node *new_np);
void display_forward(node *head);
void display_backward(node *tail);
void clean(node **headp);
node *search(node *tail, int num);
void update_list(node *tail, int num, int equivalent);
float get_eq_value_at_index(node *head, float num);
int is_unique(node *head, int num);
int get_regions(node *head);
int len(node *head);


segment_data *seg_node(float seg_value,  int min_x, int min_y, int max_x, int max_y);
void seg_clean(segment_data **headp);
void seg_insert_end(segment_data **headp, segment_data **tailp, segment_data *new_np);
void display_seg_forward(segment_data *head);
segment_data* search_seg(segment_data *head, float num);
int seg_len(segment_data *head);
segment_data* get_seg_value_at_index(segment_data *head, int seg_num);
int is_seg_unique(segment_data *head, float num);

#endif /* SEGMENTATION_LIST_UTILITIES_H_ */
