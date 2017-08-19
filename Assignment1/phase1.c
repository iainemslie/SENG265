#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define BLOCK_SIZE 21
#define MAX_FILENAME_LEN 50

/**Forward functions**/
void forward_transform(char*, char*);
char* process_block(char*, char*, int, FILE*);
void rotate_sort_write(char*, int, char*, int);
void open_outfile(char*);
void write_to_outfile(char*, char*, int, int);

/***Backward functions***/
void backward_transform(char*, char*);
char* process_ph1(char*, char*, int, FILE*, int);
void rotate_sort_ph1(char*, int, char*, int);
void write_to_textfile(char*, char*, int, int);

/***Used by both***/
int size_of_file(char*);
int compare_strings();


int main(int argc, char *argv[]){
  int i;
  char infile_name[MAX_FILENAME_LEN]; /*Name of input file to process*/
  char outfile_name[MAX_FILENAME_LEN]; /*Name of file to output to*/
  /*If no arguments are provided then exit*/
  if(argc<2)
  {
    printf("No arguments provided\n");
    exit(1);
  }
  /*Cycle through the args and look for correct terms and run functions*/
  for (i = 1; i < argc; i++) {
      if (strcmp(argv[i], "--infile") == 0) {
          strncpy(infile_name, argv[i+1], MAX_FILENAME_LEN);
      }
      if (strcmp(argv[i], "--outfile") == 0) {
          strncpy(outfile_name, argv[i+1], MAX_FILENAME_LEN);
      }
  }
  for (i = 1; i < argc; i++) {
        if (strcmp(argv[i], "--forward") == 0) {
          forward_transform(infile_name, outfile_name);
        }
        if (strcmp(argv[i], "--backward") == 0) {
            backward_transform(infile_name, outfile_name);
        }
  }

#ifdef DEBUG
  printf("Input filename is: %s\n", infile_name);
  printf("Output filename is: %s\n", outfile_name);
  printf("Number of args: %d\n",argc);
  printf("Number of chars in file: %d\n", size_of_file(infile_name));
#endif
  return 0;
}

/*********************Forward Transform***************************/

/*This function reads from the file and adds to an array*/
void forward_transform(char* infile_name, char* outfile_name){

      int num_blocks, left_over;
      num_blocks = size_of_file(infile_name)/20;
      left_over = size_of_file(infile_name)%20;
      int block_size = BLOCK_SIZE;

      /*Open infile*/
      FILE *infile = fopen(infile_name,"r");

      /*Return an error if unable to open the input file*/
      if(infile==NULL){
        fprintf(stderr, "Unable to open input file: %s\n",infile_name);
        exit(1);
      }

      /*Create a character array of the left_over size*/
      char char_arr[block_size+1];

      /*Write hex codes to start of file*/
      open_outfile(outfile_name);

      /*If more than one block is to be read*/
      while(num_blocks>0){
        /*Read in one block at a time and decrement num_blocks*/
        num_blocks--;
        process_block(char_arr, infile_name, block_size-1, infile);
        rotate_sort_write(char_arr, block_size, outfile_name, num_blocks);

      }
        /*Read leftovers*/
        num_blocks=-100;
        process_block(char_arr, infile_name, left_over-1, infile);
        rotate_sort_write(char_arr, left_over, outfile_name, num_blocks);

#ifdef DEBUG2
  int i;
  int len = strlen(char_arr);
  printf("Number of blocks: %d\n",num_blocks);
  printf("Number of leftover: %d\n",left_over);
  /*Print out characters to verify*/
  printf("First block read:\n");
  printf("Block size: %d\n", block_size);
  if(num_blocks>0){
    for(i=0;i<block_size+1;i++){
          printf("%c", char_arr[i]);
    }
    printf("\n");
  }
  else{
    for(i=0;i<left_over;i++){
          printf("%c", char_arr[i]);
    }
    printf("\n");
  }

  //printf("$$\n"); /*---- used to verify spaces*/
  printf("Size of char_arr: %d\n", len);
#endif
}

/*Reads in the file and stores it to a 2D array*/
char* process_block(char* char_arr, char* infile_name, int block_size, FILE *infile){

      char EOT = (char) 3;
      char NULL_CHAR = (char) 0;

      /*Return an error if unable to open the input file*/
      if(infile==NULL){
        fprintf(stderr, "Unable to open input file: %s\n",infile_name);
        exit(1);
      }

      /*Create a character array of the block_size*/
      char temp;

      /*Loop through number of elements and copy to char_arr*/
        int i;
        for(i=0;i<block_size;i++){
          temp = fgetc(infile);
          char_arr[i]=temp;
        }
        /*Set last element in char_arr to null and then E0T*/
        char_arr[block_size] = EOT;
        char_arr[block_size+1] = NULL_CHAR;

        return char_arr;

#ifdef DEBUG3
  printf("Block size: %d\n", block_size);
  int len = strlen(char_arr);
  printf("Length of char_arr: %d", len);
  printf("\n");
#endif
}

/*Perform the shift rotation on each row of the 2D array*/
void rotate_sort_write(char* char_arr, int block_size, char* outfile_name, int num_blocks){

      char EOT = (char) 3;
      char NULL_CHAR = (char) 0;

      int i, j;
      char char_arr_2D[block_size+2][block_size+2];
      int len = strlen(char_arr);

      /*Copy from first row to other rows*/
      for(i=0;i<block_size;i++){
        strncpy(char_arr_2D[i],char_arr,block_size);
      }

      char_arr_2D[0][block_size] = NULL_CHAR;

      /*Shift each array row over by one*/
      for(i=0;i<block_size;i++){
        char tmp2 = char_arr_2D[i][0];
        for(j=0;j<=block_size+1;j++){
          char tmp = char_arr_2D[i][j]; /*Read first string from 1th char*/
          char_arr_2D[i+1][j-1] = tmp; /*Copy tmp to next column*/
        }
        char_arr_2D[i+1][len] = tmp2;
      }

      /*Sort 2D array in lexographic order*/
      qsort(char_arr_2D, block_size, sizeof(char)*block_size+2, compare_strings);

      /*Copy last column of sorted 2D array into last_col_arr*/
      char last_col_arr[block_size+1];
      char temp;
      for(i=0;i<block_size;i++){
        temp = char_arr_2D[i][block_size];
        last_col_arr[i]=temp;
      }

      /*Write last column to output file*/
      write_to_outfile(outfile_name, last_col_arr, block_size, num_blocks);

#ifdef DEBUG3
    for(i=0;i<block_size;i++){
      printf("%c",last_col_arr[i]);
    }
    printf("\n\n");
    /*printf("Len: %d\n",len);*/

    /*Print out the 2D array*/
    for(j=0;j<block_size;j++){
      for(i=0;i<block_size;i++){
            printf("%c", char_arr_2D[j][i]);
      }
      printf("\n");
    }
#endif
}

/****Write to ph1 file****/
void write_to_outfile(char* outfile_name, char* last_col_arr, int block_size, int num_blocks){

      FILE *outfile = fopen(outfile_name,"a");

      /*Return an error if unable to open the output file*/
      if(outfile==NULL){
        fprintf(stderr, "Unable to open output file: %s\n",outfile_name);
        exit(1);
      }

      /*Write new line if last block*/
      if(num_blocks==-100){
        fputc('\n', outfile);
      }

      char ch;
      int i;
      for(i=0;i<block_size;i++){
        ch = last_col_arr[i];
        if(ch == '\x00'||ch== '\xA4'){
          ch = '\x03';
        }
        fputc(ch, outfile);
      }

      fclose(outfile);
}

/*Open output file for writing and add hex chars*/
void open_outfile(char* outfile_name){
  /*Hex characters for start of file*/
  int ch1 = '\xab'; int ch2 = '\xba'; int ch3 = '\xbe'; int ch4 = '\xef';
  char NP = '\x14'; int ch5 = '\00';

  FILE *outfile = fopen(outfile_name,"a");

  /*Return an error if unable to open the output file*/
  if(outfile==NULL){
    fprintf(stderr, "Unable to open output file: %s\n",outfile_name);
    exit(1);
  }

  /*Write hex characters to file*/
  fputc(ch1,outfile); fputc(ch2,outfile); fputc(ch3,outfile); fputc(ch4,outfile);
  fputc(NP,outfile); fputc(ch5,outfile); fputc(ch5,outfile); fputc(ch5,outfile);

  fclose(outfile);
}

/*********************Backward Transform**************************/

void backward_transform(char* infile_name, char* outfile_name){
  int num_char;
  char char_arr[30];
  char junk_arr[20];
  char ch;
  int i;

  FILE *infile = fopen(infile_name,"r");

  if(infile==NULL){
    fprintf(stderr, "Unable to open input file: %s\n",infile_name);
    exit(1);
  }

  /*Find total number of chars in input file*/
  num_char = size_of_file(infile_name)-8;

  /*Number of chars minus hex junk*/
  int num_blocks = num_char/BLOCK_SIZE;
  int left_over = num_char%BLOCK_SIZE;
  int block_size = BLOCK_SIZE;

  /*Remove hex junk from front*/
  do{
    ch = fgetc(infile);
    junk_arr[i]=ch;
  } while(ch=='\xab'||ch=='\xba'||ch=='\xbe'||ch=='\xef'||ch=='\x14');
  ch = fgetc(infile);ch = fgetc(infile);

  /*Read in blocks of 21*/
  while(num_blocks>0){
    process_ph1(char_arr, infile_name, block_size, infile, num_blocks);
    num_blocks--;
    rotate_sort_ph1(char_arr, block_size, outfile_name, num_blocks);
  }
  /*Read in leftovers*/
    process_ph1(char_arr, infile_name, left_over-1, infile, num_blocks);
    num_blocks=-100;
    rotate_sort_ph1(char_arr, left_over-1, outfile_name, num_blocks);

#ifdef DEBUG4
printf("Block size: %d\n", block_size);
printf("Leftovers: %d\n", left_over);
printf("Number of blocks: %d\n", num_blocks);
printf("Num char: %d\n", num_char);
#endif
}

/*Read in blocks from PH1 file*/
char* process_ph1(char* char_arr, char* infile_name, int block_size, FILE *infile, int num_blocks){

  char temp;
  int i;

  /*Copy ph1 file into array*/
  for(i=0;i<block_size;i++){
    temp = fgetc(infile);
    if(temp=='\n'&&num_blocks==0){
      i--;
      continue;
    }
    char_arr[i]=temp;
  }
  return char_arr;
}

/*Perform backwards algorithm on the PH1 data*/
void rotate_sort_ph1(char* char_arr, int block_size, char* outfile_name, int num_blocks){

    int i,j;
    char temp;
    int col_count = 1;
    int col_of_orig = 0;
    char char_arr_2D[block_size][block_size];
    char char_temp_2D[block_size][block_size];
    char out_arr[block_size-1];

    while(col_count<block_size){
      /*Step 1: Copy string into first column of a 2D array*/
      for(i=0;i<block_size;i++){
        temp = char_arr[i];
        char_arr_2D[i][0] = temp;
      }

      /*Step 2: Copy contents of the original 2D array into temp 2D array*/
      for(j=0;j<block_size;j++){
        for(i=0;i<block_size;i++){
              temp = char_arr_2D[i][j];
              char_temp_2D[i][j] = temp;
        }
      }

      /*Sort in lexographic order*/
      qsort(char_temp_2D, block_size, sizeof(char)*block_size, compare_strings);

      /*Step 3: Copy the right most text column back into original array*/
        for(i=0;i<block_size;i++){
              temp = char_temp_2D[i][col_count-1];
              char_arr_2D[i][col_count] = temp;
        }
        col_count++;
    }

    /*Find row with last character as null*/
    for(i=0;i<block_size;i++){
      temp = char_arr_2D[i][block_size-1];
      if(temp == '\x03'){
        break;
      }
      col_of_orig++;
    }

    /*Copy original row to array for output*/
    for(i=0;i<block_size-1;i++){
      temp = char_arr_2D[col_of_orig][i];
      out_arr[i] = temp;
      /*printf("%c", temp);*/
    }

    /*Output transformed array to textfile*/
    write_to_textfile(outfile_name, out_arr, block_size, num_blocks);

#ifdef DEBUG5

printf("\nColumn of original: %d", col_of_orig);

printf("\n");printf("\n");
printf("Original 2D Array:\n");
    for(j=0;j<block_size;j++){
      for(i=0;i<block_size;i++){
            printf("%c", char_arr_2D[j][i]);
      }
      printf("\n");
    }

    printf("\n");printf("\n");
    printf("Sorted 2D Array:\n");
        for(j=0;j<block_size;j++){
          for(i=0;i<block_size;i++){
                printf("%c", char_temp_2D[j][i]);
          }
          printf("\n");
        }
#endif
}

void write_to_textfile(char* outfile_name, char* out_arr, int block_size, int num_blocks){

      FILE *outfile = fopen(outfile_name,"a");

      /*Return an error if unable to open the output file*/
      if(outfile==NULL){
        fprintf(stderr, "Unable to open output file: %s\n",outfile_name);
        exit(1);
      }

      char ch;
      int i;
      for(i=0;i<block_size-1;i++){
        ch = out_arr[i];
        if(ch == '\00' || ch == '\03'){
          continue;
        }
        fputc(ch, outfile);
      }

      /*printf("Block size: %d", block_size);*/
      /*If last block then add new line at end*/
      if(num_blocks==-100&&block_size!=-1){
        fputc('\n',outfile);
      }

      fclose(outfile);
}

/**************Shared Functions***************/

/*Counts the number of characters in the input file*/
int size_of_file(char* infile_name){
  int ch, num_char;
  FILE *infile = fopen(infile_name,"r");
  /*Return message if cannot open file*/
  if(infile==NULL){
    fprintf(stderr, "Unable to open inputfile: %s\n",infile_name);
    exit(1);
  }
  /*Loop through text file and count the number of characters*/
  num_char=0;
  while((ch=fgetc(infile))!=EOF){
    num_char++;
  }

  fclose(infile);
  return num_char;
}

/*Called by QSort comparison*/
int compare_strings(const void *a, const void *b) {
    char *sa = (char *)a;
    char *sb = (char *)b;
    return strcmp(sa, sb);
}
