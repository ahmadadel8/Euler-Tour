#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <stdbool.h>
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

LinkedList* L; //This is a global linked list that will contain vertices to visit later. It has to be global but I will try to fix this.
//////////////////////////////////
///MEMORY ALLOCATION FUNCTIONS///
////////////////////////////////

LinkedList* create_list () {
    // a function to allocate memory to the list and intialize it to avoid segmentation errors.
    LinkedList* list = (LinkedList*) malloc (sizeof(LinkedList));
    return list;
}

Ht_item* create_item() {
	// Creates a pointer to a new hash table item
	Ht_item* item = (Ht_item*) malloc (sizeof(Ht_item));
	item->list = NULL; //a null linkedlist
	return item;
	}

HashTable* create_table(int size) {
	// Creates a new HashTable with a fixed size
	HashTable* table = (HashTable*) malloc (sizeof(HashTable));
	table->size = size;
	table->count = 0;
	table->items = (Ht_item**) calloc (table->size, sizeof(Ht_item*));
	for (int i=0; i<table->size; i++)
	table->items[i] = NULL;

	return table;
	}

LinkedList* linkedlist_insert(LinkedList* list, int data) {
    // Inserts the data to the head of the linkedlist. O(1) complexity.
    LinkedList* head=create_list(); //creates a new head to the list
    head->data=data;    //add the data to it
    head->next=list;    //point to the old head of the list
    return head;
}
////////////////////////////////////
///LINKED LISTS HELPER FUNCTIONS///
//////////////////////////////////
LinkedList* linkedlist_remove_head(LinkedList* list) {
    // Removes the head from the linked list
    // O(1) complexity
    if (!list){ //if the list is already empty, return null
      return NULL;
    }
    LinkedList* new_head = create_list(); //create a new head
    LinkedList* temp = create_list();
    new_head=list->next; //skip the old head
    list->next = NULL; //disconnect the old head from the list
    return new_head;
}

void linkedlist_remove(LinkedList* head, int data) {
	//Removes the node of the linked list whose value==data
	//O(n) complexity. Used to remove a certain vertix from the adjacency list
  LinkedList* crawler=head, *prev; //ca crawler variable to crawl over the list until it finds a node whose value ==data, and a variable to keep track of the last list
  if (crawler) //if the list is empty, return
  {
      while (crawler != NULL && crawler->data != data) //keep crawiling until you find the data or the end of the list
      {
        prev = crawler;
        crawler = crawler->next;
      }
      if (crawler == NULL) return; //data isn't in the list, return
      prev->next = crawler->next;   //if the data is in the list, skip that node.

      free(crawler);  // Free memory
 }
}

//////////////////////////////////
///HASHTABLES HELPER FUNCTIONS///
////////////////////////////////
int hash_function(char *str) {
// Define a hash function that maps each row of the adjanceny list to the value of the vertix
    return atoi(strtok(str, " ")); //get the first number and then map it to an integer
}

void ht_insert(HashTable* hashtable, char* adj_list) {//takes inputs(hashtable, and a line from from the text file)
  //Inster an entry to the hashtable
	if (hashtable->count == hashtable->size) {
	    // Hash Table Full
	    printf("Insert Error: Hash Table is full\n");
	    return;
	}

	char * vertix; //a char to parse the adj list
	int index = hash_function(adj_list);//index of the hashfunction

	Ht_item* item = create_item();//create a new item
	while (true)
	{
		vertix = strtok(NULL, " \n\r");//continue the parsing that started in hash_function
		if (vertix==NULL) break;//if you reaches the end of the line, stop

		if (item->list==NULL)//if the item is fresh, and the list is empty, create a new list and add the data to it
		{
			item->list=create_list();
			item->list->data=atoi(vertix);
			item->list->next=NULL;
		}
		else
			item->list=linkedlist_insert(item->list, atoi(vertix));// else, append the existing list
	}

	// Insert the created item to the hashtable
	hashtable->items[index] = item;
	hashtable->count++;

	}



int pop_and_remove_edge(int u, HashTable* adj_table) {
	//this function pops the first adjacent vertix to u removes the edge (u,v) from the hashtable, i.e., removes v from u's adjlist and vice verse.
	Ht_item* u_item=adj_table->items[u];//u's adjacency list
	Ht_item* v_item;
  int v = u_item->list->data;//the first vertix adjacent to u
	u_item->list=linkedlist_remove_head(u_item->list); //remove said vertix

v_item=adj_table->items[v];//v's adjacencylist
//now we want to delete u's node in v's list.
  if (v_item->list->data==u) //if u is the first node in the list, remove the list's head
    v_item->list=linkedlist_remove_head(v_item->list);
  else
    linkedlist_remove(v_item->list, u);//else, search for it and remove it
	return v;//return v
}

HashTable* read_inputs(char* filename){
  //reads the inputs from the text file, and parses it to a hashtable
	FILE *ifp; //a file pointer
	// read input from file
	ifp = fopen(filename, "r");

	int num_v = 0; //an interger that holds the number of vetices
	char l[256];
	while (fgets(l, sizeof l, ifp) != NULL) num_v++;
	rewind(ifp);

	HashTable* adj_table = create_table(num_v+1);//creates a table to hold the adjacent matrix

	char line[1024];
	while (fgets(line, sizeof line, ifp) != NULL)
		ht_insert(adj_table, line); //insert and parse that line
	fclose(ifp); //close th file
	return adj_table;//return the table
	}
//////////////////////////////
////FILE HELPER FUNCTIONS////
////////////////////////////
  void write(LinkedList* head, FILE *ifp)
  //write a given euler tour to file ifp
  //it's a recursive function, which terminates until you find the end of the linkedlist
  {
      if(head == NULL)//termination condition
      {
        fprintf(ifp, "\n");
      }
      else
      {
          fprintf(ifp,"%d ", head -> data);//write the data inside the node to the file
          write(head->next, ifp);//recursive call
      }
  }

void write_outputs(LinkedList* T)
{//write the contents of an euler tour to A.txt file
  FILE *ifp;
  // read input from file
  ifp = fopen("output/A.txt", "w ");
  if (T) //if the euler tour exists
  {
    fprintf(ifp, "%d\n",1);//print 1 and call write
    write(T, ifp);
  }
  else
  fprintf(ifp, "%d\n",0);//else, print 0
}

LinkedList* splice(LinkedList* T,LinkedList*C, LinkedList* location_in_T)
//splices list C into list T at a given poisition
{
  LinkedList* split_end;
  if (!C) return T;
  split_end=location_in_T->next;//remainder of T after the split
	location_in_T->next=C;//point the list at T to C to link them together
  C->residual_link->next=split_end; //link the tail of C which is stored in the residiual link attribute to the remained to T
	return T;
}

int out_degree(int u, HashTable* adj_list)
//a function that counts the calculates the outdegree of a given tour
{	int count=0;
	LinkedList* list=adj_list->items[u]->list; //adj list of u
	while(list)//count the vertices in that list
	{
		count++;
		list=list->next;
	}
	return count;
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
    	while (out_degree(u, adj_list)>0)  //while the outdegree of node u >0
        {
      		w=pop_and_remove_edge(u, adj_list); //pop the first vertix w from the adj table
    		if (C==NULL) //intiializing the list
    		{
    			C=create_list();
    			C->data=u;
    			C->next=NULL;
    			C_tail=C;
    		 }
    		else C=linkedlist_insert(C, u);//adding subsequent nodes to the list

    		if (out_degree(u, adj_list)>0) //add any node that needs to be visited again to the global linkedlist L
    		{
    			L=linkedlist_insert(L,u);
    			L->residual_link=C;//add u's location in the list to the residual link
    		}
    		u=w;
    	}
    	if (C_tail) C->residual_link=C_tail; //if the cycle exists, add C's tail to residual_link
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

	for (int i=1; i<adj_list->size; i++)
		if (out_degree(i, adj_list)%2 !=0)//calculate the out_degree for every vertix and if one of them isn't even, returns null for no euler tour
			return NULL;

	while (L)	//L=NULL when empty
	{
		location_in_T=L->residual_link;
    u=L->data;//pop u from L
    L=linkedlist_remove_head(L);
	  C=visit(u, adj_list); //create a cycle starting from C

	    if (!location_in_T) //T is unitialized
	    {
	    	T=create_list();//initlazze T=C
	    	T=C;
	    	while(C->next)
	    		C=C->next;
	    	T=linkedlist_insert(T, C->data); //close the cylce
	    }
	    else
        T=splice(T,C,location_in_T);//splice C into T at location_in_T
	}
	return T;

}

int main(int argc, char *argv[])
{
  clock_t begin = clock();
    if (argc < 2)
	{
		printf("too few arguments !");
		return -1;
	}

	HashTable* adj_table=read_inputs (argv[1]);
	write_outputs(euler_tour(adj_table));
  clock_t end = clock();
  printf("Time taken to calculate the tour is= %lf\n", ((double)end-begin)/CLOCKS_PER_SEC);
  return 0;
}
