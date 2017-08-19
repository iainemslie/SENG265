/*
 * linkedlist.c
 *
 * Based on the implementation approach described in "The Practice
 * of Programming" by Kernighan and Pike (Addison-Wesley, 1999).
 */

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include "linkedlist.h"

#define ENCODE_MODE 0
#define DECODE_MODE 1

void *emalloc(size_t n) {
    void *p;

    p = malloc(n);
    if (p == NULL) {
        fprintf(stderr, "malloc of %zu bytes failed", n);
        exit(1);
    }

    return p;
}


charval_t *new_charval(unsigned char c) {
    charval_t *temp;

    temp = (charval_t *) emalloc(sizeof(charval_t));
    temp->c = c;
    temp->next = NULL;

    return temp;
}


charval_t *add_front(charval_t *list, charval_t *cv) {
    cv->next = list;
    return cv;
}


charval_t *add_end(charval_t *list, charval_t *cv) {
    charval_t *curr;

    if (list == NULL) {
        cv->next = NULL;
        return cv;
    }

    for (curr = list; curr->next != NULL; curr = curr->next);
    curr->next = cv;
    cv->next = NULL;
    return list;
}


charval_t *peek_front(charval_t *list) {
    return list;
}


charval_t *remove_front(charval_t *list) {
    if (list == NULL) {
        return NULL;
    }

    return list->next;
}


void free_charval(charval_t *cv) {
    assert(cv != NULL);
    free(cv);
}


void apply(charval_t *list,
           void (*fn)(charval_t *list, void *),
           void *arg)
{
    for ( ; list != NULL; list = list->next) {
        (*fn)(list, arg);
    }
}

/*Checks to see if a node is in the list based on a search character*/
int check_if_in_list(charval_t *head, char search_char){
	/*This function looks for a node with a given name, and returns a pointer to it if it is found.*/

	/*We need to cycle through the nodes, looking at the name of each node.*/
	charval_t *current = head;

	while(current != NULL){
		/* If the name matches, we return the current node. */
		if(search_char == current->c){ return 1; }
		/* Otherwise, we move on to the next node. This will continue until the current node is NULL (we're off the end of the list).*/
		current = current->next;
	}

	/* If we don't find anything, we return NULL.*/
	return 0;
}

/*Find the position number in the list of a node based on a search character*/
int find_position(charval_t *head, char search_char){
  charval_t *current = head;
  int position = 0;

  while(current != NULL){
    position++;
		/* If the name matches, we return the current node. */
		if(search_char == current->c){ return position; }
		/* Otherwise, we move on to the next node. This will continue until the current node is NULL (we're off the end of the list).*/
		current = current->next;
	}

  return position;
}

/*Delete a single node from the list based on the parameter character*/
charval_t *delitem(charval_t *head, char search_char){
  charval_t *curr, *prev;

  prev = NULL;
  for(curr = head; curr != NULL; curr = curr-> next){
      if(search_char == curr->c){
        if(prev == NULL){ /*If pointed at head*/
          head = curr->next;
        } else{
          prev->next = curr->next;
        }
        free(curr);
        return head;
      }
      prev = curr;
  }
  fprintf(stderr, "delitem: %c not in list", search_char);
  exit(-1);
}

/*Count the number of nodes in the list*/
int size_of_list(charval_t *head){
  /* In this function, we set up a temporary pointer to keep track of the current node.*/
	charval_t *current = head;

  int num_items = 0;

	/* We then use this pointer to cycle through the list nodes, and count the # of nodes*/
	while(current != NULL){
    num_items++;
		current = current->next;
	}
  return num_items;
}

/*Checks to find is there is a sequence of ones in the list*/
void check_if_seq_of_one(charval_t *head){
  /*Value of one is 129 because 128 has been added*/
  unsigned char one = 129;

  int ONES_FLAG = 0;

  charval_t *current = head;
  charval_t *beginning_of_ones; /*Keeps track of where the zero is*/
  charval_t *previous = NULL;

  while(current->next->next != NULL){
    if(current->c == one && current->next->c == one && current->next->next->c == one){
      beginning_of_ones = current;
      ONES_FLAG = 1;
      delete_series_of_ones(ONES_FLAG, current, beginning_of_ones, previous);
    }
    previous = current;
    current = current->next;
  }
}

/*Removes a series of ones from the list*/
void delete_series_of_ones(int ONES_FLAG, charval_t *current, charval_t *beginning_of_ones, charval_t *previous){
  unsigned char one = 129;
  int count_of_ones = 0;

  if(ONES_FLAG == 1){
    while(current->c == one){
      count_of_ones++;
      current = current->next;
    }

    unsigned char char_after_ones = current->c;

    current = beginning_of_ones;

    /*Get correct values for zero and number of ones*/
    current->c = 128;
    current->next->c = count_of_ones+128;
    previous = beginning_of_ones->next;
    current = beginning_of_ones->next->next;

    count_of_ones = 0;

    /*Delete all the nodes with value of one*/
    while(current->c != char_after_ones){
      previous->next = current->next;
      free(current);
      current = previous->next;
    }
  }
}

/*Go through list and find zero and value of node afterwards
  insert string of ones corresponding to second node's value*/
void convert_zero_to_ones(charval_t *head){

  int num_of_ones = 0;

  /* In this function, we set up a temporary pointer to keep track of the current node.*/
  charval_t *current = head;
  charval_t *previous = NULL;

  /* We then use this pointer to cycle through the list nodes, printing the value of each one.*/
  while(current != NULL){
    if(current->c == 128){
      previous = current;
      current = current->next;
      num_of_ones = current->c-128;
      insert_ones(current, previous, num_of_ones);
    }

    current = current->next;
  }
}

charval_t *remove_position(charval_t *head, int position){

    int index = 1;

    if(head == NULL){
      printf("Error: List is null!");
      exit(-1);
    }

    charval_t *curr, *prev;

    prev = NULL;
    for(curr = head; curr != NULL; curr = curr-> next){
        if(index == position){
          if(prev == NULL){ /*If pointed at head*/
            head = curr->next;
          } else{
            prev->next = curr->next;
          }
          free(curr);
          return head;
        }
        prev = curr;
        index++;
    }
  return NULL;
}

char return_node_at_position(charval_t *head, int position){
  /*This function prints a linked list out.*/
  int index = 1;
  /* In this function, we set up a temporary pointer to keep track of the current node.*/
  charval_t *current = head;

  /* We then use this pointer to cycle through the list nodes, the node with matching position is returned*/
  while(current != NULL){
    if(index == position){
      return current->c;
    }
    current = current->next;
    index++;
  }
  return -1;
}

void insert_ones(charval_t *current, charval_t *previous, int num_of_ones){

  /*Want to change value of previous and current nodes to be ones*/
  current->c = 129;
  previous->c = 129;

  int i;
  for(i=0;i<num_of_ones-2;i++){
    charval_t *temp_node = new_charval(129);
    previous->next = temp_node;
    temp_node->next = current;
    previous = previous->next;
  }
}

void freeall(charval_t *head){

    charval_t *current = head;
    charval_t *previous = NULL;

    while(current != NULL){
      previous = current;
      current = current->next;
      free(previous);
    }
}

	/*This function prints a linked list out.*/
void print_list(charval_t *head){

	/* In this function, we set up a temporary pointer to keep track of the current node.*/
	charval_t *current = head;

	/* We then use this pointer to cycle through the list nodes, printing the value of each one.*/
	while(current != NULL){
		printf("Character: '%c' \n", current->c);

		current = current->next;
	}

	printf("\n");
}
