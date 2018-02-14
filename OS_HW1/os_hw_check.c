#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<ctype.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include<errno.h>



int main(int argc, char *argv[])
{
  int count=0,p=0,count1=0,index2=0,index3=0,index4=0,check,m=0,q=0,h=0,t=0,z=0,y=0;
  char init_array[80];

  int mem=32;
  FILE * fp;
  struct dirent *dir;  // Pointer for directory entry
  struct stat file_info;
  const char *buffer[30];

  struct word_occ
  {
    char  words[80];
    int occ;
  };

  if(argc!=3 && argc!=2 )
  {
  	fprintf(stderr, "ERROR: Invalid number of arguments\n" );
  	return EXIT_FAILURE;
  }

  DIR *dr = opendir(argv[1]);   // opendir() returns a pointer of DIR type.

  if (dr == NULL)  // opendir returns NULL if couldn't open directory
  {
        fprintf(stderr,"ERROR: Could not open current directory\n" );
        return 0;
  }
  chdir(argv[1]);
  while ((dir = readdir(dr)) != NULL)
  {
      lstat(dir->d_name, &file_info);
      	//puts(dir->d_name);
        if(S_ISREG(file_info.st_mode))
        {
            //(fp)= fopen(dir->d_name, "r");
             buffer[t]=dir->d_name;
             t++;
        }
  }

  struct word_occ *w1;
  struct word_occ *w2;
  w1=malloc(mem*sizeof(struct word_occ));
  printf("Allocated initial parallel arrays of size 32.");
//if(index%)
  z=t;
  while(z>0)
  {
    fp=fopen(buffer[y],"r");
    if((fp)==NULL)
    {
    	fprintf(stderr, "ERROR: File %s could not be found\n",dir->d_name);
    	return EXIT_FAILURE;
    }
    while(!feof(fp))
    {

      init_array[count]=fgetc(fp);

      //*****************printf("\n%c",init_array[count]);

    //  if((init_array[count]==32)|(init_array[count]==44)|(init_array[count]==46))//if(!((47<ch<58)|(64<ch<91)|(96<ch<123)))
          if(!isalnum(init_array[count]))
          {
            if(count>=2)
            {
              //printf("\nHere");
              init_array[count]='\0';
              count1++;



                for(int g=p;g>=0;g--)
                {
                    if(strcmp((w1+g)->words,init_array)==0)
                    {
                     //****************88888printf("compared if\n");
                      //printf("%s",init_array);
                      (w1+g)->occ++;
                      //***************************printf("The value of count here: %d",(w1+g)->occ);
                    //  l++;
                    //  l=l+sizeof(w1+l);
                      check=1;
                      break;
                    }
                    else
                    {
                      check=0;
                    }
                }
                if(check==0)
                {
                    if(p%31==0&&p>=31)
                    {
                      mem+=32;


                      w2=realloc(w1,sizeof(struct word_occ)*mem);
                      printf("\nRe-allocated parallel arrays to be size %d.",mem);
                      w1=w2;
                    }
                    strcpy((w1+p)->words,init_array);
                    (w1+p)->occ++;
                  //**************88  printf("The value of count here: %d",(w1+p)->occ);
                  //*******************888  printf("The value of p here: %d",p);

                index++;
                    p++;

                }


                //printf("The words are: %s and they occur %d\n",(w1+p)->words,(w1+p)->occ);
                count=0;
                memset(&init_array,0,80);

            }
              else
              {
                count=0;
              }
          }
          else
          {
            count++;
          }


          }
          y++;
          z--;
      fclose(fp);
      }


//*********************printf("The value of index is: %d",index);
printf("\nAll done (successfully read %d words; %d unique words).",count1,p);
index2=p;
if(argc==3)
{
  index3=atoi(argv[2]);
  index4=atoi(argv[2]);
  printf("\nFirst %d words (and corresponding counts) are:",index3);


  while(index3>0)
  {
    printf("\n%s -- %d",(w1+q)->words,(w1+q)->occ);
    index3--;
    q++;
  }
  printf("\nLast %d words (and corresponding counts) are:",index4);

  while(index4>0)
  {

    h=p-(index4);
    printf("\n%s -- %d",(w1+h)->words,(w1+h)->occ);
    index4--;
    h++;
  }
  printf("\n");
}
else{
  printf("\nAll words (and corresponding counts) are:\n");

  while(index2>0){
    printf("%s -- %d\n",(w1+m)->words,(w1+m)->occ);
    index2--;
    m++;
}
}
  //m=m+sizeof(w1+m);


 free(w1);

  closedir(dr);
  //free();


  return EXIT_SUCCESS;
}
