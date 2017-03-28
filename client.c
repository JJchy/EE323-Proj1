// client

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#define PACKETSIZE 1024
#define MAXDATASIZE 5000000

void* get_in_addr (struct sockaddr* sock_addr)
{
  if (sock_addr->sa_family == AF_INET)
    return &(((struct sockaddr_in*)sock_addr)->sin_addr);
  return &(((struct sockaddr_in6*)sock_addr)->sin6_addr);
}

bool check_port_ip_number (int argc, char** argv) // true -> port first, false -> ip first
{
  int port_location, ip_location;
  struct sockaddr_in soc_addr;

  if (!((strcmp (argv[1], "-p") == 0) || (strcmp (argv[1], "-h") == 0)) ||\
      !((strcmp (argv[3], "-p") == 0) || (strcmp (argv[3], "-h") == 0)) ||\
      (strcmp (argv[1], argv[3]) == 0) || (argc != 5))
  {
    perror ("client : argument\n");
    exit (1);
  }

  if (strcmp (argv[1], "-p") == 0) 
  {
    port_location = 2;
    ip_location = 4;
  }
  else
  { 
    port_location = 4;
    ip_location = 2;
  }

  for (int i = 0; i < strlen (argv[port_location]); i++)
  {
    if (isdigit (argv[port_location][i]) == 0)
    {
      perror ("server : PORT number\n");
      exit (1);
    }
  }

  if ((atoi (argv[port_location]) > 65536) ||\
      (atoi (argv[port_location]) < 1024))
  {
    perror ("server : PORT number\n");
    exit (1);
  }

  if (inet_pton (AF_INET, argv[ip_location], &(soc_addr.sin_addr)) != 1)
  {
    perror ("server : IP address\n");
    exit (1);
  }

  if (port_location == 2) return true;
  else false;
}

int main (int argc, char** argv)
{
  int sockfd, bytes;
  int bytes, location;
  struct addrinfo hints, *servinfo, *p;
  int success;
  char s[INET6_ADDRSTRLEN];
}

