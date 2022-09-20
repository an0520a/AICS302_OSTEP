// gcc -o wish wish.c -Wall -Werror
#include <stdio.h>
#include <stdlib.h> // exit / malloc
#include <unistd.h> // STDERR_FILENO / chdir / access / frok / dup2
#include <stdbool.h> // true, false
#include <string.h> // strsep / strcpy / strcat / strchr
#include <sys/wait.h> // wait
#include <fcntl.h> // open
#include <memory.h> // memomove

// Global Variable
char *shell_path[100]; // Path Array
int path_counter; // The number of Path
char *argv_[100]; // Argv

// Function
void shell_error()
{
    char error_message[30] = "An error has occurred\n";
    write(STDERR_FILENO, error_message, strlen(error_message)); 

    return ;
}

bool check_built_in(char *command)
{
    if(strcmp(command,"exit") == 0)
    {
        return true;
    }
    else if(strcmp(command,"cd") == 0)
    {
        return true;
    }
    else if(strcmp(command,"path") == 0)
    {
        return true;
    }

    return false;
}

void built_in(char *command, char *path)
{        
    //char *input;
    char *tmp_path;
    int tmpLen;

    // Command : exit
    if(strcmp(command,"exit") == 0)
    {
        // Only exit
        if(path == NULL)
        {
            exit(0);
        }
        else
        {
            shell_error();
            return ;
        }
    }
    // Command : cd
    else if(strcmp(command,"cd") == 0)
    {
        // No path Argument
        if(path == NULL)
        {
            shell_error();
            return ;
        }

        // Chdir None Path Error
        if(chdir(path) != 0)
        {
            shell_error();
            return ;
        }
    }
    // Command : path
    else 
    {
        // No path Argument ( Reset Path )
        if(path == NULL)
        {
            int i;
            for(i=0; i<path_counter;i++)
            {
                shell_path[i] = NULL;
            }
            i = 0;
            return ;
        }
        else
        {
            // Add Space
            tmpLen = strlen(path);
            path[tmpLen] = ' ';

            while(true)
            {
                // Extract Paths
                tmp_path = strsep(&path," ");

                // Add Path
                shell_path[path_counter] = tmp_path;
                path_counter++;

                // Finish
                if(strcmp(path,"") == 0)
                {
                    return ;
                }
            }
        }
    }

    return ;
}

void remover(char **input, char ch,bool front)
{
    int tmpLen;
    int i;

    // Cannot strlen
    if(*input == NULL)
        return ;

    if(front == true)
    {
        i = 0;
        while((*input)[i] == ch && (*input)[i] != '\n') 
        {
            i++;
        }
        tmpLen = strlen(*input);
        memmove(*input, (*input) + i, tmpLen - i);
        (*input)[tmpLen - i] = '\0';
    }
    else
    {
        i = 1;
        tmpLen = strlen((*input));
        
        while(( (*input)[tmpLen - i] == ch ))
        {
            (*input)[tmpLen - i] = '\0';
            i++;
        }
    }
    
    return ;
}

char* command_strcat(char *path,char *command)
{   
    char *program = malloc(strlen(path) + strlen(command) + 1);

    strcpy(program,path);
    strcat(program,"/");
    strcat(program,command);
    
    return program;
}

void set_argv(char *origin)
{
    char *tmpstr;
    int i = 0;

    while(true)
    {
        tmpstr = strsep(&origin," ");
        argv_[i] = tmpstr;
        i++;

        // Finish
        if(origin == NULL)
        {
            return ;
        }
    }
}

char* parser(char **origin,char *sperator)
{
    char *parsed;

    // Parsing
    parsed = strsep(origin,sperator);

    // Remove Space
    remover(&parsed,' ',false);
    remover(origin,' ',true);

    return parsed;
}


int main(int argc, char **argv)
{
    FILE *mode = stdin; // Interactive(stdin) / Batch Mode(fopen)
    
    // Add Initial Path
    shell_path[path_counter] = "/bin";
    path_counter++;

    // Batch File Argument Check
    if(argc > 2)
    {
        shell_error();
        exit(1);
    }
    else if(argc == 2)
    {
        if((mode = fopen(argv[1],"r")) == NULL)
        {
            shell_error();
            exit(1);
        }
    }

    // Command Struct
    char * command_line = NULL; // Raw Input
    char *tmp_input = NULL;     // After '&' Parsing
    char *input = NULL;         // After '>' Parsing
    char *command = NULL;       // Only Command
    char *program = NULL;       // Path + Command
    
    size_t len = 0 ;            // Command Line Len
    bool command_error = true;  // Error Flag
    bool redirection_flag = false; // Redirection Flag
    int fd;                     // Redirection Stream
    bool waitFlag = false;

    // Start Prompt
    while(true)
    {
        // No Batch mode
        if( mode == stdin )
        {
            printf("wish> ");
        }

        if((getline(&command_line, &len, mode) == -1))
        {
            exit(0);
        }

        // Remove Space
        remover(&command_line,' ',true);
        if(*command_line == '\n')
        {
            continue;
        }
            
        // Pasing Start
        while(true)
        {
            // Parallel Parsing
            tmp_input = (char *)NULL;
            tmp_input = parser(&command_line,"&");
            if(tmp_input == (char *)NULL || *tmp_input == '\0')
            {
                break;
            }
                
            // Redirection Parsing
            input = parser(&tmp_input,">");
            if( (*input) == '\0' ) // No Redirection Source
            {
                shell_error();
                break;
            }
                
            // Output File(tmp_input) Check
            if( tmp_input != NULL)
            {
                if(strcmp(tmp_input,"\n") == 0) // No File
                {
                    shell_error();
                    break;
                }
                else if(strchr(tmp_input,' ') != NULL) // Redirection Many Output File
                {
                    shell_error();
                    break;
                }
                else
                {
                    // Change Ouput Stream
                    redirection_flag = true;
                    remover(&tmp_input,'\n',false);

                }
            }

            //-- tmp_input -> input --//

            // Save Origin for execv
            remover(&input,'\n',false);
            char *origin = malloc(strlen(input) + 1);
            strcpy(origin,input);

            // Extract Command
            command = strsep(&input," ");

            remover(&command,'\n',false);
            command_error = true;

            // Built-in Command
            if(check_built_in(command) == true)
            {
                built_in(command,input);
            }
            // External Command
            else
            {
                // Run Program
                for( int i = 0 ; i < path_counter; i++ )
                {
                    if(shell_path[i] == NULL)
                    {
                        command_error = true;
                        break;
                    }   
                        
                    program = command_strcat(shell_path[i],command); // Make Command Path

                    // External Program Path Check
                    if(access(program,X_OK) == 0)
                    {
                        command_error = false;

                        int rc = fork();
                        if(rc > 0)
                        {
                            waitFlag = true;
                        }
                        else if(rc == 0)
                        {   
                            if(redirection_flag)
                            {
                                fd = open(tmp_input, O_CREAT | O_TRUNC | O_WRONLY);
                                dup2(fd, STDOUT_FILENO);
                                dup2(fd, STDERR_FILENO);
                            }
                            
                            int tmpLen = strlen(origin);
                            for (int i = tmpLen - 1; i >= 0; i--)
                            {
                                if(origin[i] == ' ') 
                                {
                                    origin[i] = '\0';
                                }
                                else
                                {
                                    break;
                                }
                            }
                            
                            set_argv(origin);      // Set argv_                         
                            execv(program, argv_); // Child : Program Run
                        }
                        else
                        {
                            shell_error();
                            exit(0);
                        }

                        free(program);
                        break;
                    }
                    free(program); // Free Malloc   
                } 

                // Nothing Run
                if(command_error)
                {
                    shell_error();  
                }               
            }

            // STDOUT_FILENO Recovery for Redirection
            if(redirection_flag == true)
            {
                close(fd);
                dup2(STDOUT_FILENO,1);
                dup2(STDERR_FILENO,2);
            }

            // Run another Command
            if(command_line != NULL)
            {
                continue;
            }

            // Go to Prompt
            break;
        }

        if(waitFlag)
        {
            while(wait(NULL) > 0);
            waitFlag = false;
        }

    }  
        
    return 0;
}