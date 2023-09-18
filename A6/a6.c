#include <sys/mman.h> /*For mmap() system call*/ 
#include <string.h> /*For memcpy function*/ 
#include <fcntl.h> /*For file descriptors*/
#include <stdlib.h> /*For file descriptors*/
#include <stdio.h> /*For printf()*/

#define SIZE 4
#define MEMORY 40
#define BUFFER_SIZE 10
#define OFFSET_MASK 1023
#define PAGES 10
#define OFFSET_BITS 10
#define PAGE_SIZE 1024

char * mmapfptr; // declare a character pointer

int page_table[PAGES]; // declare page_table array

int main(int argc, char const *argv[]) {
    int addr; // virtual address
    int pnumber; // page number
    int offset;
    int physical_addr;
    int frame_number;
    char buff[BUFFER_SIZE];
    int mmapfile_fd = open("pagetable.bin", O_RDONLY);
  
    /* Check if pagetable.bin exists */
    if (mmapfile_fd == -1) {
        printf("File pagetable.bin is not found! \n");
        return -1;
    }

    mmapfptr = mmap(0, MEMORY, PROT_READ, MAP_PRIVATE, mmapfile_fd, 0);
    for (int i = 0; i < PAGES; i++) {
        memcpy(page_table + i, mmapfptr + i*4, SIZE);
    }
    munmap(mmapfptr, MEMORY);

    FILE *fptr = fopen("ltaddr.txt", "r");  

    /* Check if ltaddr.txt exists */
    if (fptr == NULL) {
        printf("File ltaddr.txt is not found! \n");
        return -1;
    }

    while(fgets(buff, BUFFER_SIZE, fptr) != NULL) {
        addr = atoi(buff);
        pnumber = addr >> OFFSET_BITS;
        offset = addr & OFFSET_MASK;
        frame_number = page_table[pnumber];
        physical_addr = ((frame_number << OFFSET_BITS) | offset);
        printf("Virtual addr is %d: Page# = %d & Offset = %d frame number = %d Physical addr = %d.\n", addr, pnumber, offset, frame_number, physical_addr); 
    }
    fclose(fptr);    

    return 0;
}
