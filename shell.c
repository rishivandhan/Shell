//note: this code can be only run in an UNIX Based System. -- #include <sys/wait.h> is UNIX based code.


#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <sys/wait.h> 


#define BUFFSIZE 1024 * 1024 // sets the size of the buffer to 1024 *1024

void initiateDirectory(); // method that initiate the starting of the shell command

int fArgument(char buffer[], char *argv[]); // converts and formats the argument.

int main()
{

    char *argv[120];                                   // argument buffer
    int argc;                                          // argc
    char buffer[BUFFSIZE];                             // stores the buffze as the size of the array buffer
    size_t bingo = sizeof(buffer) / sizeof(buffer[0]); // calculates the size of the iput

    chdir(getenv("HOME")); // switches to home directory before shell initially prompts

    // while loop that starts the process.
    while (1)
    {

        initiateDirectory(); // method called here

        memset(buffer, '\0', bingo); // this method resets the input buffer
        int inputter;
        if ((inputter = read(STDIN_FILENO, buffer, BUFFSIZE)) == -1)
        { // this method reads through the user input.
            perror("Cannot read input\n");
        } // if
        if (inputter < 2)
        { // this checks to see if there are no arguments passed
            if (inputter < 1)
            { // this if statements check to see if ctlr - D clicked
                printf("\n");
                exit(0);
            } // if
            continue;
        } // if

        argc = fArgument(buffer, argv); // Changes inoput into arguments that can be executed

        if (strcmp(argv[0], "exit") == 0)
        { // if statement that checks to see if there is an "exit " in the cmd
            exit(0);
        } // if

        // if statement that checks to see if there is a cd in the command line argument
        if (strcmp(argv[0], "cd") == 0)
        {
            if (argc == 1)
            {
                if (chdir(getenv("HOME")) == -1)
                {
                    perror("Error changing directory");
                } // if
            }
            else
            {

                char cwd2[100];
                char *gdir = getcwd(cwd2, sizeof(cwd2));
                char *dir = strcat(gdir, "/");
                char *full = strcat(dir, argv[1]);
                if (chdir(full) == -1)
                {
                    perror("Error changing directory");
                } // if
            }     // if
            continue;
        } // if

        // fork started here
        pid_t launchProcess;
        int execute;

        launchProcess = fork();
        if (launchProcess == 0)
        {
            // if statements for redirecting files
            for (int i = 0; i < argc - 1; i++)
            { // for loop that searches for "<", ">", or ">>"
                if (strcmp(argv[i], "<") == 0)
                {                   // if statement  that check to see if the statement if "<" is passed
                    argv[i] = NULL; // ignores "<" in cmd args
                    int fd0;
                    // opens file descriptor
                    if ((fd0 = open(argv[i + 1], O_RDONLY)) < 0)
                    {
                        perror("Couldn't open input file");
                        exit(0);
                    } // if
                    dup2(fd0, STDIN_FILENO);
                    close(fd0);
                }
                else if (strcmp(argv[i], ">") == 0)
                {                   // check to see if cmd if ">" is passed
                    argv[i] = NULL; // ignores ">" in cmd args
                    int fd1;
                    // opens file descriptor
                    if ((fd1 = open(argv[i + 1], O_CREAT | O_WRONLY | O_TRUNC, 0666)) < 0)
                    {
                        perror("Couldn't open the output file");
                        exit(0);
                    }                         // if
                    dup2(fd1, STDOUT_FILENO); // redirects stdout and writes to file descriptor
                    close(fd1);               // closes file descriptor
                }
                else if (strcmp(argv[i], ">>") == 0)
                {                   // if ">>" is passed
                    argv[i] = NULL; // ignores ">>" in cmd args
                    int fd2;
                    // opens file descriptor
                    if ((fd2 = open(argv[i + 1], O_CREAT | O_WRONLY | O_APPEND, 0666)) < 0)
                    {
                        perror("Couldn't open the output file");
                        exit(0);
                    }                         // if
                    dup2(fd2, STDOUT_FILENO); // redirects stdout and appends to file descriptor
                    close(fd2);               // closes file descriptor
                }                             // if
            }                                 // for

            if (execvp(argv[0], argv) == -1)
            {
                perror("Unknown command"); // error checking
            }                              // if
            exit(EXIT_FAILURE);
        }
        else if (launchProcess > 0)
        {
            while (!(wait(&execute) == launchProcess))
                ; // parent waits for child to execute
        }
        else
        {
            perror("Error forking"); // error checking on the forking
            exit(EXIT_FAILURE);
        } // if
    }     // while
    return 0;
} // main

int fArgument(char buffer[], char *argv[])
{
    int stringerton = strlen(buffer);
    if (buffer[stringerton - 1] == '\n')
    { // sets the last char to string terminating char
        buffer[stringerton - 1] = '\0';
    } // if

    // separates each input arg into char* to add to argv
    char *token;
    token = strtok(buffer, " ");
    int argc = 0;

    while (token != NULL)
    {
        argv[argc] = token;
        token = strtok(NULL, " ");
        argc++;        // increments arg count
    }                  // while
    argv[argc] = NULL; // sets last item of argv to NULL
    return argc;       // returns number of args
} // ending for fArgument

/**
 * This method prints the modifies current working directory instead of printing userid, "~" is printed instead.
 */
void initiateDirectory()
{
    // gets current working directory
    char cwd[100];
    size_t size = sizeof(cwd) / sizeof(cwd[0]);
    memset(cwd, '\0', size);
    getcwd(cwd, size);
    int slash = 0;
    int start = 0;
    for (int i = 0; i < size; i++)
    { // for loop to find the location of userid
        if (cwd[i] == '/')
        {
            slash++;
            if (slash == 4)
            {
                start = i;
            } // if
        }     // if
    }         // for
    setbuf(stdout, NULL);
    printf("1730sh:");
    if (slash < 3)
    { // this if statement prints directory prior to userid
        setbuf(stdout, NULL);
        printf("%s$ ", cwd);
    }
    else if (slash == 3)
    { // this else if statement prints directory at userid with ~ instead
        setbuf(stdout, NULL);
        printf("~$ ");
    }
    else
    { // This else statement inserts ~ instead of userid along with following directories
        char subcwd[sizeof(cwd) - start];
        memcpy(subcwd, &cwd[start], sizeof(subcwd) - 1);
        subcwd[sizeof(subcwd) - 1] = '\0';
        setbuf(stdout, NULL);
        printf("~%s$ ", subcwd);
    } // if
} // initiateDirectory