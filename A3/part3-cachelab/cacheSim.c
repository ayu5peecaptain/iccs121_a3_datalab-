#include "cacheSim.h"
/*
Block size = 16 bytes 
L1 = 64 bytes total, 2-way set associative 
L2 = 256 bytes total, 4-way set associative 
*/
void insertL1(u_int32_t address, unsigned char block[16]);
void insertL2(u_int32_t address, unsigned char block[16]);
void writeByte(u_int32_t address, unsigned char value);

// In this question, we will assume DRAM will take a 4-byte values starting from 0 to 
long insertCounter = 0; 
void init_DRAM()
{
	unsigned int i=0;
	DRAM = malloc(sizeof(char) * DRAM_SIZE);
	for(i=0;i<DRAM_SIZE/4;i++)
	{
		*((unsigned int*)DRAM+i) = i;
	}
}

void printCache()
{
	// printf("result -> %d\n", L1_cache[0][0]);
	int i,j,k;
	printf("===== L1 Cache Content =====\n");
	// Number of sets = cache_size / (block_size x ways)
	

	for(i=0;i<2;i++)
	{
		printf("Set %d :", i);
		for(j=0;j<2;j++)
		{
			printf(" {(TAG: 0x%x)", (unsigned int)(L1_cache[i][j].tag));
			for(k=0;k<16;k++)
				printf(" 0x%x,", (unsigned int)(L1_cache[i][j].data[k]));
			printf(" |");
		}
		printf("\n");
	}
	printf("===== L2 Cache Content =====\n");
	for(i=0;i<4;i++)
	{
		printf("Set %d :", i);
		for(j=0;j<4;j++)
		{
			printf(" {(TAG: 0x%x)", (unsigned int)(L2_cache[i][j].tag));
			for(k=0;k<16;k++)
				printf(" 0x%x,", (unsigned int)(L2_cache[i][j].data[k]));
			printf(" |");
		}
		printf("\n");
	}
}
void insertL1(u_int32_t address,unsigned char block[16])
{
	unsigned int setID = getL1SetID(address);
	unsigned int tag = getL1Tag(address);

	// find the way w/ the smallest timeStamp in this set 
	int victim = 0; 
	for(int way = 1; way < 2; way++){
		if (L1_cache[setID][way].timeStamp < L1_cache[setID][victim].timeStamp){
			victim = way;
		}
	}
	// Overwrite that way w/ the new block 
	L1_cache[setID][victim].tag = tag; 

	// copy 'block' into L1_cache[setID][victim].data
	for (int i = 0; i < 16; i++) {
    	L1_cache[setID][victim].data[i] = block[i];
}	
	insertCounter++;
	L1_cache[setID][victim].timeStamp = insertCounter;
}
void insertL2(u_int32_t address,unsigned char block[16])
{
	unsigned int setID = getL2SetID(address);
	unsigned int tag = getL2Tag(address);

	// find the way w/ the smallest timeStamp in this set 
	int victim = 0; 
	for(int way = 1; way < 4; way++){
		if (L2_cache[setID][way].timeStamp < L2_cache[setID][victim].timeStamp){
			victim = way;
		}
	}
	// Overwrite that way w/ the new block 
	L2_cache[setID][victim].tag = tag; 
	// copy 'block' into L2_cache[setID][victim].data
	for (int i = 0; i < 16; i++) {
    	L2_cache[setID][victim].data[i] = block[i];
}
	insertCounter++;
	L2_cache[setID][victim].timeStamp = insertCounter;
}
unsigned char readByte(u_int32_t address)
{
    unsigned int offset = address & 0xF;

    if (L1lookup(address)) {
        unsigned int setID = getL1SetID(address);
        unsigned int tag = getL1Tag(address);
        int way;
        for (way = 0; way < 2; way++) {
            if (L1_cache[setID][way].tag == tag) break;
        }
        return L1_cache[setID][way].data[offset];

    } else if (L2lookup(address)) {
        unsigned int setID = getL2SetID(address);
        unsigned int tag = getL2Tag(address);
        int way;
        for (way = 0; way < 4; way++) {
            if (L2_cache[setID][way].tag == tag) break;
        }
        insertL1(address, L2_cache[setID][way].data);
        return L2_cache[setID][way].data[offset];

    } else {
        unsigned int blockStart = address & ~0xF;
        unsigned char block[16];
        for (int i = 0; i < 16; i++) {
            block[i] = DRAM[blockStart + i];
        }
        insertL1(address, block);
		insertL2(address, block);

        return block[offset];
    }
}
u_int32_t read_fifo(u_int32_t address)
{
    unsigned char b0 = readByte(address);
    unsigned char b1 = readByte(address + 1);
    unsigned char b2 = readByte(address + 2);
    unsigned char b3 = readByte(address + 3);

    return (b0 << 0) | (b1 << 8) | (b2 << 16) | (b3 << 24);
}

int L1lookup(u_int32_t address)
{
///// IMPLEMENT THIS /////
unsigned int setID = getL1SetID(address);
unsigned int tag = getL1Tag(address);
for (int way = 0; way< 2;way++){
	if (L1_cache[setID][way].tag == tag && L1_cache[setID][way].timeStamp != 0){
		return 1;
	}
}
return 0; // no match found = miss 
}



int L2lookup(u_int32_t address)
{
///// IMPLEMENT THIS /////
unsigned int setID = getL2SetID(address);
unsigned int tag = getL2Tag(address);
for(int way = 0; way < 4; way++){
	if(L2_cache[setID][way].tag == tag && L2_cache[setID][way].timeStamp != 0){
		return 1;
	}
}
return 0;
}

unsigned int getL1SetID(u_int32_t address)
{
///// IMPLEMENT THIS /////
return (address >> 4) & 0x1;
}

unsigned int getL2SetID(u_int32_t address)
{
///// IMPLEMENT THIS /////
return (address >> 4) & 0x3;
}

unsigned int getL1Tag(u_int32_t address)
{
///// IMPLEMENT THIS /////
return address >> 5;
}

unsigned int getL2Tag(u_int32_t address)
{
///// IMPLEMENT THIS /////
return (address >> 6);
}



void writeByte(u_int32_t address, unsigned char value)
{
    DRAM[address] = value;

    int inL1 = L1lookup(address);
    int inL2 = L2lookup(address);

    if (inL1) {
        unsigned int setID = getL1SetID(address);
        unsigned int tag = getL1Tag(address);
        unsigned int offset = address & 0xF;
        int way;
        for (way = 0; way < 2; way++) {
            if (L1_cache[setID][way].tag == tag) break;
        }
        L1_cache[setID][way].data[offset] = value;
    }

    if (inL2) {
        unsigned int setID = getL2SetID(address);
        unsigned int tag = getL2Tag(address);
        unsigned int offset = address & 0xF;
        int way;
        for (way = 0; way < 4; way++) {
            if (L2_cache[setID][way].tag == tag) break;
        }
        L2_cache[setID][way].data[offset] = value;
    }

    if (!inL1 && !inL2) {
        unsigned int blockStart = address & ~0xF;
        unsigned char block[16];
        for (int i = 0; i < 16; i++) {
            block[i] = DRAM[blockStart + i];   // includes the just-written "value"
        }
        insertL1(address, block);
        insertL2(address, block);
    }
}
void write(u_int32_t address, u_int32_t data)
{
    writeByte(address,     (data >> 0)  & 0xFF);
    writeByte(address + 1, (data >> 8)  & 0xFF);
    writeByte(address + 2, (data >> 16) & 0xFF);
    writeByte(address + 3, (data >> 24) & 0xFF);
}


int main()
{
	
	init_DRAM();
	cacheAccess buffer;
	int timeTaken=0;
	FILE *trace = fopen("input.trace","r");
	int L1hit = 0;
	int L2hit = 0;
	cycles = 0;
	while(!feof(trace))
	{
		fscanf(trace,"%d %x %x", &buffer.readWrite, &buffer.address, &buffer.data);
		printf("Processing the request for [R/W] = %d, Address = %x, data = %x\n", buffer.readWrite, buffer.address, buffer.data);

		// Checking whether the current access is a hit or miss so that we can advance time correctly
		if(L1lookup(buffer.address))// Cache hit
		{
			timeTaken = 1;
			L1hit++;
		}
		else if(L2lookup(buffer.address))// L2 Cache Hit
		{
			L2hit++;
			timeTaken = 5;
		}
		else timeTaken = 50;
		if (buffer.readWrite) write(buffer.address, buffer.data);
		else read_fifo(buffer.address);
		cycles+=timeTaken;
	}
	printCache();
	printf("Total cycles used = %ld\nL1 hits = %d, L2 hits = %d", cycles, L1hit, L2hit);
	fclose(trace);
	free(DRAM);
	return 0;
}




