// server 

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include <signal.h>
#define BACKLOG 10   // how many pending connections queue will hold
#define PACKETSIZE 1024
#define MAXDATASIZE 5000000

void* get_in_addr (struct sockaddr* sock_addr)
{
  if (sock_addr->sa_family == AF_INET) 
    return &(((struct sockaddr_in*)sock_addr)->sin_addr);
  return &(((struct sockaddr_in6*)sock_addr)->sin6_addr);
}

void check_port_number (int argc, char** argv)
{
  if ((strcmp (argv[1], "-p") != 0) || (argc != 3))
  {
    perror ("server : argument\n");
    exit (1);
  }
  
  for (int i = 0; i < strlen (argv[2]); i++)
  {
    if (isdigit (argv[2][i]) == 0)
    {
      perror ("server : PORT number\n");
      exit (1);
    }
  }

  if ((atoi (argv[2]) > 65535) || (atoi (argv[2]) < 1024))
  {
    perror ("server : PORT number\n");
    exit (1);
  }
}
  

int main (int argc, char** argv)
{
  int sockfd, new_sockfd;
  int bytes, location;
  socklen_t sock_in_size;
  struct addrinfo hints, *servinfo, *p;
  struct sockaddr_storage their_addr;
  int yes = 1, success;
  char s[INET6_ADDRSTRLEN];
  char packet[PACKETSIZE];
  char buff[MAXDATASIZE];

  memset (&buff, 0, MAXDATASIZE);

  check_port_number (argc, argv);

  memset (&hints, 0, sizeof (hints));
  hints.ai_family = AF_UNSPEC;
  hints.ai_socktype = SOCK_STREAM;
  hints.ai_flags = AI_PASSIVE;

  if ((success = getaddrinfo (NULL, argv[2], &hints, &servinfo)) != 0)
  {
    fprintf (stderr, "getaddrinfo : %s\n", gai_strerror (success));
    return 1;
  }

  for (p = servinfo; p != NULL; p = p->ai_next)
  {
    if ((sockfd = socket (p->ai_family, p->ai_socktype,\
                          p->ai_protocol)) == -1)
    {
      perror ("server : socket\n");
      continue;
    }

    if (setsockopt (sockfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1)
    {
      perror ("server : setsockopt\n");
      exit (1);
    }

    if (bind (sockfd, p->ai_addr, p->ai_addrlen) == -1)
    {
      close (sockfd);
      perror ("server : bind\n");
      continue;
    }

    break;
  }

  if (p == NULL)
  {
    fprintf (stderr, "server : failed to bind\n");
    return 2;
  }

  freeaddrinfo (servinfo);

  if (listen (sockfd, BACKLOG) == -1)
  {
    perror ("server : listen\n");
    exit (1);
  }

  while (1)
  {
    printf ("Success\n");
    sock_in_size = sizeof (their_addr);
    new_sockfd = accept (sockfd, (struct sockaddr *) & their_addr, &sock_in_size);
    if (new_sockfd == -1)
    {
      perror ("server : accept\n");
      continue;
    }

    if (!fork ())
    {
      while (1)
      {
        memset (&packet, 0, PACKETSIZE);
        if ((bytes = recv (new_sockfd, packet, PACKETSIZE, 0)) == -1)
        {
          perror ("server : recv\n");
          exit (1);
        }

        if (bytes != PACKETSIZE)
        {
          if (packet[0] == -1) close (new_sockfd);

          if (strlen (buff) != 0)
          {
            strcat (buff, "\0");
            puts (buff);
          }

          else
          {
            packet[PACKETSIZE] = '\0';
            puts (packet);
          }
        }

        else 
        {
          strcat (buff, packet);
          if (packet[PACKETSIZE - 1] == -1)
            puts (buff);
        }
      }
    }
    else close (new_sockfd);

    while (waitpid (-1, NULL, WNOHANG) > 0);
  }
}







