#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

int hash_function(char *str) {
// Define a hash function that maps each row of the adjanceny list to the value of the vertix
    return atoi(str);
}

typedef struct Ht_item Ht_item;

// Define the Hash Table Item here
struct Ht_item {
    char* key;
    char* value;
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

Ht_item* create_item(char* key, char* value) {
	// Creates a pointer to a new hash table item
	Ht_item* item = (Ht_item*) malloc (sizeof(Ht_item));
	item->key = (char*) malloc (strlen(key) + 1);
	item->value = (char*) malloc (strlen(value) + 1);

	strcpy(item->key, key);
	strcpy(item->value, value);

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


//DO I NEED TO DELETE THESE??????????????????????????????????????????????
void free_item(Ht_item* item) {
	// Frees an item
	free(item->key);
	free(item->value);
	free(item);
	}

void free_table(HashTable* table) {
	// Frees the table
	for (int i=0; i<table->size; i++) {
	Ht_item* item = table->items[i];
	if (item != NULL)
	    free_item(item);
	}

	free(table->items);
	free(table);
	}


void ht_insert(HashTable* table, char* key, char* value) {
	// Create the item
	Ht_item* item = create_item(key, value);

	// Compute the index
	int index = hash_function(key);

	Ht_item* current_item = table->items[index];

	if (current_item == NULL) {
	// Key does not exist.
	if (table->count == table->size) {
	    // Hash Table Full
	    printf("Insert Error: Hash Table is full\n");
	    // Remove the create item
	    free_item(item);
	    return;
	}

	// Insert directly
	table->items[index] = item;
	table->count++;
	}

	else {
	    // Scenario 1: We only need to update value
	    if (strcmp(current_item->key, key) == 0) {
		strcat(table->items[index]->value, value);

		return;
	    }

	}
	}


char *remove_vertix(char *adj_list, char *vertix) {
	size_t len = strlen(adj_list);
	if (len > 0) 
	{	if (strcmp(adj_list, vertix))	
		{
			char *p = adj_list;
			if ((p = strstr(p, vertix)) != NULL) 
			{
		    		memmove(p, p + len+1, strlen(p + len) + 1);
			}
		}
		else
			adj_list[0]='\0';
	}
	return adj_list;
}


char *random_vertix(int min, int max)
{
	char *vertix=malloc(2);;
	int vertix_int=(rand() %  (max - min + 1)) + min; 
	snprintf (vertix, sizeof(vertix), "%d",vertix_int);
	strcat(vertix, " ");
	return vertix;
}


char *ht_pop(HashTable* table, char *key) {
	// Searches the key in the hashtable
	// and returns NULL if it doesn't exist
	//returns the next vertex in the adjaceny list and deletes it. 
	char *next_vertix;
	int index = hash_function(key);
	int next_idx;
	Ht_item* item = table->items[index];

	// Ensure that we move to a non NULL item
	if (item != NULL) 
	{

	    char *copy = malloc(strlen(table->items[index]->value) + 1);
	    next_vertix=strtok(copy, " ");
	    	    printf("here");
    	return " ";
	    table->items[index]->value=remove_vertix(table->items[index]->value, next_vertix);

	    next_idx=hash_function(next_vertix);
	    table->items[next_idx]->value=remove_vertix(table->items[next_idx]->value, key);
	    return next_vertix;
	}

	return NULL;
}


void print_table(HashTable* table) {
	printf("\nHash Table\n-------------------\n");
	for (int i=0; i<table->size; i++) {

	if (table->items[i]) {
	    printf("Index:%d, Key:%s, Value:%s\n", i, table->items[i]->key, table->items[i]->value);
	}
	}
	printf("-------------------\n\n");
}


HashTable* read_inputs(char* filename){
	FILE *ifp;

	clock_t begin = clock();

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
	{
		char * src_vertex, * dst_vertex;
		char * pch;
		pch = strtok(line, " ");
		src_vertex=pch;


		while (pch != NULL)
		{


			pch = strtok(NULL, "\n\r");
			dst_vertex=pch;

			printf("Edge exists between (%s,%s)\n", src_vertex, dst_vertex);
			ht_insert(ht, src_vertex, dst_vertex);
			pch = strtok(NULL, " \n\r");

		}


	}
	clock_t end = clock();
	printf("Time taken = %lf\n", ((double)end-begin)/CLOCKS_PER_SEC);

	fclose(ifp);
	return ht;
	}


char *splice(char *T,char *C, char *u)
{
	char *tmp1=malloc(strlen(T) + 1);
	char *tmp2=malloc(strlen(T) + 1);
	int location_in_T=((char *)strstr(T, u)) - ((char *)T);
	strncpy (tmp1, T, location_in_T);
	strncpy (tmp2, &T[location_in_T], strlen (T) - location_in_T);
	strcat(tmp1, C);
	strcat(tmp1, " ");
	strcat(tmp1, tmp2);
	
	return tmp1;
}

char *visit(char *u, char *L, HashTable* adj_list)
{
    	char *C = malloc(adj_list->size*2*sizeof(char));
    	char *w;
    	C="";
	while (strlen(adj_list->items[hash_function(u)]->value))
	{

		w=ht_pop(adj_list, u);
			    	    	printf("here");
    	return " ";
		strcat(C,u);
		if (strlen(adj_list->items[hash_function(u)]->value))
		{
			strcat(L,u);
		}
	strcpy(w,u);
	}

	
	
	return C;
			 
}

int count_vertices(char *value)
{
	//each adjacency list takes the form "v1 v2 v3" where the number of delimeters " " is less than the number of vertices by 1.
	//to get the number of vertices, we simply add 1 then devide by 2
	return (strlen(value)+1)/2;
}

char *euler_tour(HashTable* adj_list)
{

	char *T = malloc(adj_list->size*2*sizeof(char));
	char *C = malloc(adj_list->size*2*sizeof(char));
	char *L = 1;//andom_vertix(1, adj_list->size);
	char *temp = malloc(strlen(L) + 1);
	char *co, *u;
	for (int i=0; i<adj_list->size; i++) 
	{
		if (adj_list->items[i]) 
		{
			if (count_vertices(adj_list->items[i]->value) %2 !=0) 
			{
				printf("No Euler Tour \n");
				return "";
			}	
		}
	
	
	}
	
	while (strlen(L)){
		strcpy(temp, L);
	    u=strtok(temp, " ");
	    strcat(u, " ");
	    L=remove_vertix(L, u);
	    C=visit(u,L, adj_list);
	    printf("HERE");
	    return " ";
	    if (strlen(T))
	    {

	    	splice(T, C, u);
	    }
	    else
	    {
	    	strcpy(temp, C);
	    	co=strtok(temp, " ");
	    	strcat(C, " ");
	    	strcat(C, co);
	    	strcpy(T, C);
	    }

	}
	return T;

}

int main(int argc, char *argv[])
{   HashTable* ht=read_inputs (argv[1]);
    if (argc < 2)
	{
		printf("too few arguments !");
		return -1;
	}

    print_table(ht);
    
    char *P =euler_tour(ht);
    return 0;
}