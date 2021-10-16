// PID: 730334594
// I pledge the COMP211 honor code.
// ----------------------------------------------
// These are the only libraries that can be 
// used. Under no circumstances can additional 
// libraries be included.
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <string.h>
#include "shell.h"

const char* valid_builtin_commands[] = {"cd", "exit", NULL};


void parse( char* line, command_t* p_cmd ) {

    //Status is getting parsed through
    char *status = malloc(100*sizeof(char));
    status = strtok(line," \n");

    // size is the size of argc
    int size = 0;

    // cycling through arguments for argv
    while(status != NULL){

        // preventing segmentation fault
        if(p_cmd -> argv[size] == NULL){
            p_cmd -> argv[size] = malloc(sizeof(char)*100);
        }

        strcpy(p_cmd -> argv[size], status);
        size++;
        status = strtok(NULL," \n");
    }

    //Setting default values
    p_cmd -> argv[size] = NULL;
    strcpy(p_cmd -> path,p_cmd -> argv[0]);
    p_cmd -> argc = size;

    // checking for diffrent cases
    // checking if user just hits enter
    if(size == 0){

        p_cmd -> argc = size;;

        //checking if command is built-in
    } else if(is_builtin(p_cmd)){

        //preventing segmentation fault
        if(p_cmd -> argv[size] == NULL){
            p_cmd -> argv[size] = malloc(sizeof(char)*100);
        }

        strcpy(p_cmd -> path,p_cmd -> argv[0]);
        p_cmd -> argc = size;

        //checking for non-built in commands
    } else if(find_fullpath(p_cmd -> argv[0], p_cmd)){

        p_cmd -> argc = size;

    } else{

        //Checking non-existant commands
        //preventing segmentation faults
        if(p_cmd -> argv[size] == NULL){
            p_cmd -> argv[size] = malloc(sizeof(char)*100);
        }

        strcpy(p_cmd -> path,p_cmd -> argv[0]);;
        p_cmd -> argc = -1;

    }
    return;
} // end parse function


int find_fullpath( char* command_name, command_t* p_cmd ) {

    // Defining variables
    int exists = FALSE;
    char *path = getenv("PATH");
    char *fullpath = malloc(100*sizeof(path)+1);
    strcpy(fullpath,path);

    //Status is used to iterate through Fullpath
    char *status = strtok(strcpy(fullpath,path),":");

    //cycling through full path
    while(status != NULL){
        char *actualpath = malloc(sizeof(char)*100);
        strcpy(actualpath, status);
        strcat(actualpath,"/");
        strcat(actualpath,command_name);

        struct stat buffer;

        exists = stat(actualpath, &buffer);
        if ( exists == 0 && ( S_IFREG & buffer.st_mode ) ) {
            strcpy(p_cmd -> path,actualpath);
            return TRUE;
        }

        status = strtok(NULL,":");
    }
    return FALSE;
} // end find_fullpath function


int execute( command_t* p_cmd ) {
    //Define varibles 
    int status = SUCCESSFUL;
    int child_process_status;
    pid_t child_pid;

    child_pid = fork();

    if ( child_pid == 0 ) {

        execv(p_cmd -> path,p_cmd -> argv);

        perror("Execute terminated with an error condition!\n");
        child_process_status = ERROR;
        exit( 1 );
    }
    child_process_status = wait(NULL);

    //checking parent status
    if(child_process_status == ERROR){
        status = ERROR;
    }

    return status;

}

int is_builtin( command_t* p_cmd ) {

    int cnt = 0;

    while ( valid_builtin_commands[cnt] != NULL ) {

        if ( equals( p_cmd->path, valid_builtin_commands[cnt] ) ) {

            return TRUE;

        }

        cnt++;

    }

    return FALSE;

} // end is_builtin function


int do_builtin( command_t* p_cmd ) {

    // only builtin command is cd

    if ( DEBUG ) printf("[builtin] (%s,%d)\n", p_cmd->path, p_cmd->argc);

    struct stat buff;
    int status = ERROR;

    if ( p_cmd->argc == 1 ) {

        // -----------------------
        // cd with no arg
        // -----------------------
        // change working directory to that
        // specified in HOME environmental 
        // variable

        status = chdir( getenv("HOME") );

    } else if ( ( stat( p_cmd->argv[1], &buff ) == 0 && ( S_IFDIR & buff.st_mode ) ) ) {


        // -----------------------
        // cd with one arg 
        // -----------------------
        // only perform this operation if the requested
        // folder exists

        status = chdir( p_cmd->argv[1] );

    } 

    return status;

} // end do_builtin function



void cleanup( command_t* p_cmd ) {

    int i=0;

    while ( p_cmd->argv[i] != NULL ) {
        free( p_cmd->argv[i] );
        i++;
    }

    free( p_cmd->argv );
    free( p_cmd->path );	

} // end cleanup function


int equals( char* str1, const char* str2 ) {
    int len[] = {0,0};

    char* b_str1 = str1;
    const char* b_str2 = str2;

    while( (*str1) != '\0' ) { 
        len[0]++;
        str1++;
    }

    while( (*str2) != '\0' ) {
        len[1]++;
        str2++;
    }

    if ( len[0] != len[1] ) {

        return FALSE;

    } else {

        while ( (*b_str1) != '\0' ) {

            if ( tolower( (*b_str1)) != tolower((*b_str2)) ) {

                return FALSE;

            }

            b_str1++;
            b_str2++;

        }

    } 

    return TRUE;


} // end compare function definition
