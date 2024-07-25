#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include<errno.h>
#include <sys/types.h>
#include <unistd.h>


#define Local_DNS 9991
#define Root_DNS 9992

struct cache
{
    char host_name[256];
    char ip_addr[16];
};

int main() 
{
  int local_sock, root_sock;
  struct sockaddr_in local_server, client, root_server;

  struct cache dns_data;
  memset(&dns_data,0,sizeof(dns_data));

  // Creating a Socket
  local_sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
  root_sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);

  if (local_sock == -1) 
  {
    printf("Error creating local server socket\n");
    printf("errno: %d\n", errno);
    return -1;
  }

  if (root_sock == -1) 
  {
    printf("Error creating root server socket\n");
    printf("errno: %d\n", errno);
    return -1;
  }


  // Setting up the Local Server Address
  memset(&local_server, 0, sizeof(local_server));
  local_server.sin_family = AF_INET;
  local_server.sin_port = htons(Local_DNS);
  local_server.sin_addr.s_addr = inet_addr("127.0.0.1");

  // Setting up the Root Server
  memset(&root_server, 0, sizeof(root_server));
  root_server.sin_family = AF_INET;
  root_server.sin_port = htons(Root_DNS);
  root_server.sin_addr.s_addr = inet_addr("127.0.0.2");

  int optval = 1;
  setsockopt(local_sock, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval));
  // Binding the Local Server Socket to Server Address
  if (bind(local_sock, (struct sockaddr*) &local_server, sizeof(local_server)) == -1) 
  {
    printf("Error binding Local socket\n");
    printf("errno: %d\n", errno);
    return 1;
  }

  while (1) 
  {
    char input[256];
    int client_len = sizeof(client);
    int root_len = sizeof(root_server);
    char ip_addr[16];
    char host_name[256];

    printf("\nLocal Server is Running\n");
    // Receive a DNS request
    int n = recvfrom(local_sock, input, 256, 0, (struct sockaddr*) &client, &client_len);
    if (n== -1) 
    {
        printf("Error receiving data\n");
        printf("errno: %d\n", errno);
        return 1;
    }
    // If requested domain is already there in DNS Local Server Cache 
    if(strcmp(input,dns_data.host_name)==0)
    {
        printf("Domain found in local cache\n");
        strcpy(ip_addr,dns_data.ip_addr);
        n=sendto(local_sock, ip_addr, 16, 0, (struct sockaddr*) &client, client_len);
        if (n== -1) 
        {
            printf("Error sending data\n");
            printf("errno: %d\n", errno);
            return 1;
        }
    }
    // Requesting to Root Server for given Domain Name
    else
    {
        printf("Domain not found in cache - Request Passing to Root Server\n");
        strcpy(host_name,input);
        n=sendto(root_sock, host_name, 256, 0, (struct sockaddr*) &root_server, root_len);
        if (n<0) 
        {
            printf("Error sending data\n");
            printf("errno: %d\n", errno);
            return 1;
        }
        memset(ip_addr,0,16);
        n = recvfrom(root_sock, ip_addr, 16, 0, NULL, NULL);
        if (n<0) 
        {
            printf("Error receiving data\n");
            printf("errno: %d\n", errno);
            return 1;
        }
        n=sendto(local_sock, ip_addr, 16, 0, (struct sockaddr*) &client, client_len);
        if (n<0) 
        {
            printf("Error sending data\n");
            printf("errno: %d\n", errno);
            return 1;
        }
        strcpy(dns_data.host_name,host_name);
        strcpy(dns_data.ip_addr,ip_addr);

    }
    printf("Request Completed\n");

  }
  close(local_sock);
  close(root_sock);

  return 0;
}
