#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include<limits.h>

#define ERROR "An error has occurred\n"

char cmd[128];
char finalcmd[128];
int numpath;
int path_set = 0;

struct path_struct{
	char path_var[128];
}spath[128];

void remove_leading_spaces(){
	int leading_space = 0;
	int i = 0, j =0;

	while(1){
		if(cmd[i] == ' '){
			//printf("\n Space encountered!");
			leading_space++;
		}else if(cmd[i] == '\0'){
			//printf("\n Null encountered!");
			break;
		}else{
			//printf("\n Character encountered!");
			break;
		}
		i++;
	}
	//printf("\n No of characters traversed : %d", i);

	//printf("\n No of leading spaces : %d", leading_space);

	for(i = leading_space; i < strlen(cmd); i++){
		//printf("\n Copying character %c", cmd[i]);
		finalcmd[j++] = cmd[i];
	}
	finalcmd[j] = '\0';
	/*printf("\n Command after removing leading spaces : %s", finalcmd);
	printf("\n length of final command : %lu", strlen(finalcmd));*/

	strcpy(cmd, finalcmd);
	//printf("\n After copying, command : %s", cmd);
}


void remove_trailing_spaces(){

	int trailing_space = 0;	
	int i = strlen(cmd) - 1;
	
	while(i >= 0){
		if(cmd[i] == ' '){
			//printf("\n Space encountered!");
			trailing_space++;
		}else if(cmd[i] == '\0'){
			//printf("\n Null encountered!");
		}else{
			//printf("\n Character encountered!");
			break;
		}
		i--;
	}
	//printf("\n No of trailing spaces : %d", trailing_space);
	finalcmd[0] = '\0';

	for(i = 0; i < strlen(cmd) - trailing_space; i++){
		finalcmd[i] = cmd[i];
	}
	finalcmd[i] = '\0';
	cmd[0] = '\0';
	strcpy(cmd, finalcmd);
	/*printf("\n Final Command after removing leading spaces : %s", finalcmd);
	printf("\n length of final command : %lu", strlen(finalcmd));

	printf("\n Command after removing leading spaces : %s", cmd);
	printf("\n length of command : %lu", strlen(cmd));*/

}



void print_error(){
	write(STDERR_FILENO, ERROR, strlen(ERROR));
}


int main(int argc, char* argv[]){
	
	int i = 0, j = 0;
	char *cwd;
	char buffer[PATH_MAX + 1];
	char *path;

	if(argc > 1){
		print_error();
		return 1;
	}
	while(1){
		printf("whoosh>");
		gets(cmd);
		//printf("The command you entered : %s\n",cmd); 
		
		if(cmd[127] != '\0'){
			printf("\n Command longer than 128 characters!");
			continue;
		}else{
			//printf("\n Last character is null");
		}

	
		remove_leading_spaces();
		remove_trailing_spaces();
		
		if(strlen(cmd) == 0){
			continue;
		}

		if(strcmp(cmd, "exit") == 0){
			exit(0);
		}else if(strcmp(cmd, "pwd") == 0){
			cwd = getcwd(buffer, PATH_MAX + 1);
			if(cwd != NULL){
				printf("%s\n", cwd);
			}else{
				print_error();
			}
		}else if(strcmp(cmd, "cd") == 0){
			//printf("\n Cd without arguments!");
 			path = getenv("HOME");

			if(chdir(path) == 0){
				//Do nothing
				//printf("\nDirectory changed successfully!\n");
			}else{
				//printf("\nError!");
				print_error();
			}

		}else if(cmd[0] == 'c' && cmd[1] == 'd'){
			//printf("\n Cd with arguments!");
			char dir[128];
			int dir_index  = 0;
			int dir_end_index = 0;

			for(i = 2; i < strlen(cmd); i++){
				if(cmd[i] != ' '){
					break;				
				}
			} 
			
			dir_index = i;
			//printf("\n Space ends in position : %d", i-1);
			//printf("\n Length of command : %d", strlen(cmd));

			for(j = i; j < strlen(cmd); j++){
				if(cmd[j] == ' '){
					break;
				}
			}
			dir_end_index = j;

			//printf("\n Value of j after searching for spaces : %d", j);
			//printf("\n Directory found starting in position %d", dir_index);	
			if(j != strlen(cmd)){
				/*printf("\nSpace found in or after directory path or multiple directories listed!");
				print_error();
				continue;*/
				int k = 0;
				for(i = dir_index; i < dir_end_index; i++){
					dir[k++] = cmd[i];
				}
			}else{		
				int k = 0;
				for(i = dir_index; i < strlen(cmd); i++){
					dir[k++] = cmd[i];
				}
			}
			//printf("\nDirectory to change to : %s", dir);
			
			if(chdir(dir) == 0){
				//Do nothing
				//printf("\nDirectory changed successfully!\n");
			}else{
				//printf("\nError!");
				print_error();
			}
		}else if(cmd[0] == 'p' && cmd[1] == 'a' && cmd[2] == 't' && cmd[3] == 'h'){
			 char *token = strtok(cmd, " ");
   			i = numpath;
			/* walk through other tokens */
			while( token != NULL ) 
			{	
				if(strcmp("path", token) != 0){
					strcpy(spath[i++].path_var, token);
				}
				//printf( " %s\n", token );
				token = strtok(NULL, " ");
			}
			numpath = i;
			path_set = 1;
			/*for(i = 0; i < numpath; i++){
				printf("Path  = %s\n", spath[i].path_var);
			}*/
		}else{	
			int pid = fork();
			//printf("\n I am CHILD with PID : %d", (int)getpid());
			if(pid < 0){
				print_error();
			}else if(pid == 0){
				char full_path[128];
				char token[100];

				char *args[128];
	
				char new_cmd[128];
				int prev_space_found = 0;
				int i = 0;
				int j = 0;
				while( i < strlen(cmd)){
					//printf("\nProcessing character  :%c", cmd[i]);
					if(cmd[i] != ' '){
						if(prev_space_found == 1){
							prev_space_found = 0;			
						}
						new_cmd[j++] = cmd[i];
					}else if(cmd[i] == ' '){
						if(prev_space_found == 1){
							i++;
							continue;
						}else{
							new_cmd[j++] = cmd[i];
							prev_space_found = 1;
						}
					}
					i++;
				}
				new_cmd[j] = '\0';

				/*printf("\n Command after removing spaces in between : %s", new_cmd);
				printf("\n Length of new command : %d", strlen(new_cmd));*/
	
				i = 0;
				j = 0;
				int k = 0;

				while(1){
					if(new_cmd[i] == '\0'){
						break;
					}else if(new_cmd[i] == ' '){
						token[k] = '\0';
						args[j] = (char*) malloc(sizeof(char) * strlen(token));
						strcpy(args[j], token);
						j++;
						k = 0;
					}else{
						token[k] = new_cmd[i];
						k++;
					}
					i++;
				}
				token[k] = '\0';
				args[j] = (char*) malloc(sizeof(char) * strlen(token));
				strcpy(args[j], token);
				args[j+1] = NULL;
					
				/*printf("\n Total no of tokens : %d", j+1);
				for(i = 0; i <= j; i++){
					printf("\nToken %d = %s", i, args[i]);
				}*/
				
				//i = 0;
				for(i = 0; i  < numpath; i++){
					full_path[0] = '\0';
					strcat(full_path, spath[i].path_var);
					strcat(full_path, "/");
					strcat(full_path, args[0]);
					int file_exists = access( full_path, F_OK );
					//printf("\n File exists : %d", file_exists);
					if(file_exists == -1){
						continue;
					}
					//printf("\n Full path : %s\n", full_path);
					if( access( full_path, F_OK ) != -1 ) {
						//printf("\n Path set = %d", path_set);
						//printf("\n Command File exists!");
						if(path_set == 1){
							//printf("\n Going to execute execv()!\n");
							if (execv(full_path, args) < 0){
								exit(0);
							}
						}else{
							exit(0);					
						}
					}/*else{
						printf("\n Command File does not exist!\n");
						//exit(0);					
					}*/
					
				}
				exit(0);
			} else {
				int id = wait(NULL);
				//printf("\n I am Parent with PID : %d\n", (int)getpid());
				//printf("\n My child's PID is %d\n", id);
			}
				
			
		}
	}
	
}
