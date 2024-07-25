#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>
#include <errno.h>

int main()
{
    int sockfd, portno, n;
    struct sockaddr_in serv_addr;
    char string[256];

    int x;
    printf("Enter the Port Number:");
    scanf("%d",&x);
    portno = x;

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0)
    {
        printf("Error: Socket Creating Failed\n");
        printf("errno: %d\n", errno);
        return 1;
    }
    printf("Socket Created\n");

    memset((char*) &serv_addr,0,sizeof(serv_addr));

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(portno);
    serv_addr.sin_addr.s_addr=INADDR_ANY;


    int con = connect(sockfd, (struct sockaddr*)&serv_addr, sizeof(serv_addr));
    if ( con < 0)
    {
        printf("Error: Connection failed\n");
        printf("errno: %d\n", errno);
        return 1;
    }
    printf("Connection Established\n");
    

    printf("Enter Your String: \n");
    memset(string,0, 256);
    char temp[10];
    fgets(temp,sizeof(temp),stdin);
    fgets(string,256,stdin);
    int ret;
    ret = write(sockfd, string, strlen(string));
    if (ret < 0)
    {
        printf("Error: Writing Failed\n");
        printf("errno: %d\n", errno);
        return 1;
    }

    memset(string,0,256);
    ret = read(sockfd, string, 255);
    if (ret < 0)
    {
        printf("Error: Reading Failed\n");
        printf("errno: %d\n", errno);
        return 1;
    }
    printf("Reversed String:");
    printf("%s\n", string);
    shutdown(sockfd, SHUT_RDWR);
    close(sockfd);
    return 0;
}
