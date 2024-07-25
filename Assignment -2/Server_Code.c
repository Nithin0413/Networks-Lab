#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include <errno.h>
#include <signal.h>

struct client 
{
    int id;
    char name[200];
};

struct client clients[10];
int client_count=0;


void *handle_client(void *arg)
{
    int n;
    struct client *client_info = (struct client *)arg;

    char string[256];
    memset(string,0,256);
    int client_socket = client_info->id;
    char client_name [256];
    strcpy(client_name,client_info->name);
    n = write(client_socket, client_info->name, strlen(client_info->name));
    if (n < 0)
    {
        printf("Error: Writing Failed\n");
        printf("errno: %d\n", errno);
        return NULL;
    }
    while(1)
    {
        memset(string,0, 256);
        n = read(client_socket, string, 256);
        if (n < 0) 
        {
            printf("Error: Reading Failed\n");
            printf("errno: %d\n", errno);
            return NULL;
        }
        if(strcmp(string,"/exit")==0 || strcmp(string,"/part")==0 || strcmp(string,"/quit")==0)
        {
            char str[100];
            strcpy(str,client_name);
            char temp[100] = " has left the chat";
            strcat(str,temp);
            for(int i=0;i<client_count;i++)
            {
                if((clients[i].id)==client_socket)
                {
                    clients[i].id=-1;
                    continue;
                }
                if(clients[i].id==-1)
                    continue;
                n = write(clients[i].id, str, 256);
                if (n < 0)
                {
                    printf("Broadcasting failed for %s from %s\n",clients[i].name,client_info->name);
                    printf("Error: Writing Failed\n");
                    printf("errno: %d\n", errno);
                    return NULL;
                }
            }
            printf("%s\n",str);
            break;
        }
        printf("%s : %s\n", client_name, string);
        char new_string[256];
        strcpy(new_string,client_name);
        char tmp[100] = " : ";
        strcat(new_string,tmp);
        strcat(new_string,string);
        for(int i=0;i<client_count;i++)
        {
            if(clients[i].id==client_socket || clients[i].id==-1)
                continue;
            n = write(clients[i].id, new_string, 256);
            if (n < 0)
            {
                printf("Broadcasting failed for %s from %s",clients[i].name,client_name);
                printf("Error: Writing Failed\n");
                printf("errno: %d\n", errno);
                return NULL;
            }
        }
        
    }
    close(client_socket);
    pthread_exit(NULL);
}

int main()
{
    int client_socket, port_no,server_socket;
    socklen_t client_len;
    struct sockaddr_in server_addr, client_addr; 
    int x;
    printf("Enter the Port Number: ");
    scanf("%d",&x);
    port_no = x;

    char client_name[200];
    pthread_t threads[10];

    server_socket = socket(AF_INET, SOCK_STREAM, 0);

    if (server_socket < 0)
    {
        printf("Error: Socket Creating Failed\n");
        printf("errno: %d\n", errno);
        return 1;
    }

    memset((char *) &server_addr,0, sizeof(server_addr));

    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = htons(INADDR_ANY);
    server_addr.sin_port = htons(port_no);

    if (bind(server_socket, (struct sockaddr *) &server_addr, sizeof(server_addr)) < 0)
    {
        printf("Error: Binding Failed\n");
        printf("errno: %d\n", errno);
        return 1;
    }

    listen(server_socket, 10);

    client_len = sizeof(client_addr);

    for(int i=0;i<10;i++)
    {
        client_socket = accept(server_socket, (struct sockaddr *) &client_addr, &client_len);
        client_count++;
        if (client_socket < 0) 
        {
            printf("Error: Accepting Connection Failed\n");
            printf("errno: %d\n", errno);
            return 1;
        }
        memset(client_name,0, 200);
        sprintf(client_name, "Client - %d", i+1);
        printf("%s Connected\n",client_name);
        clients[i].id = client_socket;
        strcpy(clients[i].name, client_name);

        if (pthread_create(&threads[i], NULL, handle_client, (void *)&clients[i]) != 0) 
        {
            printf("Error: Creating Thread Failed\n");
            printf("errno: %d\n", errno);
            return 1;
        }
    }

    for (int i = 0; i < 10; i++) 
    {
        pthread_join(threads[i], NULL);
    }

    close(server_socket);
    return 0;
}

