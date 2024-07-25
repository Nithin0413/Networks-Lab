#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include<errno.h>
#include <sys/types.h>
#include <unistd.h>



#define Root_DNS 9992
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
  int root_sock, TLD_sock;
  struct sockaddr_in root_server, local_server, TLD_server;


  // Creating Sockets
  root_sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP); // Acts as Server
  TLD_sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP); // Acts as Client

  if (root_sock == -1) 
  {
    printf("Error creating Root server socket");
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
  memset(&root_server, 0, sizeof(root_server));
  root_server.sin_family = AF_INET;
  root_server.sin_port = htons(Root_DNS);
  root_server.sin_addr.s_addr = inet_addr("127.0.0.2");

  // Setting up the TLD Server Address
  memset(&TLD_server, 0, sizeof(TLD_server));
  TLD_server.sin_family = AF_INET;
  TLD_server.sin_port = htons(TLD_DNS);
  TLD_server.sin_addr.s_addr = inet_addr("127.0.0.3");

  int optval = 1;
  setsockopt(root_sock, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval));

  // Binding the Root Socket to Server Address
  if (bind(root_sock, (struct sockaddr*) &root_server, sizeof(root_server)) == -1) 
  {
    printf("Error binding Local socket");
    printf("errno: %d\n", errno);
    return 1;
  }

  while (1) 
  {
    char input[256];
    struct cache dns_data;
    
    int local_len = sizeof(local_server);
    int TLD_len = sizeof(TLD_server);
    printf("\nRoot Server is Running\n");

    // Receive a DNS request
    memset(input,0,256);
    int n = recvfrom(root_sock, input, 256, 0, (struct sockaddr*) &local_server, &local_len);
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
        n=sendto(root_sock, ip_addr, 16, 0, (struct sockaddr*) &local_server, local_len);
        if (n== -1) 
        {
            printf("Error sending data");
            printf("errno: %d\n", errno);
            continue;
        }
    }
    // Passing to TLD Server
    else
    {
        char *token;
        const char delim[]=".";
        char tld[10];
        char temp[256];
        strcpy(temp,input);
        token =strtok(temp,delim);
        while(token!=NULL)
        {
          strcpy(tld,token);
          token=strtok(NULL,delim);
        }
        printf("TLD - %s\n",tld);
        printf("Request passing to corresponding TLD\n");
        n=sendto(TLD_sock, input, 256, 0, (struct sockaddr*) &TLD_server, TLD_len);
        if (n== -1) 
        {
            printf("Error sending data");
            printf("errno: %d\n", errno);
            continue;
        }
        memset(ip_addr,0,16);
        n = recvfrom(TLD_sock, ip_addr, 16, 0, NULL, NULL);
        if (n<0) 
        {
            printf("Error receiving data\n");
            printf("errno: %d\n", errno);
            return 1;
        }
        n=sendto(root_sock, ip_addr, 16, 0, (struct sockaddr*) &local_server, local_len);
        if (n<0) 
        {
            printf("Error sending data\n");
            printf("errno: %d\n", errno);
            return 1;
        }
    }
    printf("Request Completed\n");

  }
  close(root_sock);
  close(TLD_sock);

  return 0;
}