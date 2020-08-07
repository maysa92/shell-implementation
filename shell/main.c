#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <signal.h>
#include <sys/wait.h>
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
        printf("%s", "done \n");
	}else{
		setenv(name, value, 1);
        printf("%s", "done \n");
	}
    return 0;
}

int env(char **args, char**args2){
	char **env_arr; 
	if(args2[0] == NULL){
        for(env_arr = environ; *env_arr != 0; env_arr ++){
		    printf("%s\n", *env_arr);
	    }
    }
    return 0;
}

//task 2, task 3 & task4
void programs(char **args, int is_background){	 
    pid = fork();
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
	if(strcmp(args[0],"exit") == 0) {
        printf("Bye-bye \n");
        exit(0);
    }
    else if (strcmp(args[0],"pwd") == 0) {
        printf("%s\n",getcwd(args[1], 2048)); 
    }
	else if (strcmp(args[0],"cd") == 0) cd(args);
	else if (strcmp(args[0],"env") == 0) env(0, args2);
    else if (strcmp(args[0],"echo") == 0) {
        for(int x = 1; x < 32; x++){
            if(args[x]!= '\0'){
                printf("%s%s",args[x],"");
                printf(" ");
            }
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
    }

    if (c == '$') {
        userinput[i] = c; 
        i++;
        buf[0] = '$';
        buf[1] = '\0';
        return DOLLAR_SIGN;
    }

        if (c == '&') {
        userinput[i] = c; 
        is_background = 1;
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
            i++;
            buf[pos] = '\0';
            return ID;
        }
        buf[pos++] = c;
        userinput[i] = c; 
        i++;
        c = fgetc(stdin);
    }
    is_eof = 1;
    return FAIL;
}

int main(int argc, char *argv[]) {
    int token_no = 0;
    char *tokens[32] = {0};
    char *dest[32] = {0};
    int tmp = 0;
    printf("Welcome, enter a command!\n");
    while (!is_eof) {
        signal(SIGINT, _sigint);
        type = get_nexttoken();
        //printf("token: type = %d, text is '%s'\n", type, buf);
        if(type == 3){
           // for(int y = 0; y < 10; y++)printf("userinput = %c\n", userinput[y]);
            tokens[0] = strtok(userinput," \n\t");
            token_no = 1;
            while(tmp < 511 && userinput[tmp] != '$') tmp++;
            //found first $
            if(tmp <511){
                    tokens[token_no] = strtok(NULL, " \n\t$");
                    while(tokens[token_no] != NULL) {
                        printf("%s\n", getenv(tokens[token_no]));
                        dest[token_no - 1] = getenv(tokens[token_no]);
                        tokens[token_no] = NULL;
                    }
                    token_no++;
                    tmp++;
                    while(tmp < 511 && userinput[tmp] != '$') tmp++;
                    //found second $
                    //printf("tmp = %d\n, token_no = %d\n", tmp, token_no);
                     if(tmp <511){
                        tokens[token_no] = strtok(NULL, " \n\t$");
                        while(tokens[token_no] != NULL) {
                            printf("%s\n", getenv(tokens[token_no]));
                            dest[token_no - 1] = getenv(tokens[token_no]);
                            tokens[token_no] = NULL;
                        }
                        token_no++;
                        tmp++;
                        while(tmp < 511 && userinput[tmp] != '$') tmp++;
                        //found third $
                        if(tmp <511){
                            tokens[token_no] = strtok(NULL, " \n\t$");
                            while(tokens[token_no] != NULL) {
                                printf("%s\n", getenv(tokens[token_no]));
                                dest[token_no - 1] = getenv(tokens[token_no]);
                                tokens[token_no] = NULL;
                            }
                        token_no++;
                        }
                    }
                }
        // no $ found
        tokens[token_no] = strtok(NULL, " \n\t&");
        while(tokens[token_no] != NULL){
            token_no++;
            tokens[token_no] = strtok(NULL, " \n\t&");
        }
        //for(int x = 0; x < 7; x++) printf("dest = %s\n", dest[x]);
        //for(int x = 0; x < 5; x++) printf("tokens = %s\n", tokens[x]);
        commands(argc, tokens, dest);
        memset(tokens, 0, 64);
        memset(dest, 0, 64);
        memset(userinput, 0, 512);
        i = 0;
        tmp = 0;
        //for(int y = 0; y < 10; y++)printf("userinput = %c\n", userinput[y]);
        }
    }
    return 0;
}
