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

void move(int,int,int,int,int p[], int **);



void move(int m,int n, int max1, int max2, int p[], int ** b)
{
  int parent_pid;
  int row=m;
  int column=n;
  int ar[max1][max2];//********This array is just to maintain moves everytime************
  //int p[2];
  //pipe(p);
  //printf("PID: %d Reached here, %d,%d\n",getpid(),row,column);
  int count=0;
  int count1=0;

/*printf("PID %d: Prinitng the board to check the moves\n",getpid());
  for(int m=0;m<max1;m++){
    for(int n=0;n<max2;n++){
      printf("%d",b[m][n]);
  }
  printf("PID %d:",getpid());
  printf("\n");
}*/

  for(int m=0;m<max1;m++){
    for(int n=0;n<max2;n++){
      ar[m][n]=0;


    }
  }

  if(m==0 && n==0){// Filling the array that maintains if visited or not, with zeroes
    for(int m=0;m<max1;m++){
      for(int n=0;n<max2;n++){
        b[m][n]=0;
        parent_pid=getpid();
      }
    }
  }

    b[0][0]=1;

//  count=check_move(m,n,max1,max2,*);
  //check how many moves are possible
  if((row+2)<max1)
  {
    if((column+1)<max2&&(row+2)<max1)
    {
      if(b[row+2][column+1]==0)
      {
        //ar[row][column]=move((row+2),(column+1),max1,max2)+1;
        ar[row+2][column+1]=2;
        count++;
      }
    }
    if((column-1)>=0&&(row+2)<max1)
    {
      if(b[row+2][column-1]==0)
      {
        //ar[row][column]=move((row+2),(column-1),max1,max2)+1;
        ar[row+2][column-1]=2;
        count++;
      }
    }
  }
  if((row-2)>=0)
  {

    if((column+1)<max2&&(row-2)>=0)
    {
      if(b[row-2][column+1]==0)
      {
        //ar[row][column]=move((row-2),(column+1),max1,max2)+1;
        ar[row-2][column+1]=2;
        count++;
      }
    }
    if((column-1)>=0&&(row-2)>=0)
    {
      if(b[row-2][column-1]==0)
      {
        //ar[row][column]=move((row-2),(column-1),max1,max2)+1;
        ar[row-2][column-1]=2;
        count++;
      }
    }
  }
  if((column+2)<max2)
  {

    if((row-1)>=0&&(column+2)<max2)
    {
      if(b[row-1][column+2]==0)
      {
      //  ar[row][column]=move((row-1),(column+2),max1,max2)+1;
      ar[row-1][column+2]=2;
      count++;
      }
    }
    if((row+1)<max1&&(column+2)<max2)
    {
      if(b[row+1][column+2]==0)
      {
      //  ar[row][column]=move((row+1),(column+2),max1,max2)+1;
      ar[row+1][column+2]=2;
      count++;
      }
    }
  }
  if((column-2)>=0)
  {

    if((row-1)>=0&&(column-2)>=0)
    {
      if(b[row-1][column-2]==0)
      {
        //ar[row][column]=move((row-1),(column-2),max1,max2)+1;
        ar[row-1][column-2]=2;
        count++;
      }
    }
    if((row+1)<max1&&(column-2)>=0)
    {
      if(b[row+1][column-2]==0)
      {
        //ar[row][column]=move((row+1),(column-2),max1,max2)+1;
        ar[row+1][column-2]=2;
        count++;
      }
    }
  }
  for(int m=0;m<max1;m++)
  {
    for(int n=0;n<max2;n++)
    {
      if(b[m][n]==1)count1++;
    }
  }

fflush(stdout);
/*printf("PID temp %d: Prinitng the temprorary board to check the moves\n",getpid());
  for(int m=0;m<max1;m++){
    for(int n=0;n<max2;n++){
      printf("%d",ar[m][n]);
  }
  printf("PID temp %d:",getpid());
  printf("\n");
}*/
//if(count>1)
  //printf("PID %d: %d moves possible after move #%d",getpid(),count,count1);
  if(count==0)//****************if there are no further moves possible .....send to parent****************************
  {
    printf("PID %d: Dead end after move #%d\n",getpid(),count1);
    #ifdef DISPLAY_BOARD
    for(int m=0;m<max1;m++){
      for(int n=0;n<max2;n++){
        if(b[m][n]==1)
        {
          printf("PID: %d k ",getpid());
        }
    }
    //printf("PID temp %d:",getpid());
    printf("\n");
  }
    #endif


    write(p[1],&count1,sizeof(count1));
    printf("PID %d: Sent %d on pipe to parent\n",getpid(),count1);
    fflush(stdout);
    exit (1);
    //return 1;
  }


  if(count>1)// *************Defining the fork process if there are more than one moves possible.*************
  {
pid_t pid;
int p1[2];
pipe(p1);
      printf("PID %d: %d moves possible after move #%d\n",getpid(),count,count1);
      #ifdef DISPLAY_BOARD
      for(int m=0;m<max1;m++){
        for(int n=0;n<max2;n++){
          if(b[m][n]==1)
          {
            printf("PID: %d k ",getpid());
          }
      }
      //printf("PID temp %d:",getpid());
      printf("\n");
    }
      #endif
          //printf("Forked2");

      for(int m=0;m<max1;m++){
        for(int n=0;n<max2;n++){//  check and send the new potential coordinates
          if(ar[m][n]==2)
          {
            //printf("Forked1");

            b[m][n]=1;
            fflush(stdout);
            pid=fork();

          //  printf("Forked");

            if(pid==0)//**************child process********************
            {


              //printf("CHILD1: %d",getpid());
              //printf("p[0]: %d, p[1]: %d\n",p[0],p[1]);



              //b1[0][0]=1;
            //  b1[m][n]=1;
              //printf("CHILD2: %d\n",getpid());
              //printf("values of m is %d and n is %d",m,n);

              //sq=squares(m,n,max1,max2,b);
              move(m,n,max1,max2,p1,b);


              //No need to write in the pipe here*************
            //write(p[1],&sq,sizeof(sq));
              //printf("PID %d: Sent %d on pipe to parent",getpid(),sq);
              exit(1);
            }
          }
        }
      }
            if(pid>0){//***************Parent process**********************
            //no_parallel to be added here

              //printf("PID %d: Waiting for all children to terminate\n",getpid());
              int child_values[20]={0};
              int l=0;
              int temp;
              int temp_count=count;
              int status;
              //wait(&status);
              while(temp_count>0)
              {
                wait(&status);
                int num;
                read(p1[0],&num,sizeof(num));
                //memcpy(&num1,num,sizeof(int));
                child_values[l]=num;
                printf("PID %d: Received %d from child\n",getpid(),num);
                l++;
                temp_count--;
              }
              for(int h=0;h<20;h++)
              {

                if(child_values[h]>child_values[h+1])
                {
                  temp=child_values[h];
                  child_values[h]=child_values[h+1];
                  child_values[h+1]=temp;
                }
              }
              if(getpid()==parent_pid)
              {
                int pro=max1*max2;
                printf("PID %d: Best solution found visits %d squares (out of %d)\n",getpid(),child_values[19],pro);
              }
              else{
              write(p[1],&child_values[19],sizeof(child_values[19]));
              printf("PID %d: All child processes terminated; sent %d on pipe to parent\n",getpid(),child_values[19]);
              //return child_values[19];
              //exit(1);
            }

          }
          //  #endif
}


  if(count==1)
  {
//printf("values in nowhere of m is %d and n is %d\n",m,n);
    for(int m=0;m<max1;m++){
      for(int n=0;n<max2;n++){//  check and send the new potential coordinates
        if(ar[m][n]==2){
          b[m][n]=1;

        move(m,n,max1,max2,p,b);
       }
        }
      }
  }

//return EXIT_SUCCESS;
}

int main(int argc,char *argv[])
{
  //int row,column;

  //int square;
  int max1,max2;


  max1=(int)atoi(argv[1]);
  max2=(int)atoi(argv[2]);
  int p[2];
  pipe(p);
  int **b;
  //p[1]=-1;
  if( argc<3||max1<3||max2<3)
  {
    fprintf(stderr, "ERROR: Invalid argument(s)\n" );
    fprintf(stderr, "USAGE: a.out <m> <n>\n");
  	return EXIT_FAILURE;
  }
  b=(int**)malloc(sizeof (int *) * max1);//**********Dynamically allocating memory to the array for storing moves*************
  b[0]=(int *)malloc(sizeof (int ) * max2*max1);

  for(int i=0;i<max1;i++){
    b[i]=(*b+max2*i);
  }
  printf("PID %d: Solving the knight's tour problem for a %dx%d board\n",getpid(),max1,max2);

  move(0,0,max1,max2,p,b);
//printf("The value of square is %d\n",square);
/*  wait(&status);
  int number;// other parent process
  error_in_read=read(p[0],&number,sizeof(number));
  printf(" This is the error in read %d\n",error_in_read);
  printf("PID %d: Received %d (original parent) from child\n",getpid(),number);
  int pro=max1*max2;
  printf("PID %d: Best solution found visits %d squares (out of %d)\n",getpid(),number,pro);*/
  free(b);
  return EXIT_SUCCESS;
}
