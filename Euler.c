
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <stdbool.h>

typedef struct LinkedList LinkedList;

// Define the Linkedlist here
struct LinkedList {
    int data;
    LinkedList* next;
    LinkedList* residual_link;
};



LinkedList* allocate_list () {
    // Allocates memory for a Linkedlist pointer
    LinkedList* list = (LinkedList*) malloc (sizeof(LinkedList));
    return list;
}

LinkedList* L; //has to be global.


void display(LinkedList* head)
{
    if(head == NULL)
    {
      printf("\n");
    }
    else
    {
        printf("%d ", head -> data);
        display(head->next);
    }
}

void write(LinkedList* head, FILE *ifp)
{
    if(head)
    {
        fprintf(ifp,"%d ", head -> data);
        write(head->next, ifp);
    }
}
LinkedList* linkedlist_insert(LinkedList* list, int data) {
    // Inserts the data to the head of the linkedlist. O(1) complexity.
    LinkedList* head=allocate_list();
    head->data=data;
    head->next=list;
    return head;
}


LinkedList* linkedlist_remove_head(LinkedList* list) {
    // Removes the head from the linked list
    // and returns the item of the popped element
    // O(1) complexity
    if (!list){
      return NULL;
    }
    LinkedList* new_head = allocate_list();
    LinkedList* temp = allocate_list();
    temp=list;
    new_head=list->next;
    temp->next = NULL;

    return new_head;
}

void linkedlist_remove(LinkedList* head, int data) {
	//Removes the node of the linked list whose value==data
	//and returns the linkedlist
	//O(n) complexity
  LinkedList* temp=head, *prev;
  if (!temp) return;
  else
  {
      while (temp != NULL && temp->data != data)
      {
        prev = temp;
        temp = temp->next;
      }
      if (temp == NULL) return;
      prev->next = temp->next;

      free(temp);  // Free memory
 }
}



int hash_function(char *str) {
// Define a hash function that maps each row of the adjanceny list to the value of the vertix
    return atoi(str);
}

typedef struct Ht_item Ht_item;

// Define the Hash Table Item here
struct Ht_item {
    char* key;
    LinkedList* list;
};

typedef struct HashTable HashTable;

// Define the Hash Table here
struct HashTable {
	// Contains an array of pointers
	// to items
	Ht_item** items;
	int size;
	int count;
	};

Ht_item* create_item(char* key) {
	// Creates a pointer to a new hash table item
	Ht_item* item = (Ht_item*) malloc (sizeof(Ht_item));
	item->key = (char*) malloc (strlen(key) + 1);
	item->list = NULL;
	strcpy(item->key, key);
	return item;
	}

HashTable* create_table(int size) {
	// Creates a new HashTable
	HashTable* table = (HashTable*) malloc (sizeof(HashTable));
	table->size = size;
	table->count = 0;
	table->items = (Ht_item**) calloc (table->size, sizeof(Ht_item*));
	for (int i=0; i<table->size; i++)
	table->items[i] = NULL;

	return table;
	}





void ht_insert(HashTable* table, char* adj_list) {

	if (table->count == table->size) {
	    // Hash Table Full
	    printf("Insert Error: Hash Table is full\n");
	    return;
	}

	char * vertix;
	char * key;
	key = strtok(adj_list, " ");
	int index = hash_function(key);

	Ht_item* item = create_item(key);
	while (true)
	{
		vertix = strtok(NULL, " \n\r");
				if (vertix==NULL) break;

		printf("Edge exists between (%s,%s)\n", key, vertix);
		if (item->list==NULL)
		{
			item->list=allocate_list();
			item->list->data=atoi(vertix);
			item->list->next=NULL;
		}
		else

			item->list=linkedlist_insert(item->list, atoi(vertix));
	}

	// Insert directly
	table->items[index] = item;
	table->count++;

	}



int pop_and_remove_edge(int u, HashTable* table) {
	//this function pops the first adjacent vertix to u removes the edge (u,v) from the hashtable, i.e., removes v from u's adjlist and vice verse.
	Ht_item* u_item=table->items[u];
	Ht_item* v_item;
  int v = u_item->list->data;
	u_item->list=linkedlist_remove_head(u_item->list);;
	v_item=table->items[v];

  if (v_item->list->data==u)
    v_item->list=linkedlist_remove_head(v_item->list);
  else
    linkedlist_remove(v_item->list, u);
	return v;
}

int random_vertix(int max)
{
	int vertix;
  srand(time(NULL));
	vertix=rand() % (max) +1;
	return vertix;
}



void print_table(HashTable* table) {
	printf("\nHash Table\n-------------------\n");
	LinkedList* list;
	for (int i=0; i<table->size; i++) {

	if (table->items[i]) {
	    printf("Index:%d, Key:%s, Values:", i, table->items[i]->key);
      display(table->items[i]->list);

	}
	}
	printf("\n-------------------\n\n");
}


HashTable* read_inputs(char* filename){
	FILE *ifp;


	// read input from file
	ifp = fopen(filename, "r");

	int num_v = 0;
	char l[256];
	while (fgets(l, sizeof l, ifp) != NULL) num_v++;
	rewind(ifp);


	printf("Total number of vertices = %d\n", num_v);
	HashTable* ht = create_table(num_v+1);

	char line[1024];
	while (fgets(line, sizeof line, ifp) != NULL)
		ht_insert(ht, line);

	print_table(ht);
	fclose(ifp);
	return ht;
	}

void write_outputs(LinkedList* T)
{
  FILE *ifp;


  // read input from file
  ifp = fopen("output/A.txt", "w ");
  if (T)
  {
    fprintf(ifp, "%d\n",1);
    write(T, ifp);
  }
  else
  fprintf(ifp, "%d\n",0);
}
LinkedList* splice(LinkedList* T,LinkedList*C, LinkedList* end_of_C)
{	LinkedList* split_start=T;
  LinkedList* split_end;
  if (!C) return T;
	while (split_start!=end_of_C)
		split_start=split_start->next;

  split_end=split_start->next;
	split_start->next=C;
  C->residual_link->next=split_end;
	return T;
}

int out_degree(int u, HashTable* adj_list)
{	int count=0;
	LinkedList* list=adj_list->items[u]->list;

	while(list)
	{
		count++;
		list=list->next;
	}
	return count;
}


LinkedList* visit(int u, HashTable* adj_list)
{
    	LinkedList* C = NULL;
    	LinkedList* head_of_c=NULL;
    	int w;
    	while (out_degree(u, adj_list)>0)
        {
      		w=pop_and_remove_edge(u, adj_list);
    		if (C==NULL)
    		{
    			C=allocate_list();
    			C->data=u;
    			C->next=NULL;
    			head_of_c=C;
    		 }
    		else C=linkedlist_insert(C, u);

    		if (out_degree(u, adj_list)>0)
    		{
    			L=linkedlist_insert(L,u);
    			L->residual_link=C;
    		}
    		u=w;
    	}
    	if (head_of_c) C->residual_link=head_of_c; /////need to fix this
      return C;
    }


LinkedList* euler_tour(HashTable* adj_list)
{

  LinkedList* T = NULL;
  LinkedList* C = NULL;
  L=allocate_list();
	LinkedList* location_of_split;
	int u;

	L->data= 1;//random_vertix(adj_list->size);
	L->residual_link=NULL;
	L->next=NULL;
  int j=0;

	for (int i=1; i<adj_list->size; i++)
		if (out_degree(i, adj_list)%2 !=0)
			return NULL;

	while (L)	//L=NULL when empty
	{
		location_of_split=L->residual_link;
    u=L->data;
    L=linkedlist_remove_head(L);
	  C=visit(u, adj_list);

	    if (!T) //T is unitialized
	    {
	    	T=allocate_list();
	    	T=C;
	    	while(C->next)
	    		C=C->next;

	    	T=linkedlist_insert(T, C->data); //close the cylce

	    }
	    else
        T=splice(T,C,location_of_split);

	}
	return T;

}

int main(int argc, char *argv[])
{
    if (argc < 2)
	{
		printf("too few arguments !");
		return -1;
	}

	HashTable* ht=read_inputs (argv[1]);
  clock_t begin = clock();
	write_outputs(euler_tour(ht));
  clock_t end = clock();
  printf("Time taken to calculate the tour is= %lf\n", ((double)end-begin)/CLOCKS_PER_SEC);
  return 0;
}
