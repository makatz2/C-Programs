#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <string.h>
#include "mem.h"

/*
 * This structure serves as the header for each allocated and free block
 * It also serves as the footer for each free block
 * The blocks are ordered in the increasing order of addresses 
 */
typedef struct blk_hdr {                         
        int size_status;
  
    /*
    * Size of the block is always a multiple of 8
    * => last two bits are always zero - can be used to store other information
    *
    * LSB -> Least Significant Bit (Last Bit)
    * SLB -> Second Last Bit 
    * LSB = 0 => free block
    * LSB = 1 => allocated/busy block
    * SLB = 0 => previous block is free
    * SLB = 1 => previous block is allocated/busy
    * 
    * When used as the footer the last two bits should be zero
    */

    /*
    * Examples:
    * 
    * For a busy block with a payload of 20 bytes (i.e. 20 bytes data + an additional 4 bytes for header)
    * Header:
    * If the previous block is allocated, size_status should be set to 27
    * If the previous block is free, size_status should be set to 25
    * 
    * For a free block of size 24 bytes (including 4 bytes for header + 4 bytes for footer)
    * Header:
    * If the previous block is allocated, size_status should be set to 26
    * If the previous block is free, size_status should be set to 24
    * Footer:
    * size_status should be 24
    * 
    */
} blk_hdr;

/* Global variable - This will always point to the first block
 * i.e. the block with the lowest address */
blk_hdr *first_blk = NULL;

/*
 * Note: 
 *  The end of the available memory can be determined using end_mark
 *  The size_status of end_mark has a value of 1
 *
 */

/* 
 * Function for allocating 'size' bytes
 * Returns address of allocated block on success 
 * Returns NULL on failure 
 * Here is what this function should accomplish 
 * - Check for sanity of size - Return NULL when appropriate 
 * - Round up size to a multiple of 8 
 * - Traverse the list of blocks and allocate the best free block which can accommodate the requested size 
 * - Also, when allocating a block - split it into two blocks
 * Tips: Be careful with pointer arithmetic 
 */
void* Mem_Alloc(int size) {                      
	int newBlkSize;
    // Check that the size requested is greater than 0
	if(size <= 0){
		return NULL;
	}
	// Compute the size needed (add four for the header block) and find remainder
	// when divided into 8 byte blocks.
	int roundUp = (size + sizeof(blk_hdr)) % 8;
	// If 0 then the new block is size + 4 otherwise
    	if(roundUp != 0){
		newBlkSize = size + sizeof(blk_hdr) + 8 - roundUp;
	}else{
		newBlkSize = size + sizeof(blk_hdr);
	}
	// Traverse list to find best block
	// header to store the current best header found
	blk_hdr *bestBlkHeader = NULL;
	// header to store the current block you are
	blk_hdr *currBlkHeader = first_blk;
	while(1){
		// check if you have reached the end of the heap		
		int lsbs = currBlkHeader->size_status % 8;
		int currSize = (currBlkHeader->size_status - lsbs);	
		if(currBlkHeader->size_status == 1){
			// *******Check whether or not this can be checked for Null
			if(bestBlkHeader == NULL){
				return NULL;
			}	
			break;
		// check if the current block is allocated	
		}else if((currBlkHeader->size_status & 1) == 0){
			// If size of current block is equal to size needed then set to best and break
			if(newBlkSize == currSize){
				bestBlkHeader = currBlkHeader;
				break;	
			// If size of the block is large enough to fit and is a better fit than currBest
			}else if(newBlkSize < currSize){
				if(bestBlkHeader == NULL){	
					bestBlkHeader = currBlkHeader;	
				}else if(currSize < bestBlkHeader->size_status){
					bestBlkHeader = currBlkHeader;	
				}	
			}				
		}
		// Move to next blk
		currBlkHeader = (blk_hdr*)((char*)currBlkHeader + currSize);	
	}
	//Allocate bestBlk
	int lsbs = bestBlkHeader->size_status % 8;
	int bestSize = (bestBlkHeader->size_status - lsbs);
	int splitBlkSize = bestSize - newBlkSize;	
	if(bestSize != newBlkSize){
		//split block
		//correct header for newly allocated blk
		bestBlkHeader->size_status = newBlkSize + lsbs + 1;
		//create new header for split blk
		blk_hdr *splitBlk = (blk_hdr*) ((char*)bestBlkHeader + newBlkSize);
		splitBlk->size_status = splitBlkSize + 2;
		//update footer for split blk
		blk_hdr *newFooter = (blk_hdr*)((char*)splitBlk + splitBlkSize - 4);
		newFooter->size_status = splitBlkSize;
		
	}else{
		//mark block as allocated.
		bestBlkHeader->size_status += 1;
		blk_hdr *nextblk = (blk_hdr*)((char*)bestBlkHeader + bestSize);
		nextblk->size_status += 2;
	}
	
	return ((char*) bestBlkHeader + 4) ;
}

/* 
 * Function for freeing up a previously allocated block 
 * Argument - ptr: Address of the block to be freed up 
 * Returns 0 on success 
 * Returns -1 on failure 
 * Here is what this function should accomplish 
 * - Return -1 if ptr is NULL
 * - Return -1 if ptr is not 8 byte aligned or if the block is already freed
 * - Mark the block as free 
 * - Coalesce if one or both of the immediate neighbours are free 
 */
int Mem_Free(void *ptr) {                        
    // check if pointer is null
	if(ptr == NULL){
		return -1;
	}
	blk_hdr* currHead = (blk_hdr*)((char*)ptr - 4);
	int lsb = currHead->size_status % 8;
	int size = currHead->size_status - lsb;
	if((int)ptr % 8 != 0 || lsb % 2 == 0){
		return -1;
	}
	int newSize = size;
	//check if previous block is free
	if(lsb == 1){
		//get size of previous block.
		blk_hdr *prevFoot = (blk_hdr*)((char*)ptr - 8);
		int prevSize = prevFoot->size_status;
		// get to prevHeader
		currHead  = (blk_hdr*)((char*)ptr - prevSize - 4);
		newSize += prevSize;
	}
	// check if next block is free
	blk_hdr *nextHead = (blk_hdr*)((char*)ptr + size - 4);
 	if(nextHead->size_status % 2 == 0){
		int lsb2 = nextHead->size_status % 8;
		newSize += nextHead->size_status - lsb2;
	}else{
		nextHead->size_status -= 2;
	}	 	
	
	// Set header of new block
	currHead->size_status = newSize + 2;
	// Set footer of new block
	blk_hdr *newFooter = (blk_hdr*)((char*)currHead + newSize - 4);
	newFooter->size_status = newSize;
	// set prev block free in next block
    	return 0;
}

/*
 * Function used to initialize the memory allocator
 * Not intended to be called more than once by a program
 * Argument - sizeOfRegion: Specifies the size of the chunk which needs to be allocated
 * Returns 0 on success and -1 on failure 
 */
int Mem_Init(int sizeOfRegion) {                         
    int pagesize;
    int padsize;
    int fd;
    int alloc_size;
    void* space_ptr;
    blk_hdr* end_mark;
    static int allocated_once = 0;
  
    if (0 != allocated_once) {
        fprintf(stderr, 
        "Error:mem.c: Mem_Init has allocated space during a previous call\n");
        return -1;
    }
    if (sizeOfRegion <= 0) {
        fprintf(stderr, "Error:mem.c: Requested block size is not positive\n");
        return -1;
    }

    // Get the pagesize
    pagesize = getpagesize();

    // Calculate padsize as the padding required to round up sizeOfRegion 
    // to a multiple of pagesize
    padsize = sizeOfRegion % pagesize;
    padsize = (pagesize - padsize) % pagesize;

    alloc_size = sizeOfRegion + padsize;

    // Using mmap to allocate memory
    fd = open("/dev/zero", O_RDWR);
    if (-1 == fd) {
        fprintf(stderr, "Error:mem.c: Cannot open /dev/zero\n");
        return -1;
    }
    space_ptr = mmap(NULL, alloc_size, PROT_READ | PROT_WRITE, MAP_PRIVATE, 
                    fd, 0);
    if (MAP_FAILED == space_ptr) {
        fprintf(stderr, "Error:mem.c: mmap cannot allocate space\n");
        allocated_once = 0;
        return -1;
    }
 	printf("here"); 
     allocated_once = 1;
	double tes = 11.5;
	printf("%f", tes);
    // for double word alignement and end mark
    alloc_size -= 8;

    // To begin with there is only one big free block
    // initialize heap so that first block meets 
    // double word alignement requirement
    first_blk = (blk_hdr*) space_ptr + 1;
    end_mark = (blk_hdr*)((void*)first_blk + alloc_size);
  
    // Setting up the header
    first_blk->size_status = alloc_size;

    // Marking the previous block as busy
    first_blk->size_status += 2;

    // Setting up the end mark and marking it as busy
    end_mark->size_status = 1;

    // Setting up the footer
    blk_hdr *footer = (blk_hdr*) ((char*)first_blk + alloc_size - 4);
    footer->size_status = alloc_size;
  
    return 0;
}

/* 
 * Function to be used for debugging 
 * Prints out a list of all the blocks along with the following information i
 * for each block 
 * No.      : serial number of the block 
 * Status   : free/busy 
 * Prev     : status of previous block free/busy
 * t_Begin  : address of the first byte in the block (this is where the header starts) 
 * t_End    : address of the last byte in the block 
 * t_Size   : size of the block (as stored in the block header) (including the header/footer)
 */ 
void Mem_Dump() {                        
    int counter;
    char status[5];
    char p_status[5];
    char *t_begin = NULL;
    char *t_end = NULL;
    int t_size;

    blk_hdr *current = first_blk;
    counter = 1;

    int busy_size = 0;
    int free_size = 0;
    int is_busy = -1;

    fprintf(stdout, "************************************Block list***\
                    ********************************\n");
    fprintf(stdout, "No.\tStatus\tPrev\tt_Begin\t\tt_End\t\tt_Size\n");
    fprintf(stdout, "-------------------------------------------------\
                    --------------------------------\n");
  
    while (current->size_status != 1) {
        t_begin = (char*)current;
        t_size = current->size_status;
    
        if (t_size & 1) {
            // LSB = 1 => busy block
            strcpy(status, "Busy");
            is_busy = 1;
            t_size = t_size - 1;
        } else {
            strcpy(status, "Free");
            is_busy = 0;
        }

        if (t_size & 2) {
            strcpy(p_status, "Busy");
            t_size = t_size - 2;
        } else {
            strcpy(p_status, "Free");
        }

        if (is_busy) 
            busy_size += t_size;
        else 
            free_size += t_size;

        t_end = t_begin + t_size - 1;

        fprintf(stdout, "%d\t%s\t%s\t0x%08lx\t0x%08lx\t%d\n", counter, status, 
        p_status, (unsigned long int)t_begin, (unsigned long int)t_end, t_size);
    
        current = (blk_hdr*)((char*)current + t_size);
        counter = counter + 1;
    }

    fprintf(stdout, "---------------------------------------------------\
                    ------------------------------\n");
    fprintf(stdout, "***************************************************\
                    ******************************\n");
    fprintf(stdout, "Total busy size = %d\n", busy_size);
    fprintf(stdout, "Total free size = %d\n", free_size);
    fprintf(stdout, "Total size = %d\n", busy_size + free_size);
    fprintf(stdout, "***************************************************\
                    ******************************\n");
    fflush(stdout);

    return;
}
