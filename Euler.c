#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <stdbool.h>
#include <math.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
/*The structure of the graph will be a hashtable that hashes to linkedlists. Each vertix hashes into its integer value so their will be no collision
Each entry in the hashtable has a linkedlist with all the vertices into the corrisponding vertix's adjanceny adj_list
Below we define stuctures for linkedlists and hashtables.*/

typedef struct LinkedList LinkedList;

// Define the Linkedlist here. We use an augmented linkedlist with an additonal pointer that we use in different parts of the code.
struct LinkedList {
    int data; //the data stored in that node
    LinkedList* next; //a pointer to the next node in the list.
    LinkedList* residual_link; /*an additional pointer that can be used to point to additional locations like the tail of the list or where said vertix is
                                another list*/
};

///////////////////////////////////
////DATA STUCTURES DEFINITIONS////
/////////////////////////////////
typedef struct Ht_item Ht_item;//an item in the hashtable

// Define the Hash Table Item here
struct Ht_item {
    LinkedList* list; //a linked list with the adjacency list.
};

typedef struct HashTable HashTable; //the entire hashtable

// Define the Hash Table here
struct HashTable {
	Ht_item** items; //an array of pointer to the items
	int size;        //the size of the hashtable. used to allocate memory to the table.
	int count;       //number of items already entered in the table
	};

  //////////////////////////////////
 ///GLOBAL VARIABLES DECLARATIONS///
///////////////////////////////////

LinkedList* L; //This is a global linked list that will contain vertices to visit later. It has to be global but I will try to fix this.
HashTable* edge_count;
HashTable* vertix_count;
int commands_count;
  /////////////////////////////////
 ///MEMORY ALLOCATION FUNCTIONS///
/////////////////////////////////

LinkedList* create_list () {
    // a function to allocate memory to the list and intialize it to avoid segmentation errors.
    LinkedList* list = (LinkedList*) malloc (sizeof(LinkedList));
    commands_count++;
    return list;
}

Ht_item* create_item() {
	// Creates a pointer to a new hash table item
	Ht_item* item = (Ht_item*) malloc (sizeof(Ht_item));
	item->list = NULL; //a null linkedlist
  commands_count+=2;
	return item;
	}

HashTable* create_table(int size) {
	// Creates a new HashTable with a fixed size
	HashTable* table = (HashTable*) malloc (sizeof(HashTable));
	table->size = size;
	table->count = 0;
	table->items = (Ht_item**) calloc (table->size, sizeof(Ht_item*));
  commands_count+=4;
	for (int i=0; i<table->size; i++)
      {
         table->items[i] = NULL;
         commands_count++;
       }

	return table;
	}

                                                   ///////////////////////////////////
                                                  ///COMPLEXITY ANALYSIS FUNCTIONS///
      /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
     //The way we will count the number of operations assigned to each vertix and edge is through creating a hash table for edges and   //
    //a hash table for vertices. The one for the vertices has a direct mapping hashfunction where hashfunc(v)=v so we will not create  //
   //create a hashfunction for it. The one for the verices should have a one to one symmetric mapping function where                  //
  //hashfunc((u,v))==hashfunc((v,u)) for every distinct edge (v,u)                                                                   //
 /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


 int hash_function_edges(int u, int v)
 /*we will use a simple version of Szudzik's function. To create a symmetric mapping for (u,v) and (v,u) we will just sort u,v and use one
 case of Szudzik's function.
 Szudzik's paring
 Szudzik(a,b)=a >= b ? a * a + a + b : a + b * b;  where a, b >= 0
 Instead, we impliment
 Szudzik(a,b)=min(a,b) + max(a,b) * max(a,b).
 We use this instead of cantor because it yealds smaller maximum number, so the size of the hashtable will be reasonable.
 */
 {
 int max= u>=v? u:v;
 int min= u<v ? u:v;
 return min + (int)pow(max,2);
 }

 //we will use the same hashtable struct we defined above and use the linkedlists with just one node.
 //These hashtables should be global variables.
 HashTable* create_count_table(int size){
     HashTable* count= create_table(size);
     for (int i=0; i<=size; i++)
     {
     count->items[i]=create_item();
     count->items[i]->list=create_list();
     count->items[i]->list->data=0;
     }
     return count;
 }



 int edge_counter(int u, int v, int increment){
   edge_count->items[hash_function_edges(u,v)]->list->data+=increment;
   }
 int vertix_counter(int u, int increment){
         vertix_count->items[u]->list->data+=increment;
         }
//the functions that are always assigned either to edges or vertices are counted in their declarations. Other functions that are either that or that are counter accordingly
//when they are called. Functions assigned to vertices, edges or both will have a comment in their declaration  line (//V, //E, or //V OR E)
  //////////////////////////////////
 ///LINKEDLISTS HELPER FUNCTIONS///
//////////////////////////////////

LinkedList* linkedlist_insert(LinkedList* list, int data) {//V OR E: 3 C commands
    // Inserts the data to the head of the linkedlist. O(1) complexity.
    LinkedList* head=create_list(); //creates a new head to the list
    head->data=data;    //add the data to it
    head->next=list;    //point to the old head of the list
    commands_count+=3;
    return head;
}
LinkedList* linkedlist_remove_head(LinkedList* list) { //V OR E //5 C commands
    // Removes the head from the linked list
    // O(1) complexity
    if (!list){ //if the list is already empty, return null
      commands_count++;
      return NULL;
    }
    LinkedList* new_head = create_list(); //create a new head
    LinkedList* temp = create_list();
    new_head=list->next; //skip the old head
    list->next = NULL; //disconnect the old head from the list
    commands_count+=2;
    return new_head;
}

void linkedlist_remove(LinkedList* head, int v) {//E
	//Removes the node of the linked list whose value==v
	//O(n) complexity. Used to remove a certain vertix from the adjacency list
  LinkedList* crawler=head, *prev; //create a crawler variable to crawl over the list until it finds a node whose value ==data, and a variable to keep track of the last list
  edge_counter(head->data,v, 1);
  commands_count+=1;
  if (crawler) //if the list is empty, return
  {   commands_count++;
      while (crawler != NULL && crawler->data != v) //keep crawiling until you find the data or the end of the list
      {
        prev = crawler;
        crawler = crawler->next;
        edge_counter(head->data,v, 3); //two lines above plus checking the loop condition
        commands_count+=3;
      }
      if (crawler == NULL)
        { commands_count++;
          edge_counter(head->data,v, 1);
          return; //data isn't in the list, return
        }
      prev->next = crawler->next;   //if the data is in the list, skip that node.
      free(crawler);  // Free memory
      edge_counter(head->data,v, 2);
      commands_count+=2;
 }
}


  /////////////////////////////////
 ///HASHTABLES HELPER FUNCTIONS///
/////////////////////////////////
int hash_function(char *str) {//V
// Define a hash function that maps each row of the adjanceny list to the value of the vertix
    int u=atoi(strtok(str, " ")); //get the first number and then map it to an integer
    vertix_counter(u,1);
    commands_count+=1;
    return u;
}

void ht_insert(HashTable* hashtable, char* adj_list) {
  //takes inputs(hashtable, and a line from from the text file)
  //Inster an entry to the hashtable
	if (hashtable->count == hashtable->size) {
	    // Hash Table Full
      commands_count++;
	    printf("Insert Error: Hash Table is full\n");
	    return;
	}

	char * vertix_str; //a char to parse the adj list
  int v;//int value of vertix_str.
	int u = hash_function(adj_list);//index of the hashfunction. Counted.
	Ht_item* item = create_item();//create a new item
  vertix_counter(u,1); //193
  commands_count+=5;

	while (true)
	{
		vertix_str = strtok(NULL, " \n\r");//continue the parsing that started in hash_function
    if (vertix_str==NULL) break;//if you reach the end of the line, stop
    v=atoi(vertix_str);
    edge_counter(u, v, 4); //three lines above plus checking the loop condition
    commands_count+=4;


		if (item->list==NULL)//if the item is fresh, and the list is empty, create a new list and add the data to it
		{
			item->list=create_list();
			item->list->data=v;
			item->list->next=NULL;
      commands_count+=4;
      vertix_counter(u,4); //Creating the list happens once per vertix.
      edge_counter(u, v, 1); //Adding v to u's adjanceny list.
		}
		else
			{
        item->list=linkedlist_insert(item->list, v);// else, append the existing list
        edge_counter(u, v, 3); //linkedlist_insert has 3 commands
        commands_count+=1;
      }
	}

	// Insert the created item to the hashtable
	hashtable->items[u] = item;
	hashtable->count++;
  commands_count+=2;
  vertix_counter(u,2); //adding the item to the hashtable happens once per vertix
	}



int pop_and_remove_edge(int u, HashTable* adj_table) {
	//this function pops the first adjacent vertix to u removes the edge (u,v) from the hashtable, i.e., removes v from u's adjlist and vice verse.
	Ht_item* u_item=adj_table->items[u];//u's adjacency list
	Ht_item* v_item;
  int v = u_item->list->data;//the first vertix adjacent to u
	u_item->list=linkedlist_remove_head(u_item->list); //remove said vertix
  v_item=adj_table->items[v];//v's adjacencylist

  edge_counter(u,v,9); //linkedlist_remove has 5 C commands
  commands_count+=9;

//now we want to delete u's node in v's list.
  if (v_item->list->data==u) //if u is the first node in the list, remove the list's head
  {
     v_item->list=linkedlist_remove_head(v_item->list);
     commands_count+=2;
     edge_counter(u,v,6); //linkedlist_remove has 5 C commands+ the if condition
   }

  else{
    linkedlist_remove(v_item->list, u);//else, search for it and remove it
    edge_counter(u,v,5); //linkedlist_remove has 5 C commands
    commands_count+=2;
      }
	return v;
}

int out_degree(int u, HashTable* adj_list)//E
//a function that counts the calculates the outdegree of a given tour
{
	LinkedList* list=adj_list->items[u]->list; //adj list of u
  int count=0;
  commands_count+=2;
  vertix_counter(u,2);
  while(list)//count the vertices in that list
  {
    edge_counter(u, list->data, 4); //3 lines below + checking the loop condition
    commands_count+=4;
		count++;
		list=list->next;
	}
	return count;
}

///////////////////////////
///FILE HELPER FUNCTIONS///
///////////////////////////


HashTable* read_inputs(char* filename){
  //reads the inputs from the text file, and parses it to a hashtable
	FILE *ifp; //a file pointer
	// read input from file
	ifp = fopen(filename, "r");

	int V = 0; //an interger that holds the number of vetices
	char l[256];

  commands_count+=4;

	while (fgets(l, sizeof l, ifp) != NULL)
  {
    V++;
    commands_count+=2;
  }

  edge_count= create_count_table(hash_function_edges(V,V-1)+1);
  vertix_count= create_count_table(V+1);

	rewind(ifp);
	HashTable* adj_table = create_table(V);//creates a table to hold the adjacency matrix
	char line[1024];

  commands_count+=3;

	while (fgets(line, sizeof line, ifp) != NULL)
  {
		ht_insert(adj_table, line); //insert and parse that line. Counted.
    commands_count+=2;
  }
	fclose(ifp); //close the file

  commands_count++;
	return adj_table;//return the table
	}

void write_list(LinkedList* head, FILE *ifp) //V
  //write a given euler tour to file ifp
  //it's a recursive function, which terminates until you find the end of the linkedlist
  {
      if(head == NULL)//termination condition
      {
        commands_count+=2;
        fprintf(ifp, "\n");
      }
      else
      {
          fprintf(ifp,"%d ", head -> data);//write the data inside the node to the file
          vertix_counter(head -> data,1);
          write_list(head->next, ifp);//recursive call //counted
          commands_count+=3;
      }
  }
  void write_outputs(LinkedList* T, HashTable *adj_table,int V ,char* input_name)
    {
      FILE *ifp;
      char input_number[2];
      char out_1[100], out_2[100];
      int v;
      input_number[0]=input_name[8];
      input_number[1]='\0';

      strcpy(out_1, "A");
      strcat(out_1, input_number);
      strcat(out_1, ".txt");

      ifp = fopen(out_1, "w ");
      if (T) //if the euler tour exists
      {

        fprintf(ifp, "%d\n",1);//print 1 and call write
        write_list(T, ifp);
      }
      else
      fprintf(ifp, "%d\n",0);//else, print 0

      if (input_name[8]-'0'==1)
        {
          strcpy(out_2, "B");
          strcat(out_2, input_number);
          strcat(out_2, ".txt");
          ifp = fopen(out_2, "w ");
          for (int i=1; i<=V; i++)
              fprintf(ifp, "Vertix %d: %d \n", i, vertix_count->items[i]->list->data);

          for (int u=1; u<=V; u++)
              {
                while(out_degree(u,adj_table))
                  {
                    v=pop_and_remove_edge(u, adj_table);
                    fprintf(ifp, "Edge (%d,%d): %d \n", u,v, edge_count->items[hash_function_edges(u,v)]->list->data);
                  }
              }
          fprintf(ifp, "Total Number of operations: %d", commands_count);
        }
      else
        {
          strcpy(out_2, "C");
          strcat(out_2, input_number);
          strcat(out_2, ".txt");
          ifp = fopen(out_2, "w ");
          int max=0;
          for (int i=1; i<=V; i++)
              if (vertix_count->items[i]->list->data>max)
                  max=vertix_count->items[i]->list->data;
          fprintf(ifp, "Maximum number of operations charged to any single vertex is: %d \n", max);
          max=0;
          for (int u=1; u<=V; u++)
            for(int v=u+1; v<=V; v++)
                if (edge_count->items[hash_function_edges(u,v)]->list->data>max)
                    max=edge_count->items[hash_function_edges(u,v)]->list->data;
          fprintf(ifp, "Maximum number of operations charged to any single edge is: %d \n", max);
          fprintf(ifp, "Total Number of operations: %d", commands_count);
        }
    }

LinkedList* splice(LinkedList* T,LinkedList*C, LinkedList* location_in_T)//Counted with the vertix  at which we split.
//splices list C into list T at a given poisition.
{
  LinkedList* split_end;
  if (!C) return T;
  split_end=location_in_T->next;//remainder of T after the split
	location_in_T->next=C;//point the list at T to C to link them together
  C->residual_link->next=split_end; //link the tail of C which is stored in the residiual link attribute to the remained to T
  vertix_counter(location_in_T->data, 5);
  commands_count+=6;
	return T;
}

                       /////////////////////////
                      ///EULER TOUR CREATION///
///////////////////////////////////////////////////////////////////////////
//this procedure is from intro to algorithms's instructor's manual P334///
///////////////////////////////////////////////////////////////////////////


LinkedList* visit(int u, HashTable* adj_list)
//visits a simple cylce starting with node u
{
    	LinkedList* C = NULL; //creates a linked list to store the cycle in
    	LinkedList* C_tail=NULL;//a pointer to the tail of that list
    	int w;
      int i=0;

      commands_count+=3;
    	while (out_degree(u, adj_list)>0)  //while the outdegree of node u >0. counted inside
        {
      		w=pop_and_remove_edge(u, adj_list); //pop the first vertix w from the adj table. counted inside.
          commands_count++;
    		if (C==NULL) //intiializing the list
    		{
    			C=create_list();
    			C->data=u;
    			C->next=NULL;
    			C_tail=C;
          vertix_counter(u,5);
          commands_count+=5;

    		 }
    		else {
                C=linkedlist_insert(C, u);//adding subsequent nodes to the list. 3 Commands.
                vertix_counter(u,3);
                commands_count++;
            }
    		if (out_degree(u, adj_list)>0) //add any node that needs to be visited again to the global linkedlist L
    		{
    			L=linkedlist_insert(L,u);// 3 Commands.
    			L->residual_link=C;//add u's location in the list to the residual link
          vertix_counter(u,5);
          commands_count+=2;
    		}
    		u=w;
        commands_count++;

    	}
    	if (C_tail) C->residual_link=C_tail; //if the cycle exists, add C's tail to residual_link
      commands_count+=2;


      return C;
    }


LinkedList* euler_tour(HashTable* adj_list)
//Also taken from the instuctors manual. Calculates the euler tour if it exists.
{
  LinkedList* T = NULL; //the euler tour
  LinkedList* C = NULL; //the simple cylce returned by visit
  L=create_list(); //initialzes L
	LinkedList* location_in_T;
	int u;

	L->data= 1;//intial vertix in the cycle
	L->residual_link=NULL;
	L->next=NULL;
  vertix_counter(1,3);
  commands_count+=8;

	for (int i=1; i<adj_list->size; i++)
  {
		if (out_degree(i, adj_list)%2 !=0)//calculate the out_degree for every vertix and if one of them isn't even, returns null for no euler tour //counted
			return NULL;
      commands_count+=2;
  }
	while (L)	//L=NULL when empty
	{
		location_in_T=L->residual_link;
    u=L->data;//pop u from L
    L=linkedlist_remove_head(L); //5 commands
    vertix_counter(u, 6);
	  C=visit(u, adj_list); //create a cycle starting from C. Counted.
    commands_count+=5;

	    if (!location_in_T) //T is unitialized
	    {
	    	T=create_list();//initialize T=C
	    	T=C;
        commands_count+=3;

	    	while(C->next){
	    		C=C->next;
          commands_count+=2;
        }
	    	T=linkedlist_insert(T, C->data); //close the cylce
        commands_count++;
	    }
	    else
        {
          T=splice(T,C,location_in_T);//splice C into T at location_in_T. Counted.
          commands_count++;
        }
	}
	return T;

}

int main(int argc, char *argv[])
{
  clock_t begin = clock();
  commands_count=20;//initial count. Number of operations used to define data stucturs and global variables not counting those for counting operations.

    if (argc < 2)
	{
		printf("too few arguments !");
    commands_count++;
		return -1;
	}
	HashTable* adj_table=read_inputs (argv[1]);
  int V= adj_table->size;
  LinkedList *T=euler_tour(adj_table);
	write_outputs(T,adj_table, V, argv[1]);
  clock_t end = clock();
  printf("Time taken to calculate the tour is= %lf\n", ((double)end-begin)/CLOCKS_PER_SEC);
  return 0;
}
