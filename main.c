//Kyle Lessnau
//Shell program in C
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdio.h>
#include <signal.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <stdarg.h>
#define SIZE 1024

//Function protoype to parse command line
char **tokenizeCommand(char *);

//Function protoype to parse path
char **tokenizePath(char *, int *);

//Function prototype to execute commands
void **executeCommands(char **, char **, int);

pid_t pid;


void sig_handler(int sig)
{
    printf("\n\nPARENT caught signal [%d]\n\n", sig);
    if(sig == SIGINT)
        signal(SIGINT, sig_handler);
}

//Main function
int main(int argc, char **argv, char **envp)
{
    signal(SIGINT, sig_handler);
    int pathSize;
    char cmd[SIZE];
    int *pSize = malloc(sizeof(int));
    char *prompt;
    char *environment = malloc(sizeof(char *) * SIZE);
    char **cmdLine;
    char **path;
    //Sets path
    if(!getenv("MYPATH"))
        environment = getenv("PATH");
    else
        environment = getenv("MYPATH");
    if(!getenv("PS1"))
        prompt = ">>> ";
    else
        prompt =  getenv("PS1");
    
    //Parses file path and sets size of the array
    path = tokenizePath(environment, pSize);
    pathSize = *pSize;
    
    /*
     Test to print size of path and path
     printf("%d\n", pathSize);
     printf("%s%s%s%s", path[0], path[1], path[2], path[3]);
     */
    
    //While loop continues to execute command until program is exited
    while(1)
    {
        printf("%s", prompt);
        
        //User enters command. Ctrl+D registers EOF and program terminates if entered
        if(gets(cmd) == NULL)
            exit(0);
        //Searches for empty command
        if(strcmp(cmd, "") == 0)
            continue;
        else
        {
            //Function call to parse cmd
            cmdLine = tokenizeCommand(cmd);
            
            //Searches for exit in command line
            if(strcmp(cmdLine[0], "exit") == 0)
                exit(0);
            
            //Function call to search and execute commands
            executeCommands(cmdLine, path, pathSize);
        }
    }
    return 0;
}

//Function to parse command line.
char **tokenizeCommand(char *command)
{
    int i = 0;
    char **commArg = malloc(sizeof(char*) * SIZE);
    char *token;
    //Error handling for malloc
    if(!commArg)
    {
        printf("Error allocating memory");
        exit(1);
    }
    //Strtok parses command line into substrings
    token = strtok(command, " ");
    while(token != NULL)
    {
        commArg[i] = token;
        //Prints to make sure proper command tokens exist
        //printf("%s ", token);
        i++;
        token = strtok(NULL, " ");
    }
    //Returned to main
    return commArg;
}

//Function to parse path.
char **tokenizePath(char *pathLine, int *pSize)
{
    //Counter variable also used to find pathSize
    int i = 0;
    char **pathArg = malloc(sizeof(char*) * SIZE);
    char *token;
    //Error handling for malloc
    if(!pathArg)
    {
        printf("Error allocating memory");
        exit(1);
    }
    //Strtok parses path into substrings
    token = strtok(pathLine, ":");
    while(token != NULL)
    {
        pathArg[i] = token;
        //Prints to make sure proper path tokens exist
        //printf("%s\n", token);
        i++;
        token = strtok(NULL, ":");
    }
    *pSize = i;
    //Returned to main
    return pathArg;
}

void **executeCommands(char **commandLine, char **pathLine, int pathLineSize)
{
    int i = 0;
    int status = 1;
    struct stat buffer;
    char dir[SIZE];
    char *comm = malloc(sizeof(char*) * SIZE);
    getcwd(dir, sizeof(dir));
    //for loop traverses each directory in path
    for(i = 0; i < pathLineSize; i++)
    {
        //changes directory to current count
        chdir(pathLine[i]);
        //If command found
        if(stat(commandLine[0], &buffer) >= 0)
        {
            if(buffer.st_mode & S_IXUSR)
            {
                strcpy(comm, pathLine[i]);
                strcat(comm, "/");
                strcat(comm, commandLine[0]);
                chdir(dir);
                pid = fork();
                if(pid == 0)
                {
                    execvp(comm, commandLine);
                }
                else
                {
                    waitpid(pid, &status, 0);
                    break;
                }
            }
        }
        //If command not found
        if(i == (pathLineSize - 1))
        {
            printf("%s is an invalid command\n", commandLine[0]);
            chdir(dir);
        }
    }
    return 0;
}
