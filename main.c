#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int **makeMemory(int rows, int lineSize);
void populateMemory(int **mem, int numBlocks, int numWords);
int getMemAddressLength(int numBlocks);
void writeMemoryToFile(int **memoryUnit, int numBlocks, int numWords, FILE* dataFile);
void printCacheState(int **memoryUnit, int numBlocks, int numWords);
void printUserPrompt();
int* findDataBlock(int **mainMemory);
void applyMappingMethod(int addr, int **mainMemory, int **cache,int mappingMethodOption);
struct mem getMemorySize();

struct mem {
	long mainMemorySize;
	long cacheSize;
	long numBlocks;
	long numLines;

	// NOTE: Only used in direct mapping
	long numTags;
};

int main(void) {
	printUserPrompt();
	//NOTE: We have hard-coded that each 
	//		line/block contains 16 words
	int lineSize = 16;

	struct mem memData = getMemorySize();

	int **mainMemory = makeMemory(memData.numBlocks, lineSize);
	int **cacheMemory = makeMemory(memData.numLines, lineSize);

	FILE* memFile = fopen("mem.dat", "w");


	puts("Populating main memory with data.");
	populateMemory(mainMemory, memData.numBlocks, lineSize);

	puts("Writing main memory content to file...");
	writeMemoryToFile(mainMemory, memData.numBlocks, lineSize, memFile);


	puts("Cache memory content:");
	printCacheState(cacheMemory, memData.numLines, lineSize);

	for (int i = 0; i < 3; i++) {
		int *data;
		data = findDataBlock(mainMemory);
		printf("%d\n", *data);
	}
	//freeing memory used
	free(mainMemory);
	free(cacheMemory);

	return 0;
}

int **makeMemory(int rows, int lineSize){
	int **memoryUnit = (int**)malloc(rows * sizeof(int*));
	if (memoryUnit == NULL) {
		fprintf(stderr, "Memory allocation failed!\n");
		exit(1);
	}

	for (int i = 0; i < rows; i++) {
		 memoryUnit[i] = (int*)malloc(lineSize * sizeof(int));
		 if (memoryUnit[i] == NULL) {
			fprintf(stderr, "Memory allocation failed!\n");
			exit(1);
		 }
	}

	return memoryUnit;
}

void populateMemory(int **mem, int numBlocks, int numWords) {
	for(int i = 0; i < numBlocks; i++) {
		for(int j = 0; j < numWords; j++) {
			mem[i][j] = rand() % 255;
		}
	}
}

int getMemAddressLength(int numBlocks) {
	int addrLen = (int)floor(log(numBlocks) / log(16));
	return addrLen;
}

void writeMemoryToFile(int **memoryUnit, int numBlocks, int numWords, FILE* dataFile) {

	int lenBlockAddr = getMemAddressLength(numBlocks) + 1;
	int numSpaces = lenBlockAddr + 6; //align word index with data
	char spaces[numSpaces + 1]; //+1 for \0 character
	
	memset(spaces, ' ', numSpaces);
	spaces[numSpaces] = '\0'; // Indicates end of string

	fputs(spaces, dataFile);

	for (int k = 0; k < numWords; k++) {
		if ( k == numWords - 1) {
			fprintf(dataFile, "%02x\n", k);
		} else {
		fprintf(dataFile, "%02x ", k);
		}
	}

	for(int i = 0; i < numBlocks; i++) {
		if (i == 0) {
			fprintf(dataFile, "0x%0*x: [ ", lenBlockAddr, 0);
		} else {
			fprintf(dataFile, "0x%0*x: [ ", lenBlockAddr, i * numWords);
		}

		for (int j = 0; j < numWords; j++) {
			fprintf(dataFile, "%02x ", memoryUnit[i][j]);
		}
		fputs("]\n", dataFile);
	}

	fflush(dataFile);
}

void printCacheState(int **memoryUnit, int numBlocks, int numWords) {
	for(int i = 0; i < numBlocks; i++) {
		printf("%02X: [ ", i);
		for (int j = 0; j < numWords; j++) {
			printf("%d ", memoryUnit[i][j]);
		}
		printf("]\n");
	}
}

void printUserPrompt() {
	puts("\t\t\t\t\t-----------------------------------");
	puts("\t\t\t\t\t| Welcome to the memory simulator |");
	puts("\t\t\t\t\t-----------------------------------");
	puts("\tThis programs mimics the exchange of information between CPU cache and main memory.");
	puts("");

	
	puts("\t\t\t\t\t\t----------------");
	puts("\t\t\t\t\t\t| Instructions |");
	puts("\t\t\t\t\t\t----------------");
	puts("\t* You will now choose the size of the main memory and the size of the cache memory next (all in Bytes).");

	puts(" ");
	puts("\t* You will also choose the size of block/lines of memory units (also in Bytes).");

	puts(" ");
	puts("\t* Lastly, you will choose the mapping method.");

	puts(" ");
	puts("\t* ATTENTION: This programs considers every line/block of memory to be 16 Bytes long.");
	puts("Therefore, inputting values lesser than 128 Bytes for main memory and cache may cause unexpected behaviour.");

	puts(" ");
	puts("\t Let\'s begin!");
	puts("\t\t\t\t----------------------------------------------------------");
}

struct mem getMemorySize() {
	long mainSize;
	long cacheSize;

	puts("How many bytes should the main memory have?");
	scanf("%ld", &mainSize); 

	puts("How many bytes should the cache have?");
	scanf("%ld", &cacheSize); 

	puts("All caculations in this program assume that a word is equal to 1 Byte.");
	puts("We also assume that each line/block of memory contains 16 words.");

	struct mem memData = { .mainMemorySize = mainSize, .cacheSize  = cacheSize, .numTags = mainSize/cacheSize, .numBlocks = mainSize/16, .numLines = cacheSize/16 };

	return memData;
}

int* findDataBlock(int **mainMemory){
	puts("Insert a memory address:");
	long addr = 0;
	scanf("%ld", &addr);
	
	int blockNumber = (int)addr / 16;

	return mainMemory[blockNumber];
}
