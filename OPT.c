#include <stdio.h>
#include <stdlib.h>
int findLRU(int time[], int n){
	int i, minimum = time[0], pos = 0;
 
	for(i = 1; i < n; ++i){
		if(time[i] < minimum){
			minimum = time[i];
			pos = i;
		}
	}
	
	return pos;
}
int main()
{
    //Declarations--------------------------------------------------------------------------------------------------------
    int no_of_frames, no_of_pages; //Overlapping
	int frames_LRU[10], pages_LRU[30], counter = 0, time[10], flag1_LRU, flag2_LRU, i, j, pos_LRU, faults_LRU = 0; //LRU
    int frames_OPT[10], pages_OPT[30], temp[10], flag1_OPT, flag2_OPT, flag3_OPT, k, pos_OPT, max, faults_OPT = 0; //OPT
    int a[50],frame[10],avail,count=0; //FIFO
    int p[100],f[10],ava,hit=0,usebit[10]; //CLOCK
    
	//File Reading--------------------------------------------------------------------------------------------------------
   char ch, file_name[25];
   FILE *fp;

   printf("Enter name of a file you wish to see\n");
   gets(file_name);

   fp = fopen(file_name, "r"); // read mode

   if (fp == NULL)
   {
      perror("Error while opening the file.\n");
      exit(EXIT_FAILURE);
   }

   printf("The contents of %s file are:\n", file_name);

   while((ch = fgetc(fp)) != EOF)
      printf("%c", ch);

   fclose(fp);
   return 0;
   //Example Inputs------------------------------------------------------------------------------------------------------
   printf("Enter number of frames: ");
	scanf("%d", &no_of_frames);
	
	printf("Enter number of pages: ");
	scanf("%d", &no_of_pages);
	
	printf("Enter reference string: ");
	
    for(i = 0; i < no_of_pages; ++i){
    	scanf("%d", &pages_LRU[i]);
    	pages_OPT[i] = pages_LRU[i];
    	a[i] = pages_LRU[i];
    	p[i] = pages_LRU[i];
    	
    }
   
   //LRU-----------------------------------------------------------------------------------------------------------------
    
	for(i = 0; i < no_of_frames; ++i){
    	frames_LRU[i] = -1;
    }
    
    for(i = 0; i < no_of_pages; ++i){
    	flag1_LRU = flag2_LRU = 0;
    	
    	for(j = 0; j < no_of_frames; ++j){
    		if(frames_LRU[j] == pages_LRU[i]){
	    		counter++;
	    		time[j] = counter;
	   			flag1_LRU = flag2_LRU = 1;
	   			break;
   			}
    	}
    	
    	if(flag1_LRU == 0){
			for(j = 0; j < no_of_frames; ++j){
	    		if(frames_LRU[j] == -1){
	    			counter++;
	    			faults_LRU++;
	    			frames_LRU[j] = pages_LRU[i];
	    			time[j] = counter;
	    			flag2_LRU = 1;
	    			break;
	    		}
    		}	
    	}
    	
    	if(flag2_LRU == 0){
    		pos_LRU = findLRU(time, no_of_frames);
    		counter++;
    		faults_LRU++;
    		frames_LRU[pos_LRU] = pages_LRU[i];
    		time[pos_LRU] = counter;
    	}
    	
    	printf("\n");
    	
    	for(j = 0; j < no_of_frames; ++j){
    		printf("%d\t", frames_LRU[j]);
    	}
	}
	
	printf("\n\nTotal Page Faults = %d", faults_LRU);
  
   //OPT------------------------------------------------------------------------------------------------------------------
   

    for(i = 0; i < no_of_frames; ++i){
        frames_OPT[i] = -1;
    }
    
    for(i = 0; i < no_of_pages; ++i){
        flag1_OPT = flag2_OPT = 0;
        
        for(j = 0; j < no_of_frames; ++j){
            if(frames_OPT[j] == pages_OPT[i]){
                   flag1_OPT = flag2_OPT = 1;
                   break;
               }
        }
        
        if(flag1_OPT == 0){
            for(j = 0; j < no_of_frames; ++j){
                if(frames_OPT[j] == -1){
                    faults_OPT++;
                    frames_OPT[j] = pages_OPT[i];
                    flag2_OPT = 1;
                    break;
                }
            }    
        }
        
        if(flag2_OPT == 0){
        	flag3_OPT =0;
        	
            for(j = 0; j < no_of_frames; ++j){
            	temp[j] = -1;
            	
            	for(k = i + 1; k < no_of_pages; ++k){
            		if(frames_OPT[j] == pages_OPT[k]){
            			temp[j] = k;
            			break;
            		}
            	}
            }
            
            for(j = 0; j < no_of_frames; ++j){
            	if(temp[j] == -1){
            		pos_OPT = j;
            		flag3_OPT = 1;
            		break;
            	}
            }
            
            if(flag3_OPT ==0){
            	max = temp[0];
            	pos_OPT = 0;
            	
            	for(j = 1; j < no_of_frames; ++j){
            		if(temp[j] > max){
            			max = temp[j];
            			pos_OPT = j;
            		}
            	}            	
            }
			
			frames_OPT[pos_OPT] = pages_OPT[i];
			faults_OPT++;
        }
        
        printf("\n");
        
        for(j = 0; j < no_of_frames; ++j){
            printf("%d\t", frames_OPT[j]);
        }
    }
    
    printf("\n\nTotal Page Faults = %d", faults_OPT);
   //FIFO-------------------------------------------------------------------------------------------------------------
   
   
   		for(i=0;i<no_of_frames;i++)
            frame[i]= -1;
            j=0;
            printf("\tref string\t page frames\n");
      	for(i=1;i<=no_of_pages;i++)
        {
            printf("%d\t\t",a[i]);
            avail=0;
            for(k=0;k<no_of_frames;k++)
          	if(frame[k]==a[i])
            	avail=1;
            if (avail==0)
            {
                frame[j]=a[i];
                j=(j+1)%no_of_frames;
                count++;
            	for(k=0;k<no_of_frames;k++)
                printf("%d\t",frame[k]);
         	}		
        	printf("\n\n");
       	}
        printf("Page Fault Is %d",count);
   //CLOCK--------------------------------------------------------------------------------------------
 	for(i=0;i<no_of_pages;i++)
 	scanf("%d",&p[i]); 
 	for(i=0;i<no_of_pages;i++)
 	{
  		ava=0;
    		// found
  			for(j=0;j<3;j++)
  			{
   				if(p[i]==f[j])
   				{
    				ava=1;
    				hit++;
    				usebit[j]=1;
    				break;
   				}
  			}
    	//search for usebit 0
  		if(ava==0)
  		{
   			for(j=0;j<3;j++)
   			{
    			if(usebit[j]==0)
    			{			
     				f[j]=p[i];
     				usebit[j]=1;
     				ava=1;
     				break;
   		 		}
   			}
  		}
    	// fifo
  		if(ava==0)
  		{
   			for(j=0;j<3;j++)
   			usebit[j]=0;
  		}
  		f[0]=p[i];
  		usebit[0]=1;
 	} 
 	printf("The number of Hits: %d",hit);
 	return 0;
}
