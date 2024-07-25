#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <errno.h>
# define port_no 11111

struct packet 
{
    int sequence_no;
    char payload[1024];
    int checksum;
};

int checksum_calculator(struct packet data)
{
    int checksum=0;
    for (int i = 0; i < sizeof(data) - sizeof(int); i++) 
    {
        checksum =checksum^(((uint8_t*)(&data))[i]);
    }
    return checksum;
}


int main() 
{
  int sockfd;
  struct sockaddr_in server_addr, client_addr; 
  socklen_t addr_len; 
  struct packet data;
  int ack; // +1 - Acknowledgement and -1 - Negative Acknowledgement

  // Creating socket
  sockfd = socket(AF_INET, SOCK_STREAM, 0);
  if (sockfd < 0)
  {
      printf("Error: Socket Creating Failed\n");
      printf("errno: %d\n", errno);
      return 1;
  }
  printf("Socket created successfully\n");

  memset(&server_addr, 0, sizeof(server_addr));
  server_addr.sin_family = AF_INET;
  server_addr.sin_addr.s_addr = INADDR_ANY;  
  server_addr.sin_port = htons(port_no); 

  // Binding the port with server address
  if (bind(sockfd, (struct sockaddr *) &server_addr, sizeof(server_addr)) < 0)
  {
    printf("Error: Binding Failed");
    printf("errno: %d\n", errno);
    return -1;
  }
  printf("Binding Completed\n");

  // Listening to incoming request for conection
  listen(sockfd,6);

  int client_len =sizeof(client_addr);

  // Accepting new connection
  int new_sockfd = accept(sockfd, (struct sockaddr *)&client_addr, &client_len);
  if(new_sockfd<0)
  {
    printf("Error: Connection not accpeted\n");
    printf("errno: %d\n", errno);
    return -1;
  }
  printf("Connection accpeted successfully\n");
  while(1)
  {
      memset(&data,0,sizeof(data));
      int n=read(new_sockfd,&data,sizeof(data));
      if(n<0)
      {
        printf("Error: Recieving failed\n");
        printf("%d\n",errno);
        return -1;
      }
      int checksum = checksum_calculator(data);
      if(checksum!=data.checksum)
      {
        printf("-----------------Packet recieved corrupted-----------------\n");
        ack=-1;
        n=write(new_sockfd,&ack,sizeof(int));
        if(n<0)
        {
          printf("Error: Sending failed\n");
          printf("%d\n",errno);
          return -1;
        }
        continue;
      }
      printf("-----------------Packet recieved successfully-----------------\n");
      printf("Sequence Number -%d\n",data.sequence_no);
      printf("Payload -%s\n",data.payload);
      ack=1;
      n=write(new_sockfd,&ack,sizeof(int));
      if(n<0)
      {
        printf("Error: Sending failed\n");
        printf("%d\n",errno);
        return -1;
      }
      
  }
  close(sockfd);
  return 0;
}
