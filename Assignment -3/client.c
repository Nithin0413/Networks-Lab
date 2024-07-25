#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include<errno.h>
#include <sys/types.h>


#define Local_DNS 9991

int main() 
{
    int sock;
    struct sockaddr_in server,client;
    char domain_name[256];
    char ip_addr[16];

    // Creating the Socket
    sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (sock < 0) 
    {
        printf("Error creating socket");
        printf("errno: %d\n", errno);
        return 1;
    }

    // Setting up the Server Address
    memset(&server, 0, sizeof(server));
    
    server.sin_family = AF_INET;
    server.sin_port = htons(Local_DNS);
    server.sin_addr.s_addr = inet_addr("127.0.0.1");

    printf("Enter the Domain Name\n");
    scanf("%s",domain_name);

    int server_len = sizeof(server);
    int client_len = sizeof(client);
    // Send the Domain Name to the server
    if (sendto(sock, domain_name, 256, 0, (struct sockaddr *) &server, server_len) < 0) 
    {
        printf("Error sending data\n");
        printf("errno: %d\n", errno);
        return 1;
    }

    if(recvfrom(sock, ip_addr, 16, 0, NULL, NULL)<0)
    {
        printf("Error recieving data\n");
        printf("errno: %d\n", errno);
        return 1;
    }
    printf("IP Address of your corresponding domain is : %s\n",ip_addr);
    // Close the socket
    close(sock);

return 0;
}




