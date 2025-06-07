/***********************************************************
 ***************** Aspridis Nikolaos csd5178 ***************
***********************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <linux/limits.h>
#include <string.h>
#include <sys/wait.h>

#define shell_readline_length 51
#define number_of_tokens 51
#define number_of_commands 51 
#define number_of_global_variables 51

void my_shell();
int print_prompt();
char *read_cmd_line();
char **split(char *string);
void split_and_execute(char *cbuffer);


/* struct for global variables */
typedef struct global_variable {
	char *name;
	char *value;
}gl_var;


gl_var **gl_var_array;	/* global-variable pointer array */
int cnum;		/* current num of global variables */

int main(){

	my_shell();   /* procedure my_shell() is responsible for creating and running the shell */

	return 0;
}


void my_shell() {
	int x, i;
	char *cbuffer;
	
	/* creates global-variable pointer array for global variables' storage */
	gl_var_array = (gl_var **)malloc(number_of_global_variables * sizeof(gl_var *));	
	if(!gl_var_array){
		fprintf(stderr, "ERROR: Malloc failed!\nProcess terminated!\n\n");
		return;
	}
	cnum = 0;	/* current number of global variables */

	/* initialise global-variable pointer array with NULL */
	i = 0;
	while(i<number_of_global_variables) {
		gl_var_array[i] = NULL;
		i++;
	}


	do{
		x = print_prompt();
		if(x){
			/* no username found */
			return;		
		}

		cbuffer = read_cmd_line();
		if(!cbuffer){
			/* storage of command failed */
			return;
		}
	
		split_and_execute(cbuffer);

	}while(1);

}



/* 
 * finds num of semicolons and decides how many commands have to be executed 
 */
void split_and_execute(char *cbuffer) {
	int count, i, j, k, f, num_of_semicolons, found_equal, found_space, found_gl_var;
	size_t len;
	char **tokens, **toks, **comm_toks;
	char *s, *s1, *s2, *s3, *s4, *sHelper;
	pid_t p, h;
	

	/* finds the number of semicolons */
	i=0;
	num_of_semicolons = 0;
	while(cbuffer[i]){
		if(cbuffer[i] == ';'){
			num_of_semicolons++;
		}
		i++;
	}

	if(num_of_semicolons){
		/* more than one commands */
		toks = (char **)malloc((num_of_semicolons+1) * sizeof(char *));
		if(!toks) {
			fprintf(stderr, "ERROR: Malloc failed!\n1st if, procedure 'split_and_execute'.\nProcess terminated!\n\n");
			return;
		}

		/* initialise toks char* array with NULL */
		i=0;
		while(i<(num_of_semicolons+1)){
			toks[i] = NULL;
			i++;
		}
	
		/* toks array fills up with commands */
		i = 0;
		s = strtok(cbuffer, ";");	
		while(s){
			toks[i] = s;
			i++;
			s = strtok(NULL, ";");	/* delimeters: ; */		
		}

			
		/* each command is splitted into tokens and is executed */
		for(i=0; i<(num_of_semicolons+1); i++){
			
			j=0;
			while(toks[i][j] == ' '){
				j++;
			}
			
			toks[i] = &toks[i][j];
			/* but first, each command is checked if it is global variable creation */
			j = 0;
			found_equal = 0;
			while(toks[i][j]){
				if(toks[i][j] == '='){
					found_equal = 1;
					break;
				}
				j++;
			}
		
			if(!found_equal){
				/* no '=' found, so toks[i] command is not a global variable creation */
				/* comm_toks = split(toks[i]); */
				comm_toks = split(toks[i]);
				if(!comm_toks) return; /* after semicolon was null, so no other command finally */

				p = fork();
				if(p<0){
					/* fork failed */
					fprintf(stderr, "ERROR: fork failed!\nProcess terminated!\n\n");
					return;
				}

				if(!p) {
					/* child process */
					j = execvp(comm_toks[0], comm_toks);
					if(j == -1) {
						fprintf(stderr, "ERROR: execvp failed!\nChild process terminated!\n\n");
						_exit(1);
					}
				}else {
					/* parent process */		
					h = wait(NULL); /* waits till the child process ends */	
				}	
			}else {
				/* '=' found, so toks[i] command maybe is a global variable creation */
				s1 = strtok(toks[i], "="); /* returns string before '=' */
				/* checks if string before '=' contains [space] character or not */
				j = 0;
				found_space = 0;
				while(s1[j]){
					if(s1[j] == ' '){
						found_space = 1; 
						break;
					}
					j++;
				}

				if(!found_space) {
					/* space was not found before '=', so it's time to check part after '=' for any [space] */	
					s2 = strtok(NULL, "");		/* takes the rest of the string (part after first '=') */
					/* checks if the rest contains any [space] char or not */
					j = 0;					
					found_space = 0;
					while(s2[j]){
						if(s2[j] == ' '){
							found_space = 1; 
							break;
						}
						j++;
					}

					if(!found_space){
						/* space was not found after first '=', so it's time to create the global variable */
						gl_var_array[cnum] = (gl_var *)malloc(sizeof(gl_var)); 				
						if(!gl_var_array) {
							fprintf(stderr, "ERROR: Malloc failed.\nProcess terminated!\n\n");
							return;
						}
					
						gl_var_array[cnum]->name = s1;  /* variable's name is part before (first) '=' */		
						gl_var_array[cnum]->value = s2; /* variable's value is part after '=' */
					
						cnum++;	/* current number of global variables increases by one */

					}else {
						/* space found after first '='. error! */
						fprintf(stderr, "ERROR: no such command.\nProcess terminated!\n\n");
						return;
					}				

				}else {
					/* space found before '='. error! */
					fprintf(stderr, "ERROR: no such command.\nProcess terminated!\n\n");
					return;	
				}
			}	
		}
	
	}else{
		/* one command only */
		
		found_gl_var = 0; /* no global variables have been found yet */
		
		/* checks if '=' exists into cbuffer (if any) */
		i = 0;
		found_equal = 0;
		while(cbuffer[i]){
			if(cbuffer[i] == '='){
				found_equal = 1;
				break;
			}
			i++;
		}
		
		if(!found_equal){
			/* no global variables' initialisation */
			/* Echo check */
			/* copies cbuffer onto s in order cbuffer not to be changed */
			s = (char *)malloc(sizeof(char) * (strlen(cbuffer)+1));
			if(!s){
				fprintf(stderr, "ERROR: Malloc failed!\nProcedure terminated!\n\n");
				return;
			}
			s = strcpy(s, cbuffer);

			/* checks if echo command */
			s1 = strtok(s, " \n");
			if(!strcmp(s1, "echo")){
				/* echo command */
				/* count = 0; */
				i=0;
				s2 = strtok(NULL, "\n");	/* the rest of the command (echo's args) is returned to s2 */
				/* searches for $ */
				while(s2[i]){
					if(s2[i] == '$'){
						/* $ found */
						/* checks if next char is [space]/[\n] */
						if((s2[i+1] == ' ') ||(s2[i+1] == '\n') || (s2[i+1] == '\0')){
							/* no global variables to be printed */
							i++;
							continue;		
						}else {
							/* checks if next is a gl_var name or not */
							s3 = (char *)malloc(sizeof(char) * (strlen(&s2[i+1])+1));
							if(!s3){
								fprintf(stderr, "ERROR: Malloc failed!\nProcess terminated!\n\n");
								return;
							}

							s3 = strcpy(s3, &s2[i+1]);					
	
							sHelper = (char *)malloc(sizeof(char) * (strlen(s3)+1));
							if(!sHelper){
								fprintf(stderr, "ERROR: Malloc failed!\nProcess terminated!\n\n");
								return;
							}

							sHelper = strtok(s3, "$ \n"); /*sHelper is assigned with the token following $ */
							s4 = (char *)malloc(sizeof(char) * (strlen(sHelper)+1));
							if(!s4){
								fprintf(stderr, "ERROR: Malloc failed!\nProcess terminated!\n\n");
								return;
							}
							s4 = strcpy(s4, sHelper); /* s4 is assigned context of sHelper */

							for(j=0; j<cnum; j++){
								/* s4 is compared with each gl_var name */
								if(!strcmp(s4, gl_var_array[j]->name)){
									/* s4 is a gl_var name */
									count++;
									found_gl_var = 1;
									len = strlen(s4);
									/* exchanges $s4 with gl_var s4 corresponds to */
									if(i!=0){	
										s4 = strncpy(s4, s2, i); /* s4 is assgined all chars before $ */
									}else{
										s4[0] = '\0';
									}
									s4 = strcat(s4, gl_var_array[j]->value); /* gl_var's value is appended in s4 */
									s3 = strcpy(s3, &s2[i+(len+1)]);	/* s3 is assigned the rest of the string, right after the global variable */
									s4 = strcat(s4, s3);			/* s3 is appended in s4 */
									s2 = s4;
									break;
								}	
							}	
						}
					}
					i++;
				}			
			}		

			if(!found_gl_var){
				/* no global variables */
				/* tokens = split(cbuffer); */
				tokens = split(cbuffer);
			}else{
				s3 = (char *)malloc(sizeof(char) * (5+strlen(s4)+1));
				if(!s3){
					fprintf(stderr, "ERROR: Malloc failed!\nProcess terminated!\n\n");
					return;
				}
				
				s3[0] = 'e';
				s3[1] = 'c';
				s3[2] = 'h';
				s3[3] = 'o';
				s3[4] = ' ';
				s3 = strcat(s3, s4);

				tokens = split(s3);
			}
			p = fork();
			if(p<0){
				/* fork failed */
				fprintf(stderr, "ERROR: fork failed!\nProcess terminated!\n\n");
				return;
			}

			if(!p) {
				/* child process */
				j = execvp(tokens[0], tokens);
				if(j == -1) {
					fprintf(stderr, "ERROR: execvp failed!\nChild process terminated!\n\n");
					_exit(1);
				}
			}else {
				/* parent process */		
				h = wait(NULL); /* waits till the child process ends */	
			}		
		}else if(found_equal){
			/* '=' found */
			s1 = strtok(cbuffer, "="); /* returns string before '=' */
			/* checks if string before '=' contains [space] character or not */
			i = 0;
			found_space = 0;
			while(s1[i]){
				if(s1[i] == ' '){
					found_space = 1; 
					break;
				}
				i++;
			}

			if(!found_space) {
				/* space was not found before '=', so it's time to check part after '=' for any [space] */	
				s2 = strtok(NULL, "");		/* takes the rest of the string (part after first '=') */
				/* checks if the rest contains any [space] char or not */

				i = 0;					
				found_space = 0;
				while(s2[i]){
					if(s2[i] == ' '){
						found_space = 1; 
						break;
					}
					i++;
				}

				if(!found_space){
					/* space was not found after first '=', so it's time to create the global variable */
					/* but before we create it, we must check if there is already a variable with that name */
					f = 0;
					for(i=0; i<(cnum-1); i++){
						if(!strcmp(gl_var_array[i]->name, s1)){
							/* an already existing g. var with that name found */
							gl_var_array[i]->value = s2; /* variable's new value is s2 (string after '=') */
							f = 1;
							break;	
						}
					}

					if(!f){
						gl_var_array[cnum] = (gl_var *)malloc(sizeof(gl_var)); 				
						if(!gl_var_array) {
							fprintf(stderr, "ERROR: Malloc failed.\nProcess terminated!\n\n");
							return;
						}
					
						gl_var_array[cnum]->name = s1;  /* variable's name is part before (first) '=' */		
						gl_var_array[cnum]->value = s2; /* variable's value is part after '=' */	
						cnum += 1; /* current number of global variables increases by one */
					}

				}else {
					/* space found after first '='. error! */
					fprintf(stderr, "ERROR: no such command.\nProcess terminated!\n\n");
					return;
				}				

			}else {
				/* space found before '='. error! */
				fprintf(stderr, "ERROR: no such command.\nProcess terminated!\n\n");
				return;
			}
		}
	}
}

/*

gl_var *create_global_variable(char *name, char *value){
	
	
}			*/


int print_prompt() {
	char *uname;
	char buf[PATH_MAX];

	if((uname = getlogin()) == NULL){
		fprintf(stderr, "ERROR: No username found !\n");
		return 1;
	}	

	if(getcwd(buf, sizeof(buf)) == NULL){
		fprintf(stderr, "ERROR: No path found !\n");
		return 1;
	}

	printf("csd5178-hy345sh@%s:%s ", uname, buf);

	return 0;

}


char *read_cmd_line() {
	char *cbuffer;
	int position, c, buffer_size;

	buffer_size = shell_readline_length;
	cbuffer = (char *)malloc(sizeof(char) * buffer_size);
	if(!cbuffer){
		fprintf(stderr, "ERROR: Memory allocation failed. Malloc returned null!\nProcess terminated!\n");
		return NULL;
	}

	position = 0;
	c = getchar();
	while((c != EOF)&&(c != '\n')) {
		if(position == buffer_size) {
			buffer_size += shell_readline_length;
			cbuffer = (char *)realloc(cbuffer, buffer_size);
			if(!cbuffer) {
				fprintf(stderr, "ERROR: Memory allocation failed. Realloc returned null!\nProcess terminated!\n");	
				return NULL;
			}
		}
		cbuffer[position] = (char)c;
		position++;
		c = getchar();			
	}
	cbuffer[position] = '\0';
	
	return cbuffer;
}


char **split(char *string) {
	char *s;
	char **tokens;
	int i, j, num;	

	/* throws away starting spaces (if any) */
	/*i=0;
	while(string[i] == ' '){
		i++;
	}
	s = &string[i]; */

	s = strtok(string, " ;\n");	/* delimeters: [space], ;, '\n' */		
	if(!s) return NULL; /* "empty" command */	

	tokens = (char **)malloc(sizeof(char*) * number_of_tokens);
	if(!tokens){
		fprintf(stderr, "ERROR: Malloc failed in line 13 of procedure split().\nProcedure failed and null is returned!\n\n");
		exit(1);
	}

	/* initialise tokens aray with null, for execvp later usage */
	j = 0;
	num = number_of_tokens;
	while(j<num){
		tokens[j] = NULL;
		j++;
	}

	i = 0;
	while(s){
		if(i == num){
			j = num;
			/* tokens array expands */
			num += number_of_tokens;
			tokens = (char **)realloc(tokens, num * sizeof(char *));
			if(!tokens){
				fprintf(stderr, "ERROR: Realloc failed in line 25 of procedure split().\nProcedure failed and null is returned!\n\n");
				return NULL;
			}	
			
			/* initialise new token-aray elements with null, for execvp later usage */
			while(j<num){
				tokens[j] = NULL;	
			}	
		}
		
		tokens[i] = s;
		i++;
		s = strtok(NULL, " ;\n");	/* delimeters: [space], ;, '\n' */		
	}
	
	return tokens;

}


/* [space] ; '\n' */












