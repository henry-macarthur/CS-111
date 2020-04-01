/*
    NAME: HENRY MACARTHUR
    EMAIL: HENRYMAC16@GMAIL.COM
    ID: 705096169
*/
#include <getopt.h>
#include <termios.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <netdb.h>
#include <string.h>
#include <poll.h>
#include <ulimit.h>
#include "zlib.h"
#include <assert.h>

// #if defined(MSDOS) || defined(OS2) || defined(WIN32) || defined(__CYGWIN__)
// #  include <fcntl.h>
// #  include <io.h>
// #  define SET_BINARY_MODE(file) setmode(fileno(file), O_BINARY)
// #else
// #  define SET_BINARY_MODE(file)
// #endif

#define CHUNK 16384

z_stream defstream;
z_stream infstream;

struct termios saved_attributes;
struct termios updated_attributes;
int log_file_descriptor;
char * log_name = NULL;

char sent_message[] = "SENT "; //5
char received_message[] = "RECEIVED "; //9
char bytes_message[] = " BYTES: "; //7

int compress_flag = 0;
int CHUNK_2 = 200;
char buff1[300]; //[1024];
char send_comp[300]; //= malloc(1024); //[1024];

int read_from_keyboard(int socket_id);
int read_from_socket(int socket_id);

void raise_err()
{
    fprintf(stderr, "err: %s", strerror(errno));
    exit(1);
}

void reset_input_mode()
{
    
    tcsetattr (STDIN_FILENO, TCSANOW, &saved_attributes);
}

void copy_arry (char * a1, char * a2, int s2) //a2 is 
{ //copy into the array
    int i = 0;
    for(; i < s2; i++)
    {
        a1[i] = a2[i];
    }
}

void write_to_console(int amount_read, char * buff)
{
    int i = 0;
    for(; i < amount_read; i++)
    { //buff
        if(buff[i] == '\r' || buff[i] == '\n')
        {
            char output[2];
            output[0] = '\r';
            output[1] = '\n';
            //buff?
            if(write(1, &output, 2) < 0)
                raise_err();
        } //change this
        else if(buff[i] == 0x03)
        {
            char output[2] = {'^', 'C'};
            if(write(1, output, 2) < 0)
                raise_err();
        } //change this
        else if(buff[i] == 0x04)
        {
            char output[2] = {'^', 'D'};
            if(write(1, output, 2) < 0)
                raise_err();
        }
        else
        {
            if(write(1, &buff[i], 1) < 0)
                raise_err();
        }
    }
}

void init_comp()
{
    defstream.zalloc = Z_NULL;
    defstream.zfree = Z_NULL;
    defstream.opaque = Z_NULL;
    deflateInit(&defstream, Z_BEST_COMPRESSION);

   
}

int comp_data(int input_size, int output_size, char * input, char * output)
{
    init_comp();
    //printf("%d \n", size);
    // defstream.zalloc = Z_NULL;
    // defstream.zfree = Z_NULL;
    // defstream.opaque = Z_NULL;
    defstream.avail_in = (uInt)input_size;//+1; 
    defstream.next_in = (Bytef *)input; 
    defstream.avail_out = (uInt)output_size;
    defstream.next_out = (Bytef *)(output); 
    

    
    deflate(&defstream, Z_SYNC_FLUSH);//Z_FINISH);
    deflateEnd(&defstream);
   
    return (int)defstream.total_out; //return the size of the new compressed buffer
}

int decomp_data(int in_sz, char * comp, char * decomp)
{

    infstream.zalloc = Z_NULL;
    infstream.zfree = Z_NULL;
    infstream.opaque = Z_NULL;
    inflateInit(&infstream);
    // printf(" .... %d ....", in_sz);
    int BF = 300;
    
    infstream.avail_in = (in_sz);
    infstream.next_in = (Bytef *)comp;
    infstream.next_out = (unsigned char *) decomp;
    infstream.avail_out = BF;
     while(infstream.avail_in > 0)
     {
     //do
     //{
          
          int rc = inflate(&infstream, Z_SYNC_FLUSH);
          if (rc < 0) {
              fprintf(stderr, infstream.msg);
              exit(0);
          }

          //write(1, decomp, infstream.total_out);
          //printf(" ///ran//// ");
          write_to_console(infstream.total_out, decomp);
          infstream.next_out = (unsigned char *) (decomp);
          //infstream.next_in = (Bytef *)comp;
          infstream.avail_out = BF;
          
    } //while (infstream.avail_out == 0);
    
     inflateEnd(&infstream);
    return BF - infstream.avail_out;//(int)infstream.total_out;
}

int main(int argc, char ** argv)
{


    char * host_name = "localhost";
    int number_of_arguments = argc;
    int option_index = 0;
    char * short_options = "";

    static struct option long_options[] = {
        {"port", required_argument, NULL, 'p'},
        {"log", required_argument, NULL, 'l'},
        {"compress", no_argument, NULL, 'c'},
        {0, 0, NULL, 0}
    };

    char * port_name = NULL;

    int cur;
    while( (cur = getopt_long(number_of_arguments, argv, short_options, long_options, &option_index)) != -1)
    {
        switch(cur)
        {
            case 'p':
                port_name = optarg;
                break;
            case 'l':
                log_name = optarg;
                log_file_descriptor = open(log_name, O_RDWR | O_CREAT | O_TRUNC, S_IRWXU); //| O_TRUNC
                if(log_file_descriptor < 0)
                {
                    raise_err();
                }

                break;
            case 'c':
                //init_comp();
                compress_flag = 1;
                break;
            default:
                fprintf(stderr, "unrecognized input! \n");
                exit(1);
        }
    }

    if(port_name == NULL)
    {
        fprintf(stderr, "missing the --port argument! \n");
        exit(1);
    }
    

    ulimit(UL_SETFSIZE, 10000);

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

    //open the socket
    struct hostent *server;
    struct sockaddr_in serv_addr;

    int port_num = atoi(port_name);

    int socketfd = socket(AF_INET, SOCK_STREAM, 0);
    if(socketfd < 0)
        raise_err(); //is this ok?
    server = gethostbyname(host_name);
    if(server == NULL)
    {
        fprintf(stderr, "unable to find host \n");
        exit(1);
    }

    bzero((char *) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    bcopy((char *)server->h_addr, (char *)&serv_addr.sin_addr.s_addr, server->h_length);
    serv_addr.sin_port = htons(port_num);

    if(connect(socketfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
    {
        raise_err();
    }


    struct  pollfd polls[2] =
    {
        {0, POLLIN | POLLERR | POLLHUP, 0},
        {socketfd, POLLIN | POLLERR | POLLHUP, 0}
    };

    while(1)
    {
        int rt_poll = poll(polls, 2, 0);

        if(rt_poll < 0)
        {
            fprintf(stderr, "error with polling: %s", strerror(errno));
            exit(1);
        }
        else
        {
            if((polls[0].revents&POLLIN) == POLLIN) //key
            {
                read_from_keyboard(socketfd);
            }
            if((polls[1].revents&POLLIN) == POLLIN) //serv
            {
                read_from_socket(socketfd);
            }
            if((polls[0].revents&(POLLERR | POLLHUP))) //key
            {

            }
            if((polls[1].revents&(POLLERR | POLLHUP))) //serv
            {
                exit(1);
            }
        }
    }



    return 0;
}

int read_from_keyboard(int socket_id)
{
    char cu[5];
    //int br = 0;
    int amount_read = read(0, &cu, 5);
    char test_comp[10];

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
        else if(cu[i] == 0x03)
        {
            char output[2] = {'^', 'C'};
            if(write(1, output, 2) < 0)
                raise_err();
            break;
        }
        else if(cu[i] == 0x04)
        {
            char output[2] = {'^', 'D'};
            if(write(1, output, 2) < 0)
                raise_err();
            break;
        }
        else
        {
            int rd = write(1, &cu[i], 1);
            if(rd < 0)
                raise_err();
        }
    }
    //i think i can just write the entire block of data read to the server
    if(compress_flag)
    {
        int comp_size = comp_data(amount_read, 10, cu, test_comp); 
        if(write(socket_id, test_comp, comp_size) < 0)
            raise_err();
        copy_arry(cu, test_comp, comp_size);
    }
    else
    {
        if(write(socket_id, cu, amount_read) < 0)
            raise_err();
    }
    

    if(log_name != NULL)
    {
        char num[4] = {'\0'};
        sprintf(num, "%d", amount_read);
        write(log_file_descriptor, sent_message, 5); //SENT
        write(log_file_descriptor, num, strlen(num));
        write(log_file_descriptor, bytes_message, 8); // _BYTES:_
        write(log_file_descriptor, cu, amount_read);
        write(log_file_descriptor, "\n", 1);
    }    
    return 0;
}

int read_from_socket(int socket_id)
{

    int amount_read = read(socket_id, buff1, CHUNK_2);
    
    if(amount_read < 0)
        raise_err();
    if(amount_read == 0)
        exit(0);

    if(log_name != NULL)
    {
        char num[4] = {'\0'};
        sprintf(num, "%d", amount_read);
        write(log_file_descriptor, received_message, 9); //SENT
        write(log_file_descriptor, num, strlen(num));
        write(log_file_descriptor, &bytes_message, 8); // _BYTES:_
        write(log_file_descriptor, buff1, amount_read);
        write(log_file_descriptor, "\n", 1);
    }
    if(compress_flag)
    {
        int s_z = decomp_data(amount_read, buff1, send_comp);
        copy_arry(buff1, send_comp, s_z);
        amount_read = s_z;
    }
    else
    {
        write_to_console(amount_read, buff1);
    }  
    return 0;
}