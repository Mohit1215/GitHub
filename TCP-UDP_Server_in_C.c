/* tcp-server.c */

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <sys/sendfile.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <math.h>

#define MAX_CLIENTS 100
#define BUFFER_SIZE 1024
#define MAXBUFFER 8192

struct userid
{
  char name[1024]; //userid
  struct sockaddr_in user; //IP address of the client
  int socket_user; // socket details of the user
  int type; //which type of socket UDP/TCP
};

struct entry_table
{
  int num;
  struct sockaddr_in client_entry;
  int new_sd;
  int s_type;
  int ftime;
};

int num_clients=0,gtemp=0;
int count=0;
struct userid *u1;
pthread_mutex_t mutex=PTHREAD_MUTEX_INITIALIZER;

void *TCP_processing(void *e1)
{
  struct entry_table *e=(struct entry_table *)e1;
  struct sockaddr_in client1;
  int sdnew;
  int stype;
  int fftime;

  client1=e->client_entry;
  sdnew=e->new_sd;
  stype=e->s_type;
  fftime=e->ftime;

  int n,error=0,fsock=0;
  int len_buf;
  char buffer[ BUFFER_SIZE ];
  char org_msg[ BUFFER_SIZE ];
  char *token;
  char user_name[50];

  do
  {
    error=0;


    len_buf = recv( sdnew, buffer, BUFFER_SIZE, 0 );   /* or read() */

    strcpy(org_msg,buffer);
    char delim[4]=" ";

    buffer[len_buf-1]='\0';
    org_msg[len_buf-1]='\0';

    token=strtok(buffer,delim);

    char *names_tokens[1024];
    int k=0;
    names_tokens[k]=token;
    while(token!=0)
    {
      names_tokens[k++]=token;
      token=strtok(0,delim);
    }

    if(fftime==1&&(strcmp(names_tokens[0],"LOGIN")!=0))//*************to check if the user is a first time user and requesting any command other than LOGIN *******
    {
        printf( "CHILD %u: Sent ERROR (Not logged in)\n",(unsigned int)pthread_self());
        fflush(stdout);
        char err_msg[1000];
        strcpy(err_msg,"ERROR Not logged in\n");
        //strcat(err_msg," ");
        int leng=strlen(err_msg);
        n = send(sdnew, err_msg, leng, 0);
    }
    else{
        if(!strcmp(names_tokens[0],"LOGIN"))
        {
          printf( "CHILD %u: Rcvd LOGIN request for userid %s\n",(unsigned int)pthread_self(),names_tokens[1]);
          fflush(stdout);

            char temp[50];
            strcpy(temp,names_tokens[1]);
            strcpy(user_name,names_tokens[1]);
            int length=strlen(temp);
            int correct=1;

            for(int b1=0;b1<length;b1++){// To check if the user id are alphanumeric
              if(isalnum(temp[b1])==0){
                correct=0;
                break;
              }
            }

            if((correct==1)&&length>=3&&length<100)
           {
             pthread_mutex_lock(&mutex);
             if(count>0){
              for(int g=count;g>=0;g--)
              {
                if(strcmp((u1+g)->name,names_tokens[1])==0)
                {
                  printf("CHILD %u: Sent ERROR (Already connected)\n",(unsigned int)pthread_self());
                  fflush(stdout);
                  char err_msg[1000];
                  strcpy(err_msg,"ERROR Already connected\n");
                  //strcat(err_msg," ");
                  int leng=strlen(err_msg);
                  n = send(sdnew, err_msg, leng, 0);
                  error=1;
                  break;
                }
                if(sdnew==(u1+g)->socket_user)
                {
                  fsock=1;
                  gtemp=g;
                  break;
                }
              }
            }
              if(error==0){
                n = send( sdnew, "OK\n", 3, 0 );
                if(fsock==1)
                {
                  strcpy((u1+gtemp)->name,names_tokens[1]);
                }
                else
                {
                  strcpy((u1+count)->name,names_tokens[1]);
                  (u1+count)->user=client1;
                  (u1+count)->socket_user=sdnew;
                  (u1+count)->type=stype;
                  fftime=0;//Now the user can use any command
                  count++;
                }
              }
            pthread_mutex_unlock(&mutex);
            }
           else{
              printf("CHILD %u: Sent ERROR (Invalid userid)\n",(unsigned int)pthread_self());
              fflush(stdout);
              char err_msg[1000];
              strcpy(err_msg,"ERROR Invalid userid\n");
              strcat(err_msg," ");
              int leng=strlen(err_msg);
              n = send(sdnew, err_msg, leng, 0);
              if ( n != leng )
              {
                printf("CHILD %u: ERROR",(unsigned int)pthread_self());
                perror("error");
                exit(EXIT_FAILURE);
                //return 0;
              }
            }
    }
    //token delimitter was added here

    if ( len_buf == -1 )
    {
      perror( "recv() failed" );
      //return EXIT_FAILURE;
      return 0;
    }
    else if ( len_buf == 0 )
    {
      printf("CHILD %u: Client disconnected\n",(unsigned int)pthread_self());
    }
    else /* n > 0 */
    {
      if(!strcmp(names_tokens[0],"WHO"))
      {
        printf( "CHILD %u: Rcvd WHO request\n",(unsigned int)pthread_self());
        fflush(stdout);
        char namesforwho[32][1024]={0};
        for(int z=0;z<count;z++)
        {
          strcpy(namesforwho[z],(u1+z)->name);
        }

         char tempo[50];
         if(count>0)
         {
           for (int i = 0; i < count - 1 ; i++)//Arranging in the ASCII order
           {
             for (int j = i + 1; j < count; j++)
             {
               if (strcmp(namesforwho[i],namesforwho[j]) > 0)
               {
                 strcpy(tempo,namesforwho[i]);
                 strcpy(namesforwho[i], namesforwho[j]);
                 strcpy(namesforwho[j], tempo);
               }
             }
           }
        }

         if(count>0)
         {
           char temp[10000];
           strcpy(temp,"OK");
           strcat(temp,"\n");
                   for(int g=0;g<count;g++)
                   {
                     strcat(temp,namesforwho[g]);
                     strcat(temp,"\n");
                   }
                   int length=strlen(temp);
                   n = send( sdnew, temp, length, 0 );//Sending as a single packet.
                   if ( n != length )
                   {
                     perror( "send() failed" );
                     //return EXIT_FAILURE;
                     return 0;
                   }
                 }
               }

               else if(!strcmp(names_tokens[0],"SEND")){              /*** SEND request**/
                   printf( "CHILD %u: Rcvd SEND request to userid %s\n",(unsigned int)pthread_self(),names_tokens[1]);
                   fflush(stdout);
                   int rec_sock=-1;
                   int rec_type=-1;
                   struct sockaddr_in tempadd;
                   int len2 = sizeof( tempadd );
                   int msg_len1=atoi(names_tokens[2]);
                   if(msg_len1>=1&&msg_len1<=994)
                        {
                          if(count>0)
                          {
                           for(int g=0;g<count;g++)
                           {
                             if(strcmp((u1+g)->name,names_tokens[1])==0)
                             {
                               rec_sock=(u1+g)->socket_user;
                               rec_type=(u1+g)->type;
                               tempadd=(u1+g)->user;
                               error=1;
                               break;
                             }
                           }
                         }

                         char temp[50];
                         strcpy(temp,names_tokens[1]);
                         char message[1000] = {0};
                         char tempom[1000] = {0};
                         strcpy(message, "FROM ");
                         strcat(message, user_name);
                         strcat(message, " ");
                         strcat(message, names_tokens[2]);

                        char a[100];
                        strcpy(a,names_tokens[0]);
                        strcat(a,names_tokens[1]);
                        strcat(a,names_tokens[2]);

                        int lenk=strlen(a);
                        int h=0;

                         for(int k=(lenk+2);k<len_buf;k++){

                           tempom[h]=org_msg[k];
                           //printf (" the value of temp is:  %c\n",tempom[h]);
                           h++;
                         }
                         tempom[h]='\0';
                         strcat(message,tempom);

                         int msg_len=strlen(message);
                         if(error==0)
                           {
                             printf("CHILD %u: Sent ERROR (Unknown userid)\n",(unsigned int)pthread_self());
                             fflush(stdout);
                             char err_msg[1000];
                             strcpy(err_msg,"ERROR Unknown userid\n");
                             //strcat(err_msg," ");
                             int leng=strlen(err_msg);
                             n = send(sdnew, err_msg, leng, 0);
                           }

                        /*else{
                           printf("CHILD %u: Sent ERROR (Invalid userid)\n",(unsigned int)pthread_self());
                         }*/
                         if(rec_type==1){
                           n = send( sdnew, "OK\n", 3, 0 );
                           //printf("The receiving socket: %d\n",rec_sock);
                           n=send(rec_sock,message,msg_len,0);
                           if ( n != msg_len )
                           {
                             perror( "send() failed" );
                             //return EXIT_FAILURE;
                             return 0;
                           }
                         }
                         if(rec_type==0){
                           //printf("This is UDP case but sent\n");
                           n = send( sdnew, "OK\n", 3, 0 );
                           n=sendto( rec_sock, message, msg_len, 0, (struct sockaddr *) &tempadd, len2 );
                         }
                       }
                         else{
                           printf("CHILD %u: Sent ERROR (Invalid msglen)\n",(unsigned int)pthread_self());
                           fflush(stdout);
                           char err_msg[1000];
                           strcpy(err_msg,"ERROR Invalid msglen\n");
                           //strcat(err_msg," ");
                           int leng=strlen(err_msg);
                           n = send(sdnew, err_msg, leng, 0);
                           if ( n != leng )
                           {
                             printf("CHILD %u: ERROR",(unsigned int)pthread_self());
                             perror("error");
                             exit(EXIT_FAILURE);
                             //return 0;
                           }
                         }
                 }

                 else if(!strcmp(names_tokens[0],"SHARE")){  /**** SHARE request***/
                     printf( "CHILD %u: Rcvd SHARE request\n",(unsigned int)pthread_self());
                     fflush(stdout);
                     int byte_recv=0;
                     char msg_share[2048];
                     int rec_sock;
                     int rec_type;
                     if(count>0)
                     {
                       for(int g=count;g>=0;g--)
                       {
                         if(strcmp((u1+g)->name,names_tokens[1])==0)
                         {
                           rec_sock=(u1+g)->socket_user;
                           rec_type=(u1+g)->type;
                           error=1;
                           break;
                         }
                       }
                     }
                     if(rec_type==1){// file sharing for TCP
                        n = send( sdnew, "OK\n", 3, 0 );//Acknowledgement for the command share.

                        char message[1000];
                        char filesize[100];
                          strcpy(filesize,names_tokens[2]);
                          strcpy(message, "SHARE ");
                            strcat(message, user_name);
                            strcat(message," ");
                            strcat(message,filesize);
                            strcat(message," ");
                            int msg_len=strlen(message);

                            n=send(rec_sock,message,msg_len,0);
                            /* Sending file data */
                            while (1)
                            {
                                    byte_recv=recv(sdnew,msg_share,2048,0);
                                    //fprintf(stdout, "1. Server sent %d bytes from file's data, offset is now : %d and remaining data = %d\n", sent_bytes, offset, remain_data);

                                    if(byte_recv<1024){
                                        n = send( rec_sock, msg_share, byte_recv, 0 );
                                        n = send( sdnew, "OK\n", 3, 0 );
                                        break;
                                    }
                                    else{
                                      n = send( rec_sock, msg_share, 1024, 0 );
                                      n = send( sdnew, "OK\n", 3, 0 );
                                    }
                                    // int len_share=strlen(msg_share);
                            }
                          }
                          else{
                            printf("CHILD %u: Sent ERROR (SHARE not supported because recipient is using UDP)\n",(unsigned int)pthread_self());
                            fflush(stdout);
                            char err_msg[1000];
                            strcpy(err_msg,"SHARE not supported because recipient is using UDP\n");
                            strcat(err_msg," ");
                            int leng=strlen(err_msg);
                            n = send(sdnew, err_msg, leng, 0);
                            if ( n != leng )
                            {
                              printf("CHILD %u: ERROR",(unsigned int)pthread_self());
                              perror("error");
                              exit(EXIT_FAILURE);
                              //return 0;
                            }
                          }
                        }
                         else if(!strcmp(names_tokens[0],"BROADCAST")){  /*** BROADCAST request ****/
                             printf( "CHILD %u: Rcvd BROADCAST request\n",(unsigned int)pthread_self());
                             n = send( sdnew, "OK\n", 3, 0 );
                             char message1[1000]={0};
                             char tempom[1000]={0};
                             char c[100];
                             strcpy(message1, "FROM ");
                             strcat(message1, user_name);
                             strcat(message1, " ");
                             strcat(message1, names_tokens[1]);

                             strcpy(c,names_tokens[0]);
                             strcat(c,names_tokens[1]);
                             int lenk=strlen(c);

                           int h=0;

                           for(int k=(lenk+1);k<len_buf;k++)
                           {
                             tempom[h]=org_msg[k];
                             h++;
                           }
                           tempom[h]='\0';
                           strcat(message1,tempom);

                           int msg_len=strlen(message1);
                           int rec_sock;
                           int rec_type1;
                           struct sockaddr_in tempadd1;
                           int len2 = sizeof( tempadd1 );
                           if(count>0){
                             for(int g=0;g<count;g++)
                             {
                                 rec_sock=(u1+g)->socket_user;
                                 rec_type1=(u1+g)->type;
                                 tempadd1=(u1+g)->user;
                                 if(rec_type1==1)
                                 {
                                   n=send(rec_sock,message1,msg_len,0);
                                   if ( n != msg_len )
                                   {
                                     printf("CHILD %u: ERROR",(unsigned int)pthread_self());
                                     perror("error");
                                     exit(EXIT_FAILURE);
                                     //return 0;
                                   }
                                 }
                                 if(rec_type1==0){
                                   //printf("This is UDP case.\n");
                                   sendto( rec_sock, message1, msg_len, 0, (struct sockaddr *) &tempadd1, len2 );
                                 }
                             }
                           }
                     }

                     else if(!strcmp(names_tokens[0],"LOGOUT"))
                     {  /**** LOGOUT ****/
                           printf( "CHILD %u: Rcvd LOGOUT request\n",(unsigned int)pthread_self());
                           fflush(stdout);
                           n = send( sdnew, "OK\n", 3, 0 );
                           char temp[50];
                           strcpy(temp,names_tokens[1]);
                           fftime=1;
                           /* remove fd from client_sockets[] array: */
                           /* found it -- copy remaining elements over fd */
                           pthread_mutex_lock(&mutex);
                           for(int g=0;g<count;g++)
                           {
                             if(strcmp((u1+g)->name,user_name)==0)
                             {
                               int m;
                               for ( m = g ; m<count-1 ; m++ )
                               {
                                       strcpy((u1+m)->name,(u1+(m+1))->name);
                                       (u1+m)->user=(u1+(m+1))->user;
                                       (u1+m)->socket_user=(u1+(m+1))->socket_user;
                                       (u1+m)->type=(u1+(m+1))->type;
                               }
                                     count--;
                                     pthread_mutex_unlock(&mutex);
                                     break;  /* all done */
                              }
                            }
                    }
                  }
  }
  }

  while ( len_buf > 0 );
  //pthread_mutex_unlock(&mutex);
  if(len_buf<=0){
    printf("CHILD %u: Client disconnected\n",(unsigned int)pthread_self());
    fflush(stdout);
  }
  close(sdnew);


  return 0;
}

int main(int argc, char * args[])
{
  /* ====== */
  fd_set readfds;
  /* Create the listener socket as TCP socket (SOCK_STREAM) */
  int sd_tcp= socket( PF_INET, SOCK_STREAM, 0 );
  int sd_udp=socket( AF_INET, SOCK_DGRAM, 0);
  /* here, the sd is a socket descriptor (part of the fd table) */
  pthread_t tid[1000];
  /*Allocate memory to the structures for storing names of all the new clients added*/
  u1=(struct userid *)malloc(sizeof(struct userid)*100);

  if ( sd_tcp == -1 )
  {
    perror( "TCP socket() failed" );
    return EXIT_FAILURE;
  }

  if ( sd_udp == -1 )
  {
    perror( "UDP socket() failed" );
    return EXIT_FAILURE;
  }

  /* socket structures */
  struct sockaddr_in server;
  struct sockaddr_in server2;
  //struct entry_table en;
  server.sin_family = PF_INET;  /* AF_INET */
  server2.sin_family = AF_INET;  /* AF_INET */

  server.sin_addr.s_addr = htonl( INADDR_ANY );
  server2.sin_addr.s_addr = htonl( INADDR_ANY );
    /* allow any IP address to connect */

  unsigned short port = atoi(args[1]);
  unsigned short port1 = atoi(args[2]);

  /* htons() is host-to-network short for data marshalling */
  /* Internet is big endian; Intel is little endian */
  server.sin_port = htons( port );
  int len = sizeof( server );
  server2.sin_port = htons( port1 );
  int len1 = sizeof( server2 );

  /* attempt to bind (or associate) port with the TCP socket */
  if ( bind( sd_tcp, (struct sockaddr *)&server, len ) == -1 )
  {
    perror( "bind() failed" );
    return EXIT_FAILURE;
  }

  if ( bind( sd_udp, (struct sockaddr *)&server2, len1 ) == -1 )
  {
    perror( "bind() failed" );
    return EXIT_FAILURE;
  }


  /* identify this port as a listener port */
  if ( listen( sd_tcp, 5 ) == -1 )
  {
    perror( "listen() failed" );
    return EXIT_FAILURE;
  }

  printf( "MAIN: Started server\n");
  printf( "MAIN: Listening for TCP connections on port: %d\n", port );
  printf( "MAIN: Listening for UDP connections on port: %d\n", port1 );
  fflush(stdout);

  struct sockaddr_in client;
  int fromlen = sizeof( client );

  int error=0;
  while ( 1 )
  {
    FD_ZERO( &readfds );
    FD_SET( sd_udp, &readfds );
    FD_SET( sd_tcp, &readfds );

    int ready = select( FD_SETSIZE, &readfds, NULL, NULL, NULL );
    if ( ready == 0 ) {
      continue;
    }

    if ( FD_ISSET( sd_tcp, &readfds ) )// TCP connection
    {
      int newsd = accept( sd_tcp, (struct sockaddr *)&client, (socklen_t *)&fromlen );
      printf( "MAIN: Rcvd incoming TCP connection from: %s\n",inet_ntoa( (struct in_addr)client.sin_addr ) );
      fflush(stdout);
      num_clients++;
      struct entry_table *e1;
      e1=malloc(sizeof(struct entry_table)*1);
      e1->num=num_clients;
      e1->client_entry=client;
      e1->new_sd=newsd;
      e1->s_type=1;// if TCP, it is set to 1
      e1->ftime=1;

      int rc=pthread_create(&tid[num_clients],NULL,TCP_processing,e1);
      if(rc!=0)
      {
        fprintf( stderr, "Could not create thread (%d)\n",rc);
        //return EXIT_FAILURE;
      }
      free(e1);
    }

    if ( FD_ISSET( sd_udp, &readfds ) )
    {
      int n,gtemp=0,fsock=0;
      error=0;
      char buffer[ MAXBUFFER ];
      char org_msg[ BUFFER_SIZE ];

      struct sockaddr_in client2;
      int len2 = sizeof( client2 );
      n = recvfrom( sd_udp, buffer, MAXBUFFER, 0, (struct sockaddr *) &client2,
                    (socklen_t *) &len2);
      char user_name[50];

      printf( "MAIN: Rcvd incoming UDP datagram from: %s\n",inet_ntoa( (struct in_addr)client2.sin_addr ) );
      //#if 0 **************** I am dding the new code here in case there is a problem*************************
      char delim[4]=" ";
      buffer[n-1]='\0';

      strcpy(org_msg,buffer);
      int len_buf=n;
      org_msg[n-1]='\0';
      char *token;

      token=strtok(buffer,delim);

      char *names_tokens[1024];
      int k=0;
      names_tokens[k]=token;
      while(token!=0)
      {
        names_tokens[k++]=token;
        token=strtok(0,delim);
      }
      char command[1024];
      strcpy(command,names_tokens[0]);
      int lenc=strlen(command);
      //printf("This is the value of command first: %d\n",lenc);
      if(command[lenc-1]=='\n'){

        command[lenc-1]='\0';
      }
      lenc=strlen(command);
      if(!strcmp(names_tokens[0],"LOGIN")){
        printf( "MAIN: Rcvd LOGIN request for userid %s\n",names_tokens[1]);
        fflush(stdout);
              char temp[50];
              strcpy(temp,names_tokens[1]);
              strcpy(user_name,names_tokens[1]);
              int length=strlen(temp);
              int correct=1;

              for(int b1=0;b1<length;b1++){// To check if the user id are alphanumeric
                if(isalnum(temp[b1])==0){
                  correct=0;
                  break;
                }
              }
              if((correct==1)&&length>=3&&length<100)
              {
               if(count>0){
                for(int g=0;g<count;g++)
                {
                  if(strcmp((u1+g)->name,names_tokens[1])==0)
                  {
                    printf("MAIN: Sent ERROR (Already connected)\n");
                    char err_msg[1000];
                    strcpy(err_msg,"ERROR Already connected\n");
                    int leng=strlen(err_msg);
                    n = sendto(sd_udp, err_msg, leng, 0, (struct sockaddr *) &client2,len2 );
                    if ( n != leng )
                    {
                      perror( "send() failed" );
                      //return EXIT_FAILURE;
                      return 0;
                    }
                    error=1;
                    break;
                  }
                  if(sd_udp==(u1+g)->socket_user){
                    fsock=1;
                    gtemp=g;
                    break;
                  }
                }
              }
              if(error==0){
                sendto( sd_udp, "OK\n", 3, 0, (struct sockaddr *) &client2, len2 );
                if(fsock==1){
                  strcpy((u1+gtemp)->name,user_name);
                }
                else{
                  strcpy((u1+count)->name,user_name);
                  (u1+count)->user=client2;
                  (u1+count)->socket_user=sd_udp;
                  (u1+count)->type=0;
                  count++;
                  }
                }
              }
             else{
                printf("MAIN: Sent ERROR (Invalid userid)\n");
                char err_msg[1000];
                strcpy(err_msg,"ERROR Invalid userid\n");
                int leng=strlen(err_msg);
                n = sendto(sd_udp, err_msg, leng, 0, (struct sockaddr *) &client2,len2 );
              }
      }

      if(!strcmp(command,"WHO"))
      {
        printf( "MAIN: Rcvd WHO request\n");
        fflush(stdout);
        char namesforwho[32][1024]={0};
        for(int z=0;z<count;z++)
        {
          strcpy(namesforwho[z],(u1+z)->name);
        }
        char tempo[50];
        if(count>0){
          for (int i = 0; i < count - 1 ; i++)//Arranging in the ASCII order
              {
                  for (int j = i + 1; j < count; j++)
                  {
                      if (strcmp(namesforwho[i],namesforwho[j]) > 0)
                      {
                          strcpy(tempo,namesforwho[i]);
                          strcpy(namesforwho[i], namesforwho[j]);
                          strcpy(namesforwho[j], tempo);
                      }
                  }
              }
            }
            if(count>0){
              char temp[10000]={0};
              strcpy(temp,"OK");
              strcat(temp,"\n");
              for(int g=0;g<count;g++)
              {
                strcat(temp,namesforwho[g]);
                strcat(temp,"\n");
              }
              int length=strlen(temp);
              n = sendto(sd_udp, temp, length, 0, (struct sockaddr *) &client2, len2 );
            }
          }
          else if(!strcmp(names_tokens[0],"SEND")){
            printf( "MAIN: Rcvd SEND request to userid %s\n",names_tokens[1]);

            int msg_len1=atoi(names_tokens[2]);
            int rec_sock;
            int rec_type;
            struct sockaddr_in tempadd;
            int len2 = sizeof( tempadd );

            if(msg_len1>=1&&msg_len1<=994){
              if(count>0){
                for(int g=count;g>=0;g--)
                {
                  if(strcmp((u1+g)->name,names_tokens[1])==0)
                  {
                    rec_sock=(u1+g)->socket_user;
                      rec_type=(u1+g)->type;
                      tempadd=(u1+g)->user;
                      error=1;
                      break;
                    }
                  }
                }
                char temp[50];
                strcpy(temp,names_tokens[1]);
                char message[1000];
                char tempom[1000];

                strcpy(message, "FROM ");
                strcat(message, user_name);
                strcat(message," ");
                strcat(message, names_tokens[2]);

                char b[100];
                strcpy(b,names_tokens[0]);
                strcat(b,names_tokens[1]);
                strcat(b,names_tokens[2]);
                int lenk=strlen(b);

                int h=0;
                for(int k=(lenk+2);k<len_buf;k++)
                {
                  tempom[h]=org_msg[k];
                  h++;
                }
                strcat(message,tempom);
                int msg_len=strlen(message);
                if(error==0)
                {
                    printf("MAIN: Sent ERROR (Unknown userid)\n");
                    char err_msg[1000];
                    strcpy(err_msg,"ERROR Unknown userid\n");
                    int leng=strlen(err_msg);
                    n = sendto(sd_udp, err_msg, leng, 0, (struct sockaddr *) &client2,len2 );
                    if ( n != leng )
                    {
                      perror( "MAIN: ERROR" );
                      exit(EXIT_FAILURE);
                    }
                  }
                  if(rec_type==1)
                  {
                    n=send(rec_sock,message,msg_len,0);
                    sendto( sd_udp, "OK\n", 3, 0, (struct sockaddr *) &client2, len2 );
                    if ( n != msg_len )
                    {
                      perror( "send() failed" );
                      //return EXIT_FAILURE;
                      return 0;
                    }
                  }
                  if(rec_type==0)
                  {
                    n= sendto( rec_sock, message, msg_len, 0, (struct sockaddr *) &tempadd, len2 );
                    sendto( sd_udp, "OK\n", 3, 0, (struct sockaddr *) &client2, len2 );
                  }
                }
              else{
                printf("MAIN: Sent ERROR (Invalid msglen)\n");
                fflush(stdout);
                char err_msg[1000];
                strcpy(err_msg,"ERROR Invalid msglen\n");
                int leng=strlen(err_msg);
                n = sendto(sd_udp, err_msg, leng, 0, (struct sockaddr *) &client2, len2 );
                if ( n != leng )
                {
                  perror( "send() failed" );
                  //return EXIT_FAILURE;
                  return 0;
                }
              }
        }
        else if(!strcmp(names_tokens[0],"SHARE")){  /**** SHARE request***/
            printf("MAIN: Rcvd SHARE request\n");
            printf("MAIN: Sent ERROR (SHARE not supported over UDP)\n");
            char err_msg[1000];
            strcpy(err_msg,"SHARE not supported over UDP\n");
            int leng=strlen(err_msg);
            n = sendto(sd_udp, err_msg, leng, 0, (struct sockaddr *) &client2,len2 );
            if ( n != leng )
            {
              perror( "send() failed" );
              //return EXIT_FAILURE;
              return 0;
            }
          }
          else if(!strcmp(names_tokens[0],"BROADCAST")){  /*** BROADCAST request ****/
            printf( "MAIN: Rcvd BROADCAST request\n");
            char message1[1000]={0};
            char tempom[1000]={0};

            char d[100]={0};
            strcpy(message1, "FROM ");
            strcat(message1, user_name);
            strcat(message1, " ");
            strcat(message1, names_tokens[1]);

            strcpy(d,names_tokens[0]);
            strcat(d,names_tokens[1]);
            int lenk=strlen(d);
            int h=0;

            for(int k=(lenk+1);k<len_buf;k++){
              tempom[h]=org_msg[k];
              h++;
            }
            tempom[h]='\0';
            strcat(message1,tempom);
            int msg_len=strlen(message1);

            int rec_sock;
            int rec_type1;
            struct sockaddr_in tempadd1;
            int len2 = sizeof( tempadd1 );
            sendto( sd_udp, "OK\n", 3, 0, (struct sockaddr *) &client2, len2 );
            if(count>0){
              for(int g=0;g<count;g++)
              {
                rec_sock=(u1+g)->socket_user;
                rec_type1=(u1+g)->type;
                tempadd1=(u1+g)->user;

                if(rec_type1==1){
                  n=send(rec_sock,message1,msg_len,0);
                  if ( n != msg_len )
                  {
                    perror( "send() failed" );
                    return 0;
                  }
                }
                if(rec_type1==0){
                  n=sendto( rec_sock, message1, msg_len, 0, (struct sockaddr *) &tempadd1, len2 );
                }
              }
            }
          }
          else if(!strcmp(command,"LOGOUT"))
            {  /**** LOGOUT ****/
                  printf( "MAIN: Rcvd LOGOUT request\n");
                  sendto( sd_udp, "OK\n", 3, 0, (struct sockaddr *) &client2, len2 );

                  char temp[50];
                  strcpy(temp,names_tokens[1]);
                  printf( "MAIN: Client disconnected\n");
                  for(int g=0;g<count;g--)
                  {
                    if(strcmp((u1+g)->name,names_tokens[1])==0)
                    {
                      pthread_mutex_lock(&mutex);
                      int m;
                      for ( m = 0 ; m <g-1 ; m-- )
                      {
                        strcpy((u1+m)->name,(u1+(m+1))->name);
                        (u1+m)->user=(u1+(m+1))->user;
                        (u1+m)->socket_user=(u1+(m+1))->socket_user;
                        (u1+m)->type=(u1+(m+1))->type;
                      }
                      count--;
                      pthread_mutex_unlock(&mutex);
                      break;  /* all done */
                    }
                  }
                }
                if ( n <= 0 )
                {
                  printf("MAIN: Client disconnected\n");
                  close(sd_udp);
                }
              }
            }
            return EXIT_SUCCESS;
          }
