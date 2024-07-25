#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include<errno.h>
#include <sys/types.h>
#include <unistd.h>



#define Auth_DNS 9994

struct cache
{
    char host_name[256];
    char ip_addr[16];
};

int main() 
{
  char host_name[256];
  char ip_addr[16];

  // A Record
  struct cache dns_data[5];7
  strcpy(dns_data[0].host_name,"www.amazon.in");
  strcpy(dns_data[0].ip_addr,"192.0.0.1");

  strcpy(dns_data[1].host_name,"www.youtube.com");
  strcpy(dns_data[1].ip_addr,"192.0.0.2");

  strcpy(dns_data[2].host_name,"www.google.com");
  strcpy(dns_data[2].ip_addr,"192.0.0.3");

  strcpy(dns_data[3].host_name,"www.flipkart.in");
  strcpy(dns_data[3].ip_addr,"192.0.0.4");

  strcpy(dns_data[4].host_name,"www.twitter.com");
  strcpy(dns_data[4].ip_addr,"192.0.0.5");
  
  int auth_sock;
  struct sockaddr_in TLD_server, auth_server;


  // Creating Sockets
  auth_sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP); // Acts as Server

  if (auth_sock == -1) 
  {
    printf("Error creating Authoritative server socket");
    printf("errno: %d\n", errno);
    return -1;
  }



  // Setting up the TLD Server Address
  memset(&auth_server, 0, sizeof(auth_server));
  auth_server.sin_family = AF_INET;
  auth_server.sin_port = htons(Auth_DNS);
  auth_server.sin_addr.s_addr = inet_addr("127.0.0.4");

  int optval = 1;
  setsockopt(auth_sock, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval));

  // Binding the Root Socket to Server Address
  if (bind(auth_sock, (struct sockaddr*) &auth_server, sizeof(auth_server)) == -1) 
  {
    printf("Error binding Local socket");
    printf("errno: %d\n", errno);
    return 1;
  }

  while (1) 
  {
    char input[256];
    int TLD_len = sizeof(TLD_server);
    printf("\nAuthoritative Server is Running\n");
    
    // Receive a DNS request
    memset(input,0,256);
    int n = recvfrom(auth_sock, input, 256, 0, (struct sockaddr*) &TLD_server, &TLD_len);
    if (n <0) 
    {
        printf("Error receiving data");
        printf("errno: %d\n", errno);
        return 1;
    }
    int flag=1;
    // If requested domain is already there in DNS Local Server Cache
    for(int i=0;i<5;i++) 
    {
        if(strcmp(input,dns_data[i].host_name)==0)
        {
            strcpy(ip_addr,dns_data[i].ip_addr);
            n=sendto(auth_sock, ip_addr, 16, 0, (struct sockaddr*) &TLD_server, TLD_len);
            if (n== -1) 
            {
                printf("Error sending data");
                printf("errno: %d\n", errno);
                continue;
            }
            printf("Domain found\n");
            flag=0;
            break;
        }
    }
    // Passing to TLD Server
    if(flag==1)
    {
        printf("Data not found in A record\n");
    }
    printf("Requested Completed\n");

  }
  close(auth_sock);

  return 0;
}