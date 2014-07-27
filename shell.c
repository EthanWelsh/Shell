#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/times.h>

#define SIZE 512
char input[SIZE];
char delim[30] = " \t ( ) | & ; \n    ";

int main()
{
    while (1)
    {
        printf("<shell> ");
        fflush(stdout);
        fgets(input, sizeof(input), stdin); // Get input

        char *args[SIZE];  // Create an array of character pointers.
        args[0] = strtok(input, delim); // Put the first token (the command) into args[0]

        if (args[0] == NULL) continue; // If they entered nothing...

        for (int i = 1; i < SIZE; i++) // Strtok everything.
        {
            args[i] = strtok(NULL, delim);
            if (args[i] == NULL) break; // If they didn't enter anything, do nothing.
        }

        if (strcmp(args[0], "cd") == 0)
        {
            if(args[1] == NULL) printf("Please enter a file or directory.\n");
            else if (chdir(args[1]) == -1) printf("No such file or directory\n");
        }
        else if (strcmp(args[0], "exit") == 0)
        {
            exit(0);
        }
        else
        {
            if (strcmp(args[0], "time") == 0)
            {
                struct tms before;
                if(times(&before) == -1) printf("Houston, we have a problem.\n");


                int f = fork();

                if (f == 0) execvp(args[1], (args + 1));
                else if(f < 0)
                {
                    printf("There was an error launching this process. Exiting shell.\n");
                    exit(-1);
                }
                else
                {
                    int status;
                    wait(&status);

                    struct tms after;
                    if(times(&after) == -1)
                    {
                        printf("Houston, we have a problem!\n");
                    }
                    else
                    {
                        printf("User Time: %d\n", after.tms_utime - before.tms_utime);
                        printf("System Time: %d\n", after.tms_stime - before.tms_stime);
                    }
                }
            }
            else
            {

                int f = fork();

                if (f == 0)
                {
                    for(int i = 0; i < SIZE; i++)
                    {
                        if(args[i] == NULL) break;
                        if(strcmp(args[i], ">") == 0)
                        {
                            args[i] = strtok(args[i], ">");
                            if(args[i+1] == NULL) printf("Did not recognize file ''. Please try again.\n");
                            freopen(args[i+1], "w+", stdout);
                            if(execvp(args[0], args) < 0)
                            {
                                printf("Error. Could not execute that command.\n");
                                exit(0);
                            }
                        }
                        else if (strcmp(args[i], ">>") == 0)
                        {
                            args[i] = strtok(args[i], ">>");
                            if(args[i+1] == NULL) printf("Did not recognize file ''. Please try again.\n");
                            if(freopen(args[i+1], "a", stdout) < 0) printf("Error with that.\n");
                            if(execvp(args[0], args) < 0)
                            {
                                printf("Error. Could not execute that command.\n");
                                exit(0);
                            }


                        }
                    }
                    if(execvp(args[0], args) < 0) printf("Error. Could not execute that command.\n");
                }
                else if(f < 0)
                {
                    printf("Error launching process. Please try again later.");
                }
                else
                {
                    int status;
                    wait(&status);
                }
            }
        }
    }
    return 0;
}
