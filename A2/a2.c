#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

// macros
#define OFFSET_MASK 255
#define OFFSET_BITS 8
#define PAGES 128
#define PAGE_SIZE 256
#define BUFFER_SIZE 10
#define TLB_SIZE 16
#define BACKING_STORE "BACKING_STORE.bin"
#define INPUT_FILE "addresses.txt"
#define OUTPUT_FILE "output.txt"

// new data structure called TLBentry which stores page number and frame number pair
typedef struct {
  int pnumber;
  int fnumber;
} TLBentry;

// function declarations
void mem_init();
void pagetable_init();
void tlb_init();
int search_TLB(int pnumber);
void TLB_Add(int pnumber, int fnumber);
int TLB_Update(FILE *backingstore_ptr, int pnumber, int fnumber);
int search_page_table(int pnumber);
void page_table_add(int pnumber, int fnumber);

// file pointers
FILE *input_file_pointer;
FILE *backing_store_pointer;
FILE *ouput_file_pointer;

// counter variables
int address_counter = 0;
int TLB_counter = 0;
int TLB_hits = 0;
int faults = 0;
int frame_counter = 0;

// arrays
TLBentry page_table[PAGES];
TLBentry TLB[TLB_SIZE];
signed char memory[PAGES][PAGE_SIZE];
  
char buff[BUFFER_SIZE];
signed char value;
int virtual_addr;

int main(int argc, char *argv[]) {

    int pnumber;
    int offset;
    int fnumber;
    int physical_addr;

    // checking for proper usage
    if (argc != 1)  {
        fprintf(stderr,"Usage: ./assignment2\n");
        return -1;
    }
  
    input_file_pointer = fopen(INPUT_FILE, "r");
    backing_store_pointer = fopen(BACKING_STORE, "rb");
    ouput_file_pointer = fopen(OUTPUT_FILE, "w");

    if (input_file_pointer == NULL) { 
        fprintf(stderr, "Error opening %s\n", INPUT_FILE);        
        return -1;
    } else if (backing_store_pointer == NULL) {
        fprintf(stderr, "Error opening %s\n", BACKING_STORE);
        return -1;
    } else if (input_file_pointer == NULL) {
        fprintf(stderr, "Error opening %s\n", OUTPUT_FILE);
        return -1;
    }
  
    // initialize all arrays: memory, page table, TLB
    mem_init();
    pagetable_init();
    tlb_init();

    // read each number from the address file
    while (fgets(buff, BUFFER_SIZE, input_file_pointer) != NULL) {
        // increment address counter every time we read an address
        address_counter++;
        
        virtual_addr = atoi(buff);
        pnumber = (virtual_addr >> OFFSET_BITS) & OFFSET_MASK;
        offset = virtual_addr & OFFSET_MASK;

        // search for frame number in the TLB
        fnumber = search_TLB(pnumber);
        
        // TLB miss
        if (fnumber == -1) {
        // search the page table for the page number  
        fnumber = search_page_table(pnumber);
        
        // if the page number is not in the page table
        if (fnumber == -1) {
            // calculate a new frame number (using clock arithmetic)
            fnumber = frame_counter++ % PAGES;
            // add an entry to the page table with the page and frame number
            page_table_add(pnumber, fnumber);
            faults++;
    
            // sets the cursor position in backing store with offset and then reads entry into physical memory
            TLB_Update(backing_store_pointer, pnumber, fnumber);
        }
        
            // add entry to the TLB if there was a TLB miss
            TLB_Add(pnumber, fnumber); 
        } else { 
            // increment the number of TLB hits if the page was in the TLB
            TLB_hits++;
        }
        
        // calculate the physical address from the frame number and offsett
        physical_addr = (fnumber << 8) | offset;

        // signed byte value stored at the corresponding physical address  
        value = memory[fnumber][offset];  

        // print data to output file
        fprintf(ouput_file_pointer, "Virtual address: %d Physical address = %d Value=%d \n", virtual_addr, physical_addr, value);  
    }

    // print statistics
    fprintf(ouput_file_pointer,"Total addresses = %d \n", address_counter);
    fprintf(ouput_file_pointer,"Page_faults = %d \n", faults);
    fprintf(ouput_file_pointer,"TLB Hits = %d \n", TLB_hits);

    // close all files
    fclose(input_file_pointer);
    fclose(backing_store_pointer);
    fclose(ouput_file_pointer);

    return 0;
}

// function to initialize the memory table
void mem_init() {
    int i;
    int j;

    for (i = 0; i < PAGES; i++) {
        for(j = 0; j < PAGE_SIZE; j++)
        memory[i][j] = 0;
    }
}

// function to initialize the page table with -1
void pagetable_init() {
    int i;
    for (i = 0; i < PAGES; i++) {
        page_table[i].pnumber = -1;
        page_table[i].fnumber = -1;
    } 
}

// function to initialize the TLB
void tlb_init() {
    int i;
    for (i = 0; i < TLB_SIZE; i++) {
        TLB[i].pnumber = -1;
        TLB[i].fnumber = -1;
    }
}

// function to get the frame corresponding to the given pnumber
int search_page_table(int pnumber) {
    int i;
    for (i = 0; i < PAGES; i++) {
        if(page_table[i].pnumber == pnumber)
        return page_table[i].fnumber;
    }
    // return -1 if frame is not found
    return -1;
}

// function to add an entry to the page table
void page_table_add(int pnumber, int fnumber) {
    page_table[fnumber].pnumber = pnumber;
    page_table[fnumber].fnumber = fnumber;
}

// updates the page entry at the same location as that of pnumber
int TLB_Update(FILE* backingstore_ptr, int pnumber, int fnumber) {
    if (fseek(backingstore_ptr, pnumber * PAGE_SIZE, SEEK_SET) != 0) {
        fprintf(stderr, "Error seeking in backing store\n");
        return -1;
    } else if (fread(memory[fnumber], sizeof(signed char), PAGE_SIZE, backingstore_ptr) == 0) {
        fprintf(stderr, "Error reading from backing store\n");
        return -1;
    }
}

// function to determine if the specified page number is in the TLB
int search_TLB(int pnumber) {
    int i;
    for (i = 0; i < TLB_SIZE; i++) {
        if(TLB[i].pnumber == pnumber)
        return TLB[i].fnumber;
    }
    return -1;
}

// function to add an entry to the TLB
void TLB_Add(int pnumber, int fnumber) {
    TLB[TLB_counter].pnumber = pnumber;
    TLB[TLB_counter].fnumber = fnumber;
    TLB_counter = (TLB_counter + 1) % TLB_SIZE;
}