#include <assert.h>
#include <ctype.h>
#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "linkedlist.h"

#define ENCODE_MODE 0
#define DECODE_MODE 1


/******************************************************************************/
/*                          ---SHARED FUNCTIONS---                            */
/******************************************************************************/

/*Prints the correct format for command line arguments*/
void usage() {
    fprintf(stderr, "usage: phase2 [--encode|--decode] " \
                    "--infile <filename> --outfile <filename>\n"
    );
}

/*Prints the result of the encode or decode to the output file*/
void print_to_output(charval_t *head, char* outfile_name, char* block_size_string, int mode){
    char ch;
    charval_t *current = head;

    /*Chars for the "Magic Number" for head at beginning of the file*/
    int ch1 = '\xda'; int ch2 = '\xaa'; int ch3 = '\xaa'; int ch4 = '\xad'; /*Encode to ph2*/
    int ch5 = '\xab'; int ch6 = '\xba'; int ch7 = '\xbe'; int ch8 = '\xef'; /*Decode to ph1*/

    FILE *outfile = fopen(outfile_name,"a");

    /*Return an error if unable to open the output file*/
    if(outfile==NULL){
      fprintf(stderr, "Unable to open output file: %s\n",outfile_name);
      exit(1);
    }

    /*Write the correct "magic number to head depending on mode"*/
    if(mode == ENCODE_MODE){
        fputc(ch1, outfile); fputc(ch2, outfile); fputc(ch3, outfile); fputc(ch4, outfile);

        ch1 = block_size_string[0]; ch2 = block_size_string[1];
        ch3 = block_size_string[2]; ch4 = block_size_string[3];

        fputc(ch1, outfile); fputc(ch2, outfile); fputc(ch3, outfile); fputc(ch4, outfile);
    }
    else if(mode == DECODE_MODE){
      fputc(ch5, outfile); fputc(ch6, outfile); fputc(ch7, outfile); fputc(ch8, outfile);

      ch5 = block_size_string[0]; ch6 = block_size_string[1];
      ch7 = block_size_string[2]; ch8 = block_size_string[3];

      fputc(ch5, outfile); fputc(ch6, outfile); fputc(ch7, outfile); fputc(ch8, outfile);
    }

    /*Write the rest of the result after decode/encode to the output file*/
    while(current!= NULL){
      ch = current->c;
      fputc(ch, outfile);
      current = current->next;
    }

    fclose(outfile);
}

/*Read infile and return string of whole text*/
char* read_infile(char* infile_name, char* block_size_string, int mode){
  int num_char;
  char ch;

  char byte_string[4];

  int c;
  int nch = 0;
  int size = 50;
  char *buffer = malloc(size);

  if(buffer == NULL){
    fprintf(stderr, "Out of memory\n");
  }

  /*Open infile and check if it is valid*/
  FILE *infile = fopen(infile_name,"r");

  if(infile==NULL){
    fprintf(stderr, "Unable to open input file: %s\n",infile_name);
    exit(1);
  }

  num_char = 0;
  while( num_char < 4 ){
    ch = fgetc(infile);
    byte_string[num_char] = ch;
    num_char++;
  }

  if(mode == ENCODE_MODE){
    /*Check if the magic number is the correct format for ph1*/
      if(byte_string[0] != '\xab' || byte_string[1] != '\xba' || byte_string[2] != '\xbe' || byte_string[3] != '\xef'){
        printf("Error: Header file does not match ph1\n");
        exit(1);
      }
  }
  else if(mode == DECODE_MODE){
    /*Check if the magic number is the correct format for ph2*/
      if(byte_string[0] != '\xda' || byte_string[1] != '\xaa' || byte_string[2] != '\xaa' || byte_string[3] != '\xad'){
        printf("Error: Header file does not match ph1\n");
        exit(1);
      }
  }

  /*Get the blocksize of file from next 4 bytes*/
  num_char = 0;
  while( num_char < 4 ){
    ch = fgetc(infile);
    block_size_string[num_char] = ch;
    num_char++;
  }

  /*Borrowed from an infile_string at http://bit.ly/2uqh7QA*/
  while((c = fgetc(infile)) != EOF){
    if(nch >= size-1){
      size = size*10;
      buffer = realloc(buffer, size);
      if(buffer == NULL){
        fprintf(stderr, "Out of memory\n");
        exit(1);
      }
    }
    buffer[nch++] = c;
  }
  fclose(infile);
  return buffer;
}

/******************************************************************************/
/*                          ---ENCODE FUNCTIONS---                            */
/******************************************************************************/

charval_t* move_to_front(char* infile_string){
  int true_or_false;
  int position = 0;

  int index = 0;

  /*Create a new list*/
  charval_t *mtf_list = NULL;
  charval_t *result_list = NULL;
  charval_t *tempnode = NULL;

  int temp_index = 0;

  /*Loop through whole string*/
  int i;
  for(i=0;i<strlen(infile_string);i++){
    /*Check if each item is already in the mtf_list*/
    true_or_false = check_if_in_list(mtf_list, infile_string[i]);
    if(true_or_false == 0){ /*If not in the mtf_list then add to it*/
        index++;
        tempnode = new_charval(infile_string[i]);
        /*Add node to the end of the list*/
        mtf_list = add_end(mtf_list, tempnode);
        temp_index = index+128;
        /*Add index and char to the result*/
        tempnode = new_charval(temp_index);
        result_list = add_end(result_list, tempnode);
        tempnode = new_charval(infile_string[i]);
        result_list = add_end(result_list, tempnode);
    }
    else if(true_or_false == 1){ /*If already in the list*/
        position = find_position(mtf_list, infile_string[i]);
        tempnode = new_charval(position+128);
        result_list = add_end(result_list, tempnode);

        mtf_list = delitem(mtf_list, infile_string[i]);
        tempnode = new_charval(infile_string[i]);
        mtf_list = add_front(mtf_list, tempnode);
    }
    temp_index = 0;
  }
  free(mtf_list);

  return result_list;
}

/*Convert sequences of ones to zero followed by number of ones*/
charval_t *run_length_encode(charval_t *mtf_result_list){
    check_if_seq_of_one(mtf_result_list);
    charval_t *run_length_result = mtf_result_list;
    return run_length_result;
}

/******************************************************************************/
/*                          ---DECODE FUNCTIONS---                            */
/******************************************************************************/

charval_t *mtf_decode(charval_t* run_length_list){

    charval_t *current = run_length_list;
    /*The list that will keep track of elements which are MTF'd*/
    charval_t *list = NULL;
    /*The result of the MTF transform*/
    charval_t *result = NULL;

    charval_t *temp_node = NULL;
    charval_t *temp_node2 = NULL;
    charval_t *temp_node3 = NULL;

    char returned;

    int temp_int = 0;

    while(current->next!=NULL){
      /*If current is a number and the next item is a character*/
      if(current->c >= 128 && current->next->c < 128){
          /*Add to encoding list*/
          temp_node = new_charval(current->next->c);
          list = add_end(list, temp_node);
          /*Add to result list*/
          temp_node3 = new_charval(current->next->c);
          result = add_end(result, temp_node3);
      }
      /*If current is a number*/
      else if(current->c >= 128){
          /*Find the node in MTF list at that position*/
          temp_int = current->c-128;
          returned = return_node_at_position(list, temp_int);
          temp_node2 = new_charval(returned);
          /*Remove that node from the MTF list*/
          list = remove_position(list, temp_int);
          list = add_front(list, temp_node2);
          /*Add that node to the end of the result*/
          temp_node3 = new_charval(returned);
          result = add_end(result, temp_node3);
      }
      current = current->next;
    }

    /*Fencepost for last item*/
    temp_int = current->c-128;
    returned = return_node_at_position(list, temp_int);
    temp_node2 = new_charval(returned);
    list = remove_position(list, temp_int);
    list = add_front(list, temp_node2);
    if(returned != -1){
      temp_node3 = new_charval(returned);
      result = add_end(result, temp_node3);
    }

    return result;
}

/*Performs RLE by converting input string into a list and converting sequences of ones*/
charval_t *run_length_decode(char* infile_string){

    charval_t *run_length_list = NULL;
    charval_t *tempnode = NULL;

    unsigned char ch;

    /*Convert the input string into a list*/
    int i;
    for(i=0;i<strlen(infile_string);i++){
      ch = infile_string[i];
      tempnode = new_charval(ch);
      run_length_list = add_end(run_length_list, tempnode);
    }

    /*Go through list and find zero and value of node afterwards
      insert string of ones corresponding to second node's value*/
    convert_zero_to_ones(run_length_list);

    return run_length_list;
}

/******************************************************************************/
/*                          ---MAIN FUNCTION---                               */
/******************************************************************************/
int main(int argc, char *argv[]) {
    int c;
    char *infile_name = NULL;
    char *outfile_name = NULL;
    int mode;
    int encode_flag = 0;
    int decode_flag = 0;

    char *infile_string = "";

    /*
     * Based on http://bit.ly/2tHBpo1
     */
    for (;;) {
        static struct option long_options[] = {
            {"encode",     no_argument,       0, 'e'},
            {"decode",     no_argument,       0, 'd'},
            {"infile",     required_argument, 0, 'i'},
            {"outfile",    required_argument, 0, 'o'},
            {0, 0, 0, 0}
        };
        int option_index = 0;

        c = getopt_long (argc, argv, "efi:o:",
            long_options, &option_index);

        if (c == -1) {
            break;
        }

        switch (c) {
        case 'i':
            infile_name = optarg;
            break;
        case 'o':
            outfile_name = optarg;
            break;
        case 'e':
            encode_flag = 1;
            break;
        case 'd':
            decode_flag = 1;
            break;
        default:
            fprintf(stderr, "shouldn't be here...");
            assert(0);
        }
    }

    if (encode_flag == 0 && decode_flag == 0) {
        usage();
        exit(1);
    } else if (encode_flag == 1 && decode_flag == 1) {
        fprintf(stderr, "usage: choose one of --decode or --encode\n");
        exit(1);
    } else if (encode_flag == 1) {
        mode = ENCODE_MODE;
    } else if (decode_flag == 1) {
        mode = DECODE_MODE;
    } else {
        fprintf(stderr, "shouldn't be here...\n");
        assert(0);
    }

    if (infile_name == NULL) {
        usage();
        exit(1);
    }

    if (outfile_name == NULL) {
        usage();
        fprintf(stderr, "%s: need --outfile <filename>\n", argv[0]);
        exit(1);
    }

/*******************************************/
/*           ---Call Functions---          */
/*******************************************/

    char block_size_string[4];
    charval_t *mtf_result_list;
    charval_t *run_length_result;
    charval_t *run_length_list;
    charval_t *result;

    if (mode == ENCODE_MODE){
      /*Do encode stuff*/
      infile_string = read_infile(infile_name, block_size_string, mode);
      mtf_result_list  = move_to_front(infile_string);
      run_length_result = run_length_encode(mtf_result_list);
      print_to_output(run_length_result, outfile_name, block_size_string, mode);

      free(infile_string);

      freeall(mtf_result_list);
    }
    else if (mode == DECODE_MODE){
      /*Do decode stuff*/
      infile_string = read_infile(infile_name, block_size_string, mode);
      run_length_list = run_length_decode(infile_string);
      result = mtf_decode(run_length_list);
      print_to_output(result, outfile_name, block_size_string, mode);

      free(infile_string);

      freeall(run_length_list);
      freeall(result);
    }

#ifdef DEBUG_COMMAND_LINE
    printf("Outfile name is: %s\n", outfile_name);
    printf("Infile name is : %s\n", infile_name);
#endif
    if (mode == ENCODE_MODE) {
#ifdef DEBUG
        printf("DEBUG: Encoding %s.\n", infile_name);
#endif
    } else if (mode == DECODE_MODE) {
#ifdef DEBUG
        printf("DEBUG: Decoding %s.\n", infile_name);
#endif
    } else {
        /* Shouldn't be here!! */
        assert(0);
    }

    return 0;
}
