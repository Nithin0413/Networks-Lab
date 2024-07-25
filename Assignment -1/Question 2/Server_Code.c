#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <errno.h>


void rev_string(char string[])
{
    int length = strlen(string);
    int i;
    for (i = 0; i < length / 2; i++)
    {
        char temp = string[i];
        string[i] = string[length - i - 1];
        string[length - i - 1] = temp;
    }
}


int main() 
{
  int sockfd,portno; 
  struct sockaddr_in server_addr, client_addr; 
  socklen_t addr_len; 
  char string[256]; 

  int x;
  printf("Enter the Port Number: ");
  scanf("%d",&x);
  portno = x;

  sockfd = socket(AF_INET, SOCK_DGRAM, 0);
  if (sockfd < 0)
  {
      printf("Error: Socket Creating Failed\n");
      printf("errno: %d\n", errno);
      return 1;
  }

  memset(&server_addr, 0, sizeof(server_addr));
  server_addr.sin_family = AF_INET;
  server_addr.sin_addr.s_addr = INADDR_ANY;  
  server_addr.sin_port = htons(portno); 

  
  if (bind(sockfd, (struct sockaddr *) &server_addr, sizeof(server_addr)) < 0)
  {
    printf("Error: Binding Failed");
    printf("errno: %d\n", errno);
    return 1;
  }
  printf("Binding Completed\n");

  
  addr_len = sizeof(client_addr);


  if (recvfrom(sockfd, string, 256, 0, (struct sockaddr *) &client_addr, &addr_len) < 0) 
  {
    perror("Error receiving data from client");
    return 1;
  }
  
  rev_string(string);
  
  if (sendto(sockfd, string, 256, 0, (struct sockaddr *) &client_addr, addr_len) < 0) 
  {
    printf("Error: Sending data to client");
    printf("errno: %d\n", errno);
    return 1;
  }

  close(sockfd);

  return 0;
}
