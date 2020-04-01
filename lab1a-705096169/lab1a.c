//#define _POSIX_SOURCE
#include <termios.h>
#include <unistd.h>
#include <getopt.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>
#include <errno.h>
#include <poll.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>

struct termios saved_attributes;
struct termios updated_attributes;
int read_from_shell();
int read_from_keyboard(int write_fd);
int exit_status;
int stat; 
int pipefd1[2];
int pipefd2[2];
int to_shell = 1;
int stop_keyboard = 0;
int break_from_loop = 0;
int terminal_open = 1;
int child;

void sig_pipe_handle(int signal)
{
    if(signal == SIGPIPE)
    {
        to_shell = 0;
        break_from_loop = 1;
    }
}
void raise_err()
{
    fprintf(stderr, "err: %s", strerror(errno));
    exit(1);
}
int read_from_shell()
{
    //have the proper file descriptors mapped to in out and err!
    //just use 1, 0, 2
    //0 comes from the terminal
    //1 or 2 will write to terminal out or err due to piping
    char c[256];
    int buffer_size = 256; 
    int br = 0;

    int amount_read = read(pipefd1[0], &c, buffer_size);
    if(amount_read < 0)
    {
        raise_err();
    }
    if(amount_read == 0) //&& !terminal_open)
        return 1;

    int i = 0;
    for(; i < amount_read; i++)
    {
        if(c[i] == '\r' || c[i] == '\n') //only ls should come through but can keep this to play it safe
        {
            char output[2];
            output[0] = '\r';
            output[1] = '\n';
            int rd = write(1, &output, 2);
            if(rd < 0)
                raise_err();
        }
        else if(c[i] == '\004')
        {
            //char output[2];
            break_from_loop = 1;
            //this means that I can exit input as it is C^D but not sure how to do so
            br = 1;
            break;
        }
        else
        {
            int rd = write(1, &c[i], 1);
            if(rd < 0)
                raise_err();
        } 
    }
    return br; //
}
int read_from_keyboard(int write_fd)
{
    char c[5];
    int buffer_size = 5; 
    int br = 0;

    int amount_read = read(0, &c, buffer_size); //read from standard input as fd's are mapped propp[erly]
    if(amount_read < 0)
        raise_err();
    
    int i = 0;
    for(; i < amount_read; i++)
    {
        if(c[i] == '\r' || c[i] == '\n') //only ls should come through but can keep this to play it safe
        {
            char output[2];
            output[0] = '\r';
            output[1] = '\n';
            int rd;
            rd = write(1, &output, 2);
            if(rd < 0)
                raise_err();
            if(to_shell) //to_shell
            {
                rd = write(write_fd, &output[1], 1); //send LS to the pipe destination(shell)
                if(rd < 0)
                    raise_err();
            }
        }
        else if(c[i] == 0x03)
        {
            
            char output[2] = {'^', 'C'};
            if(write(1, &output, 2) < 0)
            {
                raise_err();
            }

            int status = kill(child, SIGINT);

            if(status < 0)
            {
                fprintf(stderr, strerror(errno));
                exit(1);
            }
            terminal_open = 0;
            break;
        }
        else if(c[i] == '\004')
        {
            char output[2] = {'^', 'D'};
            if(write(1, &output, 2) < 0)
            {
                raise_err();
            }
            //terminal_open = 0;
            close(pipefd2[1]);
            br = 2;
            break;
        }
        else
        {
            int rd;
            rd = write(1, &c[i], 1);
            if(rd < 0)
               raise_err();
            if(to_shell)
            {
                rd = write(write_fd, &c[i], 1);
                if(rd < 0)
                {
                    raise_err();
                }
            }
        }
    }
    return br; //1 done, 0 ok
}
void reset_input_mode()
{
    tcsetattr (STDIN_FILENO, TCSANOW, &saved_attributes);
}
int main (int argc, char ** argv)
{
    //set up the --shell flag
    int shell_flag = 0;
    int number_of_arguements = argc;
    int option_index = 0;
    char * short_options = "";
    static struct option long_options[] = {
        {"shell", no_argument, NULL, 's'},
        {0, 0, NULL, 0}
    };
    int cur;
    while((cur = getopt_long(number_of_arguements, argv, short_options, long_options, &option_index)) != -1)
    {
        switch(cur)
        {
            case 's':
                shell_flag = 1;
                break;
            default:
                fprintf(stderr, "unrecognized input!\n");
                exit(1);
        }
    }
    //=======================deal with changing terminal
    tcgetattr(STDIN_FILENO, &saved_attributes);
    atexit (reset_input_mode);
    //saved_attributes now containes all of the current terminal info 
    //now need to store a copy of the origional data
    updated_attributes = saved_attributes;

    updated_attributes.c_iflag = ISTRIP;
    updated_attributes.c_oflag = 0;
    updated_attributes.c_lflag = 0;
    //have terminal attributes change
    tcsetattr(STDIN_FILENO, TCSANOW, &updated_attributes);

    if(shell_flag)
    {
        
        int p1;
        int p2;
        p1 = pipe(pipefd1); //set up the pipe
        p2 = pipe(pipefd2);

        if(p1 < 0 || p2 < 0)
        {
            raise_err();
        }
        //parent can send data through fd2, recive through fd1
        //child can send data through fd1, recieve through fd2
        //0 read 1 out
        int rc = fork();
        child = rc;
        

        if(rc == 0) //child
        {
            
            close(pipefd2[1]);
            close(pipefd1[0]);
            close(0);
            dup(pipefd2[0]);
            close(pipefd2[0]);
            close(1);
            dup(pipefd1[1]);
            close(2);
            dup(pipefd1[1]);
            close(pipefd1[1]);
            char * args[2];
            args[0] = "/bin/bash";
            args[1] = NULL;
            execvp(args[0], args); 
        }
        else if( rc < 0)
        {
            fprintf(stderr, "fork failed %s", strerror(errno));
            exit(1);
        }
        else
        {
            signal(SIGPIPE, sig_pipe_handle);
            close(pipefd2[0]);
            close(pipefd1[1]);
            
            struct pollfd polls[2] = 
            {
                {0, POLLIN | POLLERR | POLLHUP, 0},
                {pipefd1[0], POLLIN | POLLERR | POLLHUP, 0}
            };            

            while(1)
            {
                int rt_poll = poll(polls, 2, 0);

                if(rt_poll < 0)
                {
                    fprintf(stderr, "error with polling: %s", strerror(errno));
                    exit(1);
                }
                else //keep polling if = 0, else go here
                {
                    if((polls[0].revents&POLLIN) == POLLIN) //keyboard
                    {
                        
                        int status = read_from_keyboard(pipefd2[1]); 
                        if(status == 1)
                            break_from_loop = status;
                        else if(status == 2) ///&& terminal_open)
                        {
                            stop_keyboard = 1;
                            terminal_open = 0;
                        }
                    }   
                    if((polls[1].revents&POLLIN) == POLLIN) 
                    {
                        int status = read_from_shell();
                        break_from_loop = status;
                    }
                    if((polls[0].revents&(POLLERR | POLLHUP)))
                    {
                        if(stop_keyboard)
                            break_from_loop = 1;
                    }
                    if((polls[1].revents&(POLLERR | POLLHUP)))
                    {
                        break_from_loop = 1;
                        // if(terminal_open == 0 && to_shell)
                        // {
                        //     to_shell=0;
                        // }
                        // terminal_open = 0;
                        // if(stop_keyboard)
                        //     break_from_loop = 1;
                        //time to leave!
                    }
                }
                if(break_from_loop)
                {
                    break;
                }
            }
            stat = waitpid(rc, &exit_status, 0);
            
            if(stat < 0)
            {
                raise_err();
            }
            int final_signal = exit_status & 0x007f;
            int final_status = WEXITSTATUS(exit_status);//exit_status & 0xff00;

            fprintf(stderr, "SHELL EXIT SIGNAL=%d STATUS=%d", final_signal, final_status);
            exit(0);
        }
    }
    else
    {
        char cu[5];
        while(1)
        {
            int br = 0;
            int amount_read = read(0, &cu, 5);
            if(amount_read < 0)
                raise_err();
            int i = 0;
            for(; i < amount_read; i++)
            {
                if(cu[i] == '\r' || cu[i] == '\n')
                {
                    char output[2];
                    output[0] = '\r';
                    output[1] = '\n';
                    int rd = write(1, &output, 2);
                    if(rd < 0)
                        raise_err();
                }
                else if(cu[i] == '\004')
                {
                    br = 1;
                    break;
                }
                else
                {
                    int rd = write(1, &cu[i], 1);
                    if(rd < 0)
                        raise_err();
                }
            }
            if(br)
                break;
        }
    }
    return 0;  
}