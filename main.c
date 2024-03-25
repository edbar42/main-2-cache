#include <math.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

// NOTE: 1 for cache, 0 for main memory
// cache is set to zero, memory is set
// to a random from 0 to 255
int **make_memory(int rows, int mem_type);
void print_ui();
void write_memory_to_file(int **memory_unit, FILE* file);

int LINESIZE;

struct Metadata {
	long main_memory_size;
	long cache_size;
	unsigned int block_size;
};

struct Metadata metadata;

int main(void) {
	print_ui();

	puts("How many bytes should each block/line have?");
	scanf("%ud", &metadata.block_size);
	LINESIZE = metadata.block_size;

	puts("How many bytes should the main memory have?");
	scanf("%ld", &metadata.main_memory_size);

	puts("How many bytes should the cache have?");
	scanf("%ld", &metadata.cache_size); 
	
	int mapping_method = 0;
	FILE *memory_file = fopen("mem.dat", "w");
	FILE *cache_file = fopen("cache.log", "w");


	// Creates memory as an array
	int **main_memory = make_memory(metadata.main_memory_size/LINESIZE, 0);
	int **cache_memory = make_memory(metadata.cache_size/LINESIZE, 1);

	write_memory_to_file(main_memory, memory_file);

	puts("Chose a mapping method:");
	puts("1 - Direct | 2 - Set Associative | 3 - Fully Associative");
	scanf("%d", &mapping_method);

	do {
	char inputted_addr[64]; // Plenty for this use case
	puts("Insert a binary address (use Ctrl + C to exit):");
	scanf("%s", inputted_addr);
	
	// Check for end of program
	if(strcmp(inputted_addr, "exit") == 0) {
		puts("Program terminated.");
		break;
	}
	int addr_as_index = strtol(inputted_addr,NULL, 2);
	} while (1);

	fclose(memory_file);
	fclose(cache_file);

	return 0;
}

int **make_memory(int rows, int mem_type){
	int **mem = (int**)malloc(rows * sizeof(int*));
	if (mem == NULL) {
		fprintf(stderr, "Memory allocation failed!\n");
		exit(1);
	}

	for (int i = 0; i < rows; i++) {
		 mem[i] = (int*)malloc(LINESIZE * sizeof(int));
		 if (mem[i] == NULL) {
			fprintf(stderr, "Memory allocation failed!\n");
			exit(1);
		 }
	}

	if(mem_type == 0) { // make main memory data
		for(int i = 0; i < rows; i++) {
			for(int j = 0; j < LINESIZE; j++) {
				mem[i][j] = rand() % 255;
			}
		}
	} else if (mem_type == 1) {// make cache set to 0
		for(int i = 0; i < rows; i++) {
			for(int j = 0; j < LINESIZE; j++) {
				mem[i][j] = 0;
			}
		}
	}
	return mem;
}

void write_memory_to_file(int **memory_unit, FILE* file) {
	int num_blocks = metadata.main_memory_size/LINESIZE;
	char spaces[12]; //+1 for \0 character
	
	memset(spaces, ' ', 10);
	spaces[11] = '\0'; // Indicates end of string

	fputs(spaces, file);
	// fprintf(file, "%d", block_addr_len);

	for (int k = 0; k < LINESIZE; k++) {
		if ( k == LINESIZE - 1) {
			fprintf(file, "%02x\n", k);
		} else {
		fprintf(file, "%02x ", k);
		}
	}

	for(int i = 0; i < num_blocks; i++) {
		if (i == 0) {
			fprintf(file, "0x%0*x: [ ", 4, 0);
		} else {
			fprintf(file, "0x%0*x: [ ", 4, i * LINESIZE);
		}

		for (int j = 0; j < LINESIZE; j++) {
			fprintf(file, "%02x ", memory_unit[i][j]);
		}
		fputs("]\n", file);
	}

	fflush(file);

}
 
void print_ui() {
	puts("\t\t\t\t-----------------------------------");
	puts("\t\t\t\t| Welcome to the memory simulator |");
	puts("\t\t\t\t-----------------------------------");
	puts("\tThis programs mimics the exchange of information between CPU cache and main memory.");
	puts("");

	
	puts("\t\t\t\t\t----------------");
	puts("\t\t\t\t\t| Instructions |");
	puts("\t\t\t\t\t----------------");
	puts("\t-> You will firstly choose how long each line/block of memory should be.");
	puts("\t-> Then, you will choose the size of the main memory followed by the size of the cache.");
	puts("\t-> Lastly, you will choose the mapping method.");
	puts("");

	
	puts("\t\t\t\t\t-------------");
	puts("\t\t\t\t\t| Attention |");
	puts("\t\t\t\t\t-------------");

	puts("\t-> All memory information inputted is evaluated in bytes.");
	puts("\t-> In other words, inputting 16 for line/block size, implies they will be 16 bytes long.");

	puts(" ");
	puts("\t Let\'s begin!");
	puts("\t\t\t\t----------------------------------------------------------");
}
