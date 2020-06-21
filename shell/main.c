#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <signal.h>
#include <sys/wait.h>
#include <fcntl.h>
//#include <time.h>
#include "grammar.h"

pid_t pid = -1;
extern char **environ;
int type = 0;
int cd(char **args);
int _setenv(char *name,char *value);
int env(char **args, char**args2);
int is_background = 0;
char userinput[512];

//task 1
int cd(char **args){
	if (chdir(args[1]) != 0) {
        perror("shell");
    }
    return 0;
}

int _setenv(char *name,char *value){
    if (value == NULL){
		setenv(name, "", 1);
        printf("%s", "variable overwritten \n");
	}else{
		setenv(name, value, 1);
        printf("%s", "variable overwritten \n");
	}
    return 0;
}

int env(char **args, char**args2){
	char **env_arr; 
	if(args2[1] == NULL){
        for(env_arr = environ; *env_arr != 0; env_arr ++){
		    printf("%s\n", *env_arr);
	    }
    }else{
        int i = 1;
        while(args2[i] != NULL){
            printf("%s\n", args2[i]);
            i++;
        }
    }
    return 0;
}

//task 2, task 3 & task4
void programs(char **args, int is_background){	 
    pid = fork();
    //printf("pid = %d, is_background = %d\n", pid, is_background);
	if(pid == 0){ 
        signal(SIGINT, SIG_DFL);
        int tmp = execvp(*args, args);
		if (tmp < 0){
			printf("Command not found\n");
            exit(1);
		}
	}else{
	if (is_background == 0){
        waitpid(pid,NULL,0);
    }
    }
}


void _sigint(int signo) {
    printf("\nContinue..\n");
}

//deal with all commands
int commands(int argc, char **args, char **args2){
    int q = 0;
	if(strcmp(args[0],"exit") == 0) {
        printf("Bye-bye \n");
        exit(0);
    }
    else if (strcmp(args[0],"pwd") == 0) printf("%s\n",getcwd(args[1], 1024)); 
	else if (strcmp(args[0],"cd") == 0) cd(args);
	else if (strcmp(args[0],"env") == 0) env(0, args2);
    else if (strcmp(args[0],"echo") == 0) {
        int i = 1;
        while(args[i]!= '\0' && args2[0] == NULL){
        printf("%s%s",args[i],"");
        printf(" ");
        i++;
        }
        while(args2[i] != '\0'){
        printf("%s%s",args2[i],"");
        printf(" ");
        i++;
        }
        printf("\n");
    }
	else if (strcmp(args[0],"setenv") == 0){
        _setenv(args[1],args[2]);
    }else{
        programs(args,is_background);
    }
    return 1;
}



//parser
int get_nexttoken();

char buf[1024];
int is_eof = 0;
int prev = EOF;
int i = 0;

int get_nexttoken() {
    int pos = 0;
    char c;

    if (prev == '\n' || prev == '\r') { 
        buf[0] = prev;
        buf[1] = '\0';
        prev = EOF;
        return STATEMENT_END;
    }
    
    c = fgetc(stdin);
    while (isspace(c) && c != EOF) { 
        c = fgetc(stdin);
        userinput[i] = c; 
        //printf("5, userinput[%d] = %c\n", i, userinput[i]);
        //i++;
    }

    if (c == '$') {
        userinput[i] = c; 
        //printf("4, userinput[%d] = %c\n", i, userinput[i]);
        i++;
        buf[0] = '$';
        buf[1] = '\0';
        return DOLLAR_SIGN;
    }

        if (c == '&') {
        userinput[i] = c; 
        is_background = 1;
        //printf("3, userinput[%d] = %c\n", i, userinput[i]);
        i++;
        buf[0] = '&';
        buf[1] = '\0';
        c = EOF;
        return STATEMENT_END;
    }

    while (c != EOF) {
        prev = c;
        if (isspace(c)) {
            userinput[i] = c; 
            //printf("1, userinput[%d] = %c\n", i, userinput[i]);
            i++;
            buf[pos] = '\0';
            return ID;
        }
        buf[pos++] = c;
        userinput[i] = c; 
        //printf("2, userinput[%d] = %c\n", i, userinput[i]);
        i++;
        c = fgetc(stdin);
    }
    is_eof = 1;
    return FAIL;
}

int main(int argc, char *argv[]) {
    int token_no = 0;
    char *tokens[64];
    char *dest[64];
    char *val;
    printf("Welcome, enter a command!\n");
    while (!is_eof) {
        signal(SIGINT, _sigint);
        type = get_nexttoken();
        printf("token: type = %d, text is '%s'\n", type, buf);
        //for(int y = 0; y < 10; y++)printf("userinput = %c\n", userinput[y]);
        if(type == 3){
            //for(int y = 0; y < 10; y++)printf("userinput = %c\n", userinput[y]);
            for (int j = 0; j < 256; j++) { 
                if (userinput[j] == '$'){
                    if((tokens[0] = strtok(userinput," \n\t$")) == NULL) continue;
                    token_no = 1;
		            while((tokens[token_no] = strtok(NULL, " \n\t$")) != NULL) {
                    token_no++;
                    }
                    int p = 1;
                    while(tokens[p] != NULL){
                        val = getenv(tokens[p]);
                        dest[p] = val;
                        p++;
                    }
                    break;
                }
            }
        if((tokens[0] = strtok(userinput," \n\t")) == NULL) continue;
        token_no = 1;
		while((tokens[token_no] = strtok(NULL, " \n\t&")) != NULL) {
            token_no++;
            
        }
        //for(int x = 0; x < 64; x++) printf("%s\n", dest[x]);
        //for(int x = 0; x < 5; x++) printf("tokens = %s\n", tokens[x]);
        commands(argc, tokens, dest);
        memset(tokens, 0, 64);
        memset(dest, 0, 64);
        memset(userinput, 0, 512);
        val = 0;
        i = 0;
        //for(int y = 0; y < 10; y++)printf("userinput = %c\n", userinput[y]);
        }
    }
    free(val);
    return 0;
}
