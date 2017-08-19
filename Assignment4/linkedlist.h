#ifndef _LINKEDLIST_H_
#define _LINKEDLIST_H_

typedef struct charval charval_t;
struct charval {
    unsigned char c;
    int           val;
    charval_t *next;
};

charval_t *new_charval(unsigned char c);
charval_t *add_front(charval_t *, charval_t *);
charval_t *add_end(charval_t *, charval_t *);
charval_t *peek_front(charval_t *);
charval_t *remove_front(charval_t *);
void free_charval(charval_t *);
void       apply(charval_t *, void(*fn)(charval_t *, void *), void *arg);
int check_if_in_list(charval_t *head, char search_char);
int find_position(charval_t *head, char search_char);
charval_t *delitem(charval_t *head, char search_char);
int size_of_list(charval_t *head);
void check_if_seq_of_one(charval_t *head);
void delete_series_of_ones(int ONES_FLAG, charval_t *current, charval_t *beginning_of_ones, charval_t *previous);
void convert_zero_to_ones(charval_t *head);
charval_t *remove_position(charval_t *head, int position);
char return_node_at_position(charval_t *head, int position);
void insert_ones(charval_t *current, charval_t *previous, int num_of_ones);
void freeall(charval_t *listp);
void print_list(charval_t *head);

#endif
