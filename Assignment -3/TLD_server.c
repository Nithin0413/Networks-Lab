#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include<errno.h>
#include <sys/types.h>
#include <unistd.h>



#define Auth_DNS 9994
#define TLD_DNS 9993

struct cache
{
    char host_name[256];
    char ip_addr[16];
};

int main() 
{
  char host_name[256];
  char ip_addr[16];

  struct cache dns_data;
  memset(&dns_data,0,sizeof(dns_data));
  
  int auth_sock, TLD_sock;
  struct sockaddr_in TLD_server, root_server, auth_server;


  // Creating Sockets
  TLD_sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP); // Acts as Server
  auth_sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP); // Acts as Client

  if (auth_sock == -1) 
  {
    printf("Error creating Authoritative server socket");
    printf("errno: %d\n", errno);
    return -1;
  }

  if (TLD_sock == -1) 
  {
    printf("Error creating TLD server socket");
    printf("errno: %d\n", errno);
    return -1;
  }


  // Setting up the Root Server Address
  memset(&TLD_server, 0, sizeof(TLD_server));
  TLD_server.sin_family = AF_INET;
  TLD_server.sin_port = htons(TLD_DNS);
  TLD_server.sin_addr.s_addr = inet_addr("127.0.0.3");

  // Setting up the TLD Server Address
  memset(&auth_server, 0, sizeof(auth_server));
  auth_server.sin_family = AF_INET;
  auth_server.sin_port = htons(Auth_DNS);
  auth_server.sin_addr.s_addr = inet_addr("127.0.0.4");

  int optval = 1;
  setsockopt(TLD_sock, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval));

  // Binding the Root Socket to Server Address
  if (bind(TLD_sock, (struct sockaddr*) &TLD_server, sizeof(TLD_server)) == -1) 
  {
    printf("Error binding Local socket");
    printf("errno: %d\n", errno);
    return 1;
  }

  while (1) 
  {
    char input[256];
    int root_len = sizeof(root_server);
    int auth_len = sizeof(auth_server);
    printf("\nTLD Server is Running\n");
    
    // Receive a DNS request
    memset(input,0,256);
    int n = recvfrom(TLD_sock, input, 256, 0, (struct sockaddr*) &root_server, &root_len);
    if (n <0) 
    {
        printf("Error receiving data");
        printf("errno: %d\n", errno);
        return 1;
    }
    // If requested domain is already there in DNS Local Server Cache 
    if(strcmp(input,dns_data.host_name)==0)
    {
        strcpy(ip_addr,dns_data.ip_addr);
        n=sendto(TLD_sock, ip_addr, 16, 0, (struct sockaddr*) &root_server, root_len);
        if (n== -1) 
        {
            printf("Error sending data");
            printf("errno: %d\n", errno);
            continue;
        }
        printf("Domain found\n");
    }
    // Passing to TLD Server
    else
    {
        printf("Request passing to Corresponding Authoritative Server\n");
        n=sendto(auth_sock, input, 256, 0, (struct sockaddr*) &auth_server, auth_len);
        if (n== -1) 
        {
            printf("Error sending data");
            printf("errno: %d\n", errno);
            continue;
        }
        memset(ip_addr,0,16);
        n = recvfrom(auth_sock, ip_addr, 16, 0, NULL, NULL);
        if (n<0) 
        {
            printf("Error receiving data\n");
            printf("errno: %d\n", errno);
            return 1;
        }
        n=sendto(TLD_sock, ip_addr, 16, 0, (struct sockaddr*) &root_server, root_len);
        if (n<0) 
        {
            printf("Error sending data\n");
            printf("errno: %d\n", errno);
            return 1;
        }
    }
    printf("Requested Completed\n");

  }
  close(auth_sock);
  close(TLD_sock);

  return 0;
}