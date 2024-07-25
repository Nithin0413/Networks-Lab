
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include <errno.h>
#include <pthread.h>


struct client 
{
    int id;
    char name[200];
};


void *recieving(void *arg)
{
    int n;
    struct client *client_info = (struct client *)arg;

    char temp_string[256];
    memset(temp_string,0,256);
    while(1)
    {
        int client_socket = client_info->id;
        n = read(client_socket, temp_string, 256);
        if(n==0)
        {
            printf("Connection lost\n");
            exit(0);
        }
        if (n < 0)
        {
            printf("Error: Writing Failed\n");
            printf("errno: %d\n", errno);
            return NULL;
        }
        printf("%s\n", temp_string);
    }
    pthread_exit(NULL);
}


int main()
{
    int sockfd, port_no, n;
    struct sockaddr_in serv_addr;
    struct client client_det;

    char string[256];
    pthread_t recv,send;

    sockfd = socket(AF_INET, SOCK_STREAM, 0);

    if (sockfd < 0) 
    {
        printf("Error: Socket Creating Failed\n");
        printf("errno: %d\n", errno);
        return 1;
    }
    memset((char *) &serv_addr, 0, sizeof(serv_addr));
    int x;
    printf("Enter the Port Number: ");
    scanf("%d",&x);
    port_no = x;

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(port_no);
    serv_addr.sin_addr.s_addr = htons(INADDR_ANY);

    if (connect(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) 
    {
        printf("Error: Socket Creating Failed\n");
        printf("errno: %d\n", errno);
        return 1;
    }

    memset(string,0,256);
    n = read(sockfd, string, 256);
    if (n < 0) 
    {
        printf("Error: Reading Failed\n");
        printf("errno: %d\n", errno);
        return 1;
    }

    client_det.id = sockfd;
    strcpy(client_det.name,string);

    printf("Client name: %s\n", string);

    // Recieving Data
    if (pthread_create(&send, NULL, recieving, (void *)&client_det) != 0) 
    {
        printf("Error: Creating Thread Failed - Receiving\n");
        printf("errno: %d\n", errno);
        return 1;
    }
        
    while(1)
    {

        // Sending Data
        int n;
        memset(string,0,256);
        fgets(string,100,stdin);
        if(string[0]=='\n')
            fgets(string,100,stdin);
        if(strlen(string)>0 && string[strlen(string)-1]=='\n')
            string[strlen(string)-1]='\0';
        if(strcmp(string,"/exit")==0 || strcmp(string,"/part")==0 || strcmp(string,"/quit")==0)
            break;
        n = write(sockfd, string, 256);
        if (n < 0)
        {
            printf("Error: Creating Thread Failed - Sending\n");
            printf("errno: %d\n", errno);
            return 1;
        }
    }
    n = write(sockfd, string, 256);
    if (n < 0)
    {
        printf("Error: Creating Thread Failed - Sending\n");
        printf("errno: %d\n", errno);
        return 1;
    }
    close(sockfd);
    return 0;
}