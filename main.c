#include <time.h>
#include <math.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

// NOTE: 1 for cache, 0 for main memory
// cache is set to zero, memory is set
// to a random from 0 to 255
void print_ui();
int compare_data(int *data, int *line);
int hit_or_miss(int **cache, int* block);
int **make_memory(int rows, int mem_type);
void log_cache_state(int **cache, FILE* cache_file);
void map_to_cache(int **cache, int *block, int mapping_method, int block_index);
void write_memory_to_file(int **memory_unit, FILE* file, long memory_size);


int LINESIZE;

struct Metadata {
	long main_memory_size;
	long cache_size;
	unsigned int block_size;
};

struct Metadata metadata;

int main(void) {
	// NOTE: Used to ensure actual random numbers
	srand(time(NULL));

	print_ui();

	puts("How many bytes should each block/line have?");
	scanf("%ud", &metadata.block_size);
	LINESIZE = metadata.block_size;

	puts("How many bytes should the main memory have?");
	scanf("%ld", &metadata.main_memory_size);

	puts("How many bytes should the cache have?");
	scanf("%ld", &metadata.cache_size); 
	
	int cache_hit_or_miss;
	int mapping_method;
	FILE *memory_file = fopen("mem.dat", "w");
	FILE *cache_file = fopen("cache.log", "w");
	FILE *hit_log_file = fopen("hit.log", "w");
	FILE *miss_log_file = fopen("miss.log", "w");

	// Creates memory as an array
	int **main_memory = make_memory(metadata.main_memory_size/LINESIZE, 0);
	int **cache_memory = make_memory(metadata.cache_size/LINESIZE, 1);

	puts("Writing main memory content to mem.dat");
	write_memory_to_file(main_memory, memory_file, metadata.main_memory_size);

	puts("Choose a mapping method:");
	puts("1 - Direct | 2 - Set Associative | 3 - Fully Associative");
	scanf("%d", &mapping_method);

	if (mapping_method < 1 || mapping_method > 3) {
		fprintf(stderr, "Not a valid input. Please restart the program.\n");
	}
	
	char *mapping_method_str;

	switch (mapping_method) {
		case 1:
			mapping_method_str = "DIRECT";
			break;
		case 2:
			mapping_method_str = "SET ASSOCIATIVE";
			break;
		case 3:
			mapping_method_str = "FULLY ASSOCIATIVE";
			break;
		default:
			mapping_method_str = "UNKNOWN";
	}

	fflush(cache_file);

	do {
	char inputted_addr[64]; // Plenty for this use case
	puts("Insert a binary address (use Ctrl + C to exit):");
	scanf("%s", inputted_addr);

	int addr_as_index = strtol(inputted_addr,NULL, 2)/LINESIZE;
	
	fprintf(cache_file, "MAPPING: %s\n", mapping_method_str);
	fprintf(cache_file, "ADDRESS INSERTED: %s\n", inputted_addr);
	
	cache_hit_or_miss = hit_or_miss(cache_memory, main_memory[addr_as_index]);

	if(cache_hit_or_miss == 1) {
		int *data = main_memory[addr_as_index];
		puts("CACHE MISS");
		fprintf(cache_file, "CACHE MISS\n");
		fprintf(miss_log_file, "%s\n", inputted_addr);
		map_to_cache(cache_memory, data, mapping_method, addr_as_index);
	} else if (cache_hit_or_miss == 0) {
		puts("CACHE HIT");
		fprintf(cache_file, "CACHE HIT\n");
		fprintf(hit_log_file, "%s\n", inputted_addr);
	}
	
	log_cache_state(cache_memory, cache_file);
	fflush(cache_file);
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

void write_memory_to_file(int **memory_unit, FILE* file,long memory_size) {
	int num_blocks = memory_size/LINESIZE;
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

void log_cache_state(int **cache, FILE* cache_file) {
	long num_of_lines = metadata.cache_size/LINESIZE;

	for(int i = 0; i < num_of_lines; i++) {
		printf("%02X: [ ", i);
		for (int j = 0; j < LINESIZE; j++) {
			printf("%2x ", cache[i][j]);
		}
		printf("]\n");
	}
	write_memory_to_file(cache, cache_file, metadata.cache_size);
}

int hit_or_miss(int **cache, int *block) {
	long num_of_lines = metadata.cache_size/LINESIZE;

	for(int i = 0; i < num_of_lines; i++) {
		if (compare_data(block, cache[i]) == 0) {
			return 0;
		}
	 }

	return 1;
}

void map_to_cache(int **cache, int *block, int mapping_method, int block_index) {
	long num_of_lines = metadata.cache_size/LINESIZE;
	if (mapping_method == 1) {
		int cache_line = (int) block_index % num_of_lines;
		cache[cache_line] = block;
	} else if (mapping_method == 2) {
		// Who knows?
	} else if (mapping_method == 3) {
		int line_index = rand() % num_of_lines;
		cache[line_index] = block;
	} else {
		fprintf(stderr, "Error! No mapping method found.\n");
	}
}

int compare_data(int *data, int *line) {
    for (int i = 0; i < LINESIZE; i++) {
        if (data[i] != line[i]) {
            return 1; // Lists are not equal
        }
    }
    return 0; // Lists are equal
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
