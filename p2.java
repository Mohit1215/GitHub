import java.io.BufferedReader;
import java.io.FileNotFoundException;
import java.io.FileReader;
import java.io.IOException;
import java.util.ArrayList;
import java.util.List;

class Project2 {

	int clock=0;

	public static void main(String args[]) {
		FileReader fr=null;
		String Process_split[]=null;
		String Process_split1[]=null;
		Process p=null;
		String delim1=null;
		String[][] new_ar=null;




		try {
			fr= new FileReader(args[0]);
		}
		catch(FileNotFoundException e) {
			System.out.println("File not found");
			return;
		}

		try {
			BufferedReader br= new BufferedReader(fr);
			List<Process> list=new ArrayList<Process>();
			String Process_str;

			while((Process_str=br.readLine())!=null){

				char currentChar=Process_str.charAt(0);
				if(currentChar!='#'||currentChar!='\n'){
					//System.out.println(Process_str+"It entered correctly");
					String delim = " ";
					Process_split=Process_str.split(delim);
					int length=Process_split.length;

					p=new Process();
				    p.setPid(Process_split[0]);
				    p.setNumFrames(Integer.parseInt(Process_split[1]));

				   // p.set_arr_time(new_ar);
				    new_ar=p.get_arr_time_ar();
				    delim1="[/]";
				    //int length1=new_ar.length;
				    if(length>3) {
				    	new_ar=new String[length-2][2];


				    	for(int j=0;j<(length-2);j++) {
				    		Process_split1=Process_split[j+2].split(delim1);
				    		//for(int m=0;m<new_ar.length;m++) {
				    		 for(int k=0;k<2;k++) {
				    			 new_ar[j][k]=Process_split1[k];
				    		// }
					    	}
				    	}
				    	p.set_arr_time(new_ar);
				    	p.setNum_arr(length-2);
				    }
				    else {
				    	Process_split1=Process_split[2].split(delim1);
				    	new_ar=new String[1][2];
				    	for(int k=0;k<2;k++) {
			    			 new_ar[0][k]=Process_split1[k];
			    		 }
				    	p.set_arr_time(new_ar);
				    	p.setNum_arr(1);

				    }
					list.add(p);

			}

			}
			int size=list.size();
			//System.out.println("the first object is"+list.get(size-1).getPid());
			br.close();
			Paging page=new Paging();
			String[] new1_ar=page.get_memory();
			for(int h=0;h<256;h++) {//Setting the memory initially to dots to represent empty memory
				new1_ar[h]=".";
			}
			page.set_memory(new1_ar);//This is setting the initial memory to the memory.
			System.out.println("time 0ms: Simulator started (Non-contiguous)");
			page.paging(list);
			/*Check for the time currently
			 * First check if any process has to be removed
			 * Then check if any process has newly arrived
			 * if none, simulator ended*/


		}
		 catch(IOException e)
		  {
		    e.printStackTrace();
		  }


	}
}

class Paging {

	private String[] memory=new String[256];
	int count_mem=256;
	int len;
	String[][] new_ar;
	int[][] pageTable_ar=new int[256][2];;
	Project2 main2=new Project2();
	int burst;

	 public String[] get_memory() {
	       return memory;
	   }

	   public void set_memory(String[] memory) {
	       this.memory = memory;
	   }


	public void paging(List <Process> list) {
		while(2>1) {
		int size=list.size();
		for(int j=0;j<size;j++) {
			Process p=list.get(j);
			new_ar=p.get_arr_time_ar();//For each process get the array of the arrival times
			len= p.getNum_arr();// check if multiple arrivals are there for that process
			//burst=p.getburst();
			int status=p.getOcc();
					for(int k=0;k<len;k++) {
						burst=Integer.parseInt(new_ar[k][1]);
						if(Integer.parseInt(new_ar[k][0])==main2.clock) {
							//if(p.getNumFrames()<=count_mem) {
								//System.out.println("The value of count_mem is:"+count_mem);
									this.allocate(p,list);//allocate will be the method to allocate the frames on the memory
							//}

						}
						else if(Integer.parseInt(new_ar[k][0])<main2.clock) {

							if((burst+Integer.parseInt(new_ar[k][0]))==main2.clock){
								//System.out.println("Reached to remove");
								if(status==1) {
								this.remove(p,list);
							}
							}
						}
						if(main2.clock>0 && count_mem==256) {
							System.out.println("time "+main2.clock+"ms: Simulator ended (Non-contiguous)");
							return;
						}
					}
		}
		main2.clock++;
		}
	}
	 public void allocate(Process p,List<Process> list1) {
		 int n_frames=p.getNumFrames();
		 int list_size=list1.size();
		 String name=p.getPid();
		 System.out.println("time "+main2.clock+"ms: Process "+name+" arrived (requires "+n_frames+" frames)");
		 //System.out.println("The time is: "+main2.clock);
		 int init=256-count_mem;
		 //int init2=init;
		int pk=0;
		 if(n_frames<=count_mem) {
			 System.out.println("time "+main2.clock+"ms: Placed process "+name+":");
			 p.setOcc(1);
			 pageTable_ar=p.get_pageTable_ar();
			 pageTable_ar=new int[256][2];
			 for(int z=0;z<32;z++) {
				 System.out.print("=");
			 }
			 for(int l=0;l<n_frames;l++) {//Allocated memory to a process
				 //System.out.println("VAlue of init "+init+":");
				 //System.out.println("Valueof init2 "+init2+":");
				 //System.out.println("Number of frames: "+n_frames+":");
				 for(int l1=0;l1<256;l1++) {//Allocated memory to a process

					 if(memory[l1]==".") {
					memory[l1]=name;
					 count_mem--;
					 pageTable_ar[l][0]=pk++;
					 pageTable_ar[l][1]=l1;
					 break;
					 }
					 //System.out.println("Came here1");
					 //count_mem++;
					/* pageTable_ar[l][0]=pk++;
					 pageTable_ar[l][1]=init2++;*/
					 //System.out.println("Came here2");
				 }

				/***** memory[init2]=p.getPid();
				 //System.out.println("Came here1");
				 count_mem--;
				 pageTable_ar[l][0]=pk++;
				 pageTable_ar[l][1]=init2++;
				 //System.out.println("Came here2");*****/
			 }
			 p.set_pageTable_ar(pageTable_ar);// Setting the page table for each process
			 System.out.print("\n");
			 //init2=init;
			 for(int g=0;g<256;g++) {//Displaying the memory
				 if(g%32==0&&g>0) {
					 System.out.println();
				 }
				 System.out.print(memory[g]);
			 }
			 System.out.print("\n");
			 for(int z=0;z<32;z++) {
				 System.out.print("=");
			 }
			 System.out.print("\n");
			 System.out.println("PAGE TABLE [page,frame]:");

			 //init2=0;
			 for(int p1=0;p1<list_size;p1++) {//Displaying the page table whenever a process is in memory

				 Process p2=list1.get(p1);
				 n_frames=p2.getNumFrames();
				pageTable_ar=p2.get_pageTable_ar();

				 if(p2.getOcc()==1) {
					 System.out.print(p2.getPid()+": ");
					 for(int i1=0;i1<n_frames;i1++) {
						 if(i1%10==0&&i1>0) {
							 System.out.println();
						 }
						// for(int i2=0;i2<2;i2++) {
						 System.out.print("["+pageTable_ar[i1][0]+","+pageTable_ar[i1][1]+"]" );

						//System.out.print("["+i1+","+(init2++)+"] ");
						 if(i1%10!=9) {
							 System.out.print(" ");
						 }

					 }

					 /*for(int u1=0;u1<(init-1);u1++) {
						 if(u1%10==0&&u1>0) {
							 System.out.println();
						 }
						System.out.print("["+pageTable[u1][0]+","+pageTable[u1][1]+"] ");
					 }*/
					 System.out.print("\n");
				 }

			 }
			 //System.out.print("\n");
			 this.set_memory(memory);


		 }
		 else {
			 System.out.println("time "+main2.clock+"ms: Cannot place process "+name+" -- skipped!");
		 }


	 }

	 public void remove(Process p, List<Process> list2) {
		 int n_frames=p.getNumFrames();
		 int list_size=list2.size();
		 String name=p.getPid();
		 //System.out.println("time "+main2.clock+"ms: Process "+name+" arrived (requires "+n_frames+" frames)");
		 //int init=256-count_mem;
		 //int init2=init;

		// int pk=0;

			 System.out.println("time "+main2.clock+"ms: Process "+name+" removed:");
			 //System.out.println("The time is: "+main2.clock);
			 p.setOcc(0);
			 pageTable_ar=p.get_pageTable_ar();
			 pageTable_ar=new int[256][2];
			 for(int z=0;z<32;z++) {
				 System.out.print("=");
			 }
			 for(int l=0;l<256;l++) {//Allocated memory to a process
				 //System.out.println("VAlue of init "+init+":");
				 //System.out.println("Valueof init2 "+init2+":");
				 //System.out.println("Number of frames: "+n_frames+":");
				 if(memory[l]==name) {
				 memory[l]=".";
				 count_mem++;
				 }
				 //System.out.println("Came here1");
				 //count_mem++;
				/* pageTable_ar[l][0]=pk++;
				 pageTable_ar[l][1]=init2++;*/
				 //System.out.println("Came here2");
			 }
			 p.set_pageTable_ar(pageTable_ar);// Setting the page table for each process
			 System.out.print("\n");
			 //init2=init;
			 for(int g=0;g<256;g++) {//Displaying the memory
				 if(g%32==0&&g>0) {
					 System.out.println();
				 }
				 System.out.print(memory[g]);
			 }
			 System.out.print("\n");
			 for(int z=0;z<32;z++) {
				 System.out.print("=");
			 }
			 System.out.print("\n");
			// System.out.println("The value of count memory is:"+count_mem);
			 System.out.println("PAGE TABLE [page,frame]:");

			 //init2=0;
			 for(int p1=0;p1<list_size;p1++) {//Displaying the page table whenever a process is in memory

				 Process p2=list2.get(p1);
				 n_frames=p2.getNumFrames();
				pageTable_ar=p2.get_pageTable_ar();

				 if(p2.getOcc()==1) {
					 System.out.print(p2.getPid()+": ");
					 for(int i1=0;i1<n_frames;i1++) {
						 if(i1%10==0&&i1>0) {
							 System.out.println();
						 }
						// for(int i2=0;i2<2;i2++) {
						 System.out.print("["+pageTable_ar[i1][0]+","+pageTable_ar[i1][1]+"]" );
						 if(i1%10!=9) {
							 System.out.print(" ");
						 }
						//System.out.print("["+i1+","+(init2++)+"] ");
					 }

					 /*for(int u1=0;u1<(init-1);u1++) {
						 if(u1%10==0&&u1>0) {
							 System.out.println();
						 }
						System.out.print("["+pageTable[u1][0]+","+pageTable[u1][1]+"] ");
					 }*/
					 System.out.print("\n");
				 }

			 }
			 //System.out.print("\n");
			 this.set_memory(memory);



		 /*else {
			 System.out.println("Memory full");
		 }*/


	 }

}

//import java.io.*;
//import java.util.*;
 class Process
{
  private String Process_id;
  private String arr_time_ar[][];
  private int run_time_ar[];
  private int num_frames;
  private int num_arr;
  private int occupied;
  private int burst;
  private int[][] pageTable;



  public String getPid() {
       return Process_id;
   }

   public void setPid(String Process_id) {
       this.Process_id = Process_id;
   }

   public String[][] get_arr_time_ar() {
       return arr_time_ar;
   }

   public void set_arr_time(String[][] arr_time_ar) {
       this.arr_time_ar=arr_time_ar;
   }

   public int[] run_time_ar() {
       return run_time_ar;
   }

   public void setrun_time_ar(int[] run_time_ar) {
       this.run_time_ar = run_time_ar;
   }

   public int getNumFrames() {
       return num_frames;
   }

   public void setNumFrames(int num_frames) {
       this.num_frames = num_frames;
   }

   public int getNum_arr() {
       return num_arr;
   }

   public void setNum_arr(int num_arr) {
       this.num_arr = num_arr;
   }

   public int getOcc() {
       return occupied;
   }

   public void setOcc(int occ) {
       this.occupied = occ;
   }

   public int getburst() {
       return occupied;
   }

   public void setburst(int burst) {
       this.burst = burst;
   }

   public int[][] get_pageTable_ar() {
       return pageTable;
   }

   public void set_pageTable_ar(int[][] pageTable_ar) {
       this.pageTable=pageTable_ar;
   }


}
