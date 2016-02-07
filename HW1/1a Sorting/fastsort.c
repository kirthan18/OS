#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <string.h>

#define READ_FLAG "r"

//TODO Check this
#define LINE_SIZE 129

#define BAD_CMD_LINE_PARAMETERS "Error: Bad command line parameters\n"
#define INVALID_FILE "Error: Cannot open file "
#define MALLOC_FAIL "malloc failed\n"
#define LONG_LINE "Line too long\n"

#define SUCCESS 0
#define FAILURE 1

struct line_helper
{
	char word[LINE_SIZE];
	char line[LINE_SIZE];
	int line_num;
};

/* qsort struct comparision function */ 
int compare(const void *a, const void *b) 
{ 
    struct line_helper *ia = (struct line_helper *)a;
    struct line_helper *ib = (struct line_helper *)b;
    return (strcmp(ia -> word, ib -> word));
} 

int get_number_of_lines(FILE *fp){
	int num_lines = 0;
	int ch;

	do{
		ch = fgetc(fp);
		if(ch == '\n'){
			num_lines++;
		}
	}while(ch != EOF);

	return num_lines;
}

void print_lines(struct line_helper line_arr[], int size){
	int i;

	for(i = 0; i < size; i++){
		//printf("%d\t%s",line_arr[i].line_num, line_arr[i].line);
		printf("%s", line_arr[i].line);
	}
}

int main(int argc, char* argv[]){
	
	int default_case = -1;
	int sort_key = -1;
	
	int num_lines_file = 0;

	FILE *fp;

	char *filename;
	char line[LINE_SIZE];

	struct line_helper *line_array;

	//printf("%d\n", argc);

	if(argc < 2 || argc > 3){
		fprintf(stderr, BAD_CMD_LINE_PARAMETERS);
		return FAILURE;
	}

	if(argc == 2){
		default_case = 1;
		filename = argv[1];
	}else{ 
		default_case = 0;
		filename = argv[2];
		sort_key = atoi(argv[1]) * -1;
		if(sort_key <= 0){
			fprintf(stderr, BAD_CMD_LINE_PARAMETERS);
			return FAILURE;
		}
		// printf("Sort key : %d\n", sort_key);
	}

	// printf("File name : %s\n", filename);

	fp = fopen(filename, READ_FLAG);
	if(fp == NULL){
		fprintf(stderr, "Error: Cannot open file %s\n",filename);
		return FAILURE;
	}

	num_lines_file = get_number_of_lines(fp);
	// printf("\nNumber of lines in file : %d\n", num_lines_file);
	
	line_array = (struct line_helper*) malloc( num_lines_file * sizeof(struct line_helper));
	if(line_array == NULL){
		fprintf(stderr, MALLOC_FAIL);
		return FAILURE;
	}

	int line_number = 0;

	fseek( fp, 0, SEEK_SET);
	while(fgets(line, LINE_SIZE, fp) != NULL){

		int line_length = 0;
		int eol_found = 0;

		// printf("Line read : %s", line);
		// printf("\n&&&&&&&& String length of line  : %lu\n", strlen(line));
		while(line_length < LINE_SIZE){
			line_length++;
			if(line[line_length] == '\n'){
				eol_found = 1;
				break;
			}
		}

		if(eol_found == 0){
			fprintf(stderr, LONG_LINE);
			return FAILURE;
		}

		//line_array[line_number].word = "";
		//printf("Line read : %s", line);
		strcpy(line_array[line_number].line,line);
		line_array[line_number].line_num = line_number;

		char* token = strtok (line," ");
		char *prev = NULL;

		if(default_case != 1){
			int token_num = 1;
			

			while (token != NULL){

			    // printf ("Token : %s\n",token);

			    if(token_num >= sort_key || token == NULL){
			    	break;
			    }

			    prev = strdup(token);
			    token = strtok (NULL, " ");
			    token_num++;
		  	}
	  	}

	  	if(token == NULL){
	  		token = strdup(prev);
	  	}

	  	int i = 0;
	  	while(token != NULL && token[i] != '\0'){
	  		if(token[i] == '\n'){
	  			token[i] = '\0';
	  			break;
	  		}
	  		i++;
	  	}

	  	strcpy(line_array[line_number].word,token);
	  	// printf("Word to be sorted : %s\n", line_array[line_number].word);

		line_number++;
	}

	// printf("\n*****Original lines : *****\n");
	// print_lines(line_array, num_lines_file);

	qsort(line_array, num_lines_file, sizeof(struct line_helper), compare);

	//printf("\n*****Sorted lines : *****\n");
	print_lines(line_array, num_lines_file);
	
	return SUCCESS;
}
