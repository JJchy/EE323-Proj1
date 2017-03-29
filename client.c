// client

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
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
  int sockfd, bytes, location;
  struct addrinfo hints, *servinfo, *p;
  int success;
  char s[INET6_ADDRSTRLEN];
  bool is_port_first = check_port_ip_number (argc, argv);
  char *PORT, *IP;
  char buff[MAXDATASIZE];

  memset (&buff, 0, MAXDATASIZE);

  if (is_port_first)
  {
    PORT = argv[2];
    IP = argv[4];
  }
  else
  {
    PORT = argv[4];
    IP = argv[2];
  }

  memset (&hints, 0, sizeof (hints));
  hints.ai_family  = AF_UNSPEC;
  hints.ai_socktype = SOCK_STREAM;
  
  if ((success = getaddrinfo (IP, PORT, &hints, &servinfo)) != 0) 
  {
    fprintf (stderr, "getaddrinfo : %s\n", gai_strerror (success));
    return 1;
  }

  for (p = servinfo; p != NULL; p = p->ai_next)
  {
    if ((sockfd = socket (p->ai_family, p->ai_socktype,\
                          p->ai_protocol)) == -1)
    {
      perror ("client : socket\n");
      continue;
    }

    if (connect (sockfd, p->ai_addr, p->ai_addrlen) == -1)
    {
      close (sockfd);
      perror ("client : connect\n");
      continue;
    }

    break;
  }

  if (p == NULL)
  {
    fprintf (stderr, "client : failed to connect");
    return 2;
  }

  freeaddrinfo (servinfo);

  while (1)
  {
    bytes = fgets (buff, 5000000, stdin);
  }




}

