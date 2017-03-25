// server 

#include <stdio.h>
#include <stdlib.h>
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
#define PORT "7777"  // the port users will be connecting to
#define BACKLOG 10   // how many pending connections queue will hold
#define PACKETSIZE 1024
#define MAXDATASIZE 20000000

void *get_in_addr(struct sockaddr *sock_addr)
{
  if (sock_addr->sa_family == AF_INET) 
    return &(((struct sockaddr_in*)sock_addr)->sin_addr);
  return &(((struct sockaddr_in6*)sock_addr)->sin6_addr);
}

int main (void)
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

  memset (&hints, 0, sizeof (hints));
  hints.ai_family = AF_UNSPEC;
  hints.ai_socktype = SOCK_STREAM;
  hints.ai_flags = AI_PASSIVE;

  if ((success = getaddrinfo (NULL, PORT, &hints, &servinfo)) != 0)
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
    sock_in_size = sizeof (their_addr);
    new_sockfd = accept (sockfd, (struct sockaddr *) & their_addr, &sock_in_size);
    if (new_sockfd == -1)
    {
      perror ("server : accept\n");
      continue;
    }

    inet_ntop (their_addr.ss_family, \
               get_in_addr ((struct sockaddr *) & their_addr), s, sizeof (s));

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
  // close 어떻게 짜지??  






