#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

int **makeMemory(int rows, int cols){
    int **memoryUnit = (int**)malloc(rows * sizeof(int*));
    if (memoryUnit == NULL) {
        fprintf(stderr, "Memory allocation failed!\n");
        exit(1);
    }

    for (int i = 0; i < rows; i++) {
         memoryUnit[i] = (int*)malloc(cols * sizeof(int));
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
        fprintf(dataFile, "0x%0*x: [ ", lenBlockAddr, i);
        for (int j = 0; j < numWords; j++) {
            fprintf(dataFile, "%02x ", memoryUnit[i][j]);
        }
        fputs("]\n", dataFile);
    }

	fflush(dataFile);
}


void printCacheState(int **memoryUnit, int numBlocks, int numWords) {
    for(int i = 0; i < numBlocks; i++) {
        printf("[ ");
        for (int j = 0; j < numWords; j++) {
            printf("%d ", memoryUnit[i][j]);
        }
        printf("]\n");
    }
}

int main(void) {
    int rows = 128;
    int cols = 16;
    
    int **mainMemory = makeMemory(rows, cols);
    int **cacheMemory = makeMemory(cols, cols);

	FILE* memFile = fopen("mem.dat", "w");

	puts("Populating main memory with data.");
    populateMemory(mainMemory, rows, cols);

	puts("Writing main memory content to file...");
	writeMemoryToFile(mainMemory, rows, cols, memFile);


    puts("Cache memory content:");
    printCacheState(cacheMemory, cols, cols);

    //freeing memory used
    free(mainMemory);
    free(cacheMemory);

    return 0;
}
