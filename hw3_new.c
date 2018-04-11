#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<sys/wait.h>
#include<string.h>
#include<ctype.h>
#include<dirent.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<errno.h>
#include<pthread.h>

struct parameters{
  int m;
  int n;
  int max1;
  int max2;
  int **b;
};

int max_squares=0;
int ***dead_end_boards;
int dead_ends=0;
int num_threads=0;
pthread_mutex_t mutex=PTHREAD_MUTEX_INITIALIZER;

void *move(void *);


void *move(void *arg)
{
  struct parameters *p=(struct parameters *)arg;
  int max1=p->max1;
  int max2=p->max2;
  int row=p->m;
  int column=p->n;
  int count=0;
  int count1=0;
  pthread_t tid[1000];

  int ar[max1][max2];//********This array is just to maintain moves everytime************

  p->b[row][column]=1;//** first move**

  for(int m=0;m<max1;m++){//**This array maintains if there are possible moves and therefore assign 0 everytime**
    for(int n=0;n<max2;n++){
      ar[m][n]=0;
    }
  }
  if(row==0 && column==0){// Filling the array that maintains if visited or not, with zeroes
    tid[num_threads]=pthread_self();
    for(int m=0;m<(p->max1);m++){
      for(int n=0;n<(p->max2);n++){
        p->b[m][n]=0;
      }
    }
  }

  p->b[0][0]=1;//** first move**

  //check how many moves are possible
  if((row+2)<max1)
  {
    if((column+1)<max2&&(row+2)<max1)
    {
      if(p->b[row+2][column+1]==0)
      {
        ar[row+2][column+1]=2;
        count++;
      }
    }
    if((column-1)>=0&&(row+2)<max1)
    {
      if(p->b[row+2][column-1]==0)
      {
        ar[row+2][column-1]=2;
        count++;
      }
    }
  }
  if((row-2)>=0)
  {
    if((column+1)<max2&&(row-2)>=0)
    {
      if(p->b[row-2][column+1]==0)
      {
        ar[row-2][column+1]=2;
        count++;
      }
    }
    if((column-1)>=0&&(row-2)>=0)
    {
      if(p->b[row-2][column-1]==0)
      {
        ar[row-2][column-1]=2;
        count++;
      }
    }
  }
  if((column+2)<max2)
  {

    if((row-1)>=0&&(column+2)<max2)
    {
      if(p->b[row-1][column+2]==0)
      {
        ar[row-1][column+2]=2;
        count++;
      }
    }
    if((row+1)<max1&&(column+2)<max2)
    {
      if(p->b[row+1][column+2]==0)
      {
        ar[row+1][column+2]=2;
        count++;
      }
    }
  }
  if((column-2)>=0)
  {
    if((row-1)>=0&&(column-2)>=0)
    {
      if(p->b[row-1][column-2]==0)
      {
        ar[row-1][column-2]=2;
        count++;
      }
    }
    if((row+1)<max1&&(column-2)>=0)
    {
      if(p->b[row+1][column-2]==0)
      {
        ar[row+1][column-2]=2;
        count++;
      }
    }
  }
  for(int m=0;m<max1;m++)//** To count the number of move**
  {
    for(int n=0;n<max2;n++)
    {
      if(p->b[m][n]==1)count1++;
    }
  }

fflush(stdout);

if(count==0)//****************if there are no further moves possible****************************
{
  printf("THREAD %u: Dead end after move #%d\n",(unsigned int)pthread_self(),count1);
  fflush(stdout);
  
  pthread_mutex_lock(&mutex);//**  prevent synchronization error **
  if(count1>max_squares)//** updating the max_squares if the squares covered are greater thn the maximum**
  {
    max_squares=count1;
  }

  for(int i=0;i<max1;i++)//** updating the dead_end_boards**
  {
    for(int j=0;j<max2;j++)
    {
      dead_end_boards[dead_ends][i][j]=p->b[i][j];//**updating the global array**
    }
  }
  dead_ends++;//** to know the count of dead end boards**
  pthread_mutex_unlock(&mutex);
  
  unsigned int *y=malloc(sizeof(unsigned int));
  *y=pthread_self();
  pthread_exit(y);
  free(y); 
}
else if(count==1) //** continue with the same thread**
{
  for(int m=0;m<max1;m++)
  {
    for(int n=0;n<max2;n++)
    {//  check and send the new potential coordinates
      if(ar[m][n]==2)
      {
        p->b[m][n]=1;
        p->m=m;
        p->n=n;

        move(p);
      }
    }
  }
}
else// ** Defining the different threads if there are more than one moves possible, and create threads**
{
    printf("THREAD %u: %d moves possible after move #%d; creating threads\n",(unsigned int)pthread_self(),count,count1);
    fflush(stdout);
    
  for(int m=0;m<max1;m++){
      for(int n=0;n<max2;n++){//  check and send the new potential coordinates
        if(ar[m][n]==2)
        {
          struct parameters *p2; //** Dynamically allocating and updating the structure to be sent further
          p2=malloc(sizeof(struct parameters)*1);

          p2->m=m;
          p2->n=n;
          p2->max1=max1;
          p2->max2=max2;

          p2->b=(int**)malloc(sizeof (int *) * max1);//**********Dynamically allocating memory to the array for storing moves*************
          p2->b[0]=(int *)malloc(sizeof (int ) * max2*max1);

          for(int i=0;i<max1;i++){
            p2->b[i]=(p2->b[0]+max2*i);
          }

           for(int i=0;i<max1;i++)
           {
             for(int j=0;j<max2;j++)
             {
               p2->b[i][j]=p->b[i][j];//**updating the array to be sent to the child thread**
             }
           }
           num_threads++;
           int rc=pthread_create(&tid[num_threads],NULL,move,p2);
           if(rc!=0)
           {
             fprintf( stderr, "Could not create thread (%d)\n",rc);
             //return EXIT_FAILURE;
           }
           //free(p2);
           unsigned int *x=NULL;
           int rc2=pthread_join(tid[num_threads],(void**)&x);
           if(rc2!=0)
           {
             fprintf( stderr, "Could not join thread (%d)\n",rc2);
           }
           free(x);
           free(p2);
         }
       }
     }
}
return 0;
}

int main(int argc,char *argv[])
{
  if( argc<3||argc>4)
  {
    fprintf(stderr, "ERROR: Invalid argument(s)\n" );
    fprintf(stderr, "USAGE: a.out <m> <n> [<k>]\n");
    return EXIT_FAILURE;
  }

  int max1,max2,max3;

  max1=(int)atoi(argv[1]);
  max2=(int)atoi(argv[2]);

  int pro1=max1*max2;

  if(max1<3||max2<3)
  {
    fprintf(stderr, "ERROR: Invalid argument(s)\n" );
    fprintf(stderr, "USAGE: a.out <m> <n> [<k>]\n");
    return EXIT_FAILURE;
  }
  if(argc==4){
    max3=(int)atoi(argv[3]);

  if(max3>pro1||max3<=0)
  {
    fprintf(stderr, "ERROR: Invalid argument(s)\n" );
    fprintf(stderr, "USAGE: a.out <m> <n> [<k>]\n");
    return EXIT_FAILURE;
  }
}
  struct parameters *p1;
  p1=malloc(sizeof(struct parameters)*1);
  
  pthread_t parent_tid=pthread_self();
  
  p1->b=(int**)malloc(sizeof (int *) * max1);//**Dynamically allocating memory to the array for storing moves**
  p1->b[0]=(int *)malloc(sizeof (int ) * max2*max1);

  for(int i=0;i<max1;i++){
    p1->b[i]=(p1->b[0]+max2*i);
  }

  dead_end_boards=(int ***)malloc(sizeof(int **)*900);//**Dynamically allocating memory for dead boards**
  if(dead_end_boards==0)
  {
    printf("Not enough memory space");
    abort();
  }

  for(int z=0;z<900;z++)
  {
    dead_end_boards[z]=(int **)malloc(sizeof(int *)*max1);
    for(int r=0;r<max1;r++)
    {
      dead_end_boards[z][r]=(int *)malloc(sizeof(int)*max2);
    }
  }
  printf("THREAD %u: Solving the knight's tour problem for a %dx%d board\n",(unsigned int)pthread_self(),max1,max2);

  p1->m=0;
  p1->n=0;
  p1->max1=max1;
  p1->max2=max2;

  move(p1);
  
  if(pthread_equal(parent_tid,pthread_self()))
  {
    int pro=max1*max2;
    printf("THREAD %u: Best solution found visits %d squares (out of %d)\n",(unsigned int)pthread_self(),max_squares,pro);
    fflush(stdout);
  }

if(argc==4){

  int ar[dead_ends];
  int z=0;

  for(int g=0;g<dead_ends;g++){
    for(int m=0;m<max1;m++){
      for(int n=0;n<max2;n++){
        if(dead_end_boards[g][m][n]==1)
        {

          z++;
        }
      }
    }
    ar[g]=z;
    z=0;
  }
  for(int g=dead_ends-1;g>=0;g--){
    if(ar[g]>=max3){
      for(int m=0;m<max1;m++){
        if(m==0)
        {
          printf("THREAD %u: > ",(unsigned int)pthread_self());

        }
        else{
        printf("THREAD %u:   ",(unsigned int)pthread_self());
        }
        for(int n=0;n<max2;n++){
          if(dead_end_boards[g][m][n]==1)
          {
            printf("k");
          }
          else{
            printf(".");
          }
        }
        printf("\n");
        fflush(stdout);
      }
    }
  }
  fflush(stdout);
}
else{
  for(int g=dead_ends-1;g>=0;g--){
      for(int m=0;m<max1;m++){
        if(m==0)
        {
          printf("THREAD %u: > ",(unsigned int)pthread_self());
        }
        else{
        printf("THREAD %u:   ",(unsigned int)pthread_self());
        }
        for(int n=0;n<max2;n++){
          if(dead_end_boards[g][m][n]==1)
          {
            printf("k");
          }
          else{
            printf(".");
          }
        }
        printf("\n");
        fflush(stdout);
      }
  }
  fflush(stdout);
}
  free(p1);
  free(dead_end_boards);
  return EXIT_SUCCESS;
}
