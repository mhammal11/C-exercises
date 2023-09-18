#include <sys/mman.h> /*For mmap() system call*/ 
#include <string.h> /*For memcpy function*/ 
#include <fcntl.h> /*For file descriptors*/
#include <stdlib.h> /*For file descriptors*/
#include <stdio.h> // `printf()`
#include <limits.h>

// macros
#define INPUT_FILE "request.bin"
#define OUTPUT_FILE "output.txt"
#define SIZE 4
#define COUNT 20
#define MEMORY 80 // SIZE * COUNT
#define CYLINDERS 300


FILE *input_file_pointer;
FILE *output_file_pointer;

int requests[COUNT];
int sortedrequests[COUNT];
int fcfs_requests[COUNT];
int sstf_requests[COUNT];
int scan_requests[COUNT];
int cscan_requests[COUNT];
int look_requests[COUNT];
int clook_requests[COUNT];
int start;
char* direction;

char * mmapfptr;

void bubblesort(int *requests_array);
int fcfs(int *requests_array);
int sstf(int *requests_array);
int scan(int *sortedrequests);
int cscan(int *sortedrequests);
int look(int *sortedrequests);
int clook(int *sortedrequests);

int main(int argc, char *argv[]) {
    
    // checking for proper usage
    if (argc != 3)  {
        fprintf(stderr,"Usage: ./assignment3 [initial position] [direction] \n");
        return -1;
    } else if (atoi(argv[1]) >= 0 && atoi(argv[1]) <= 299) {
        start = atoi(argv[1]);
    } else {
        fprintf(stderr, "Invalid initial position \n");
        return -1;
    }

    // checking for valid direction
    if (strcmp(argv[2], "LEFT") == 0) {
        direction = "LEFT";
    } else if (strcmp(argv[2], "RIGHT") == 0) {
        direction = "RIGHT";
    } else {
        fprintf(stderr, "Invalid direction \n");
        return -1;
    }

    int mmapfile_fd = open(INPUT_FILE, O_RDONLY);
    output_file_pointer = fopen(OUTPUT_FILE, "w");

    // checking for files
    if (mmapfile_fd == -1) {
        fprintf(stderr, "Input file %s not found! \n", INPUT_FILE);
        return -1;
    } else if (output_file_pointer == NULL) {
        fprintf(stderr, "Error opening %s\n", OUTPUT_FILE);
        return -1;
    }

    mmapfptr = mmap(0, MEMORY, PROT_READ, MAP_PRIVATE, mmapfile_fd, 0);

    // stores requests into integer array
    for (int i = 0; i < COUNT; i++) {
        memcpy(requests + i, mmapfptr + i*4, SIZE);
    }

    munmap(mmapfptr, MEMORY);

    // sorts the requests in ascending order and stores into sortedrequests array
    bubblesort(requests);

    fprintf(output_file_pointer, "Total requests = %d \n", COUNT);
    fprintf(output_file_pointer, "Initial Head Position: %d \n", start);
    fprintf(output_file_pointer, "Direction of Head: %s \n\n", direction);

    //---------------------------FCFS Section-------------------------
    fprintf(output_file_pointer, "FCFS DISK SCHEDULING ALGORITHM:\n\n");

    // computes the fcfs requests and returns head movement value
    int fcfs_movement = fcfs(requests);

    // writes the requests 
    for (int i = 0; i < COUNT; i++) {
        if (i != COUNT - 1) {
            fprintf(output_file_pointer, "%d, ", fcfs_requests[i]);
        } else {
            fprintf(output_file_pointer, "%d ", fcfs_requests[i]);
        }
    }

    fprintf(output_file_pointer, "\n\nFCFS - Total head movements = %d \n\n", fcfs_movement);

    //---------------------------SSTF Section-------------------------
    fprintf(output_file_pointer, "SSTF DISK SCHEDULING ALGORITHM \n\n");

    // computes the sstf requests and returns head movement value
    int sstf_movement = sstf(requests);

    for (int i = 0; i < COUNT; i++) {
        if (i != COUNT - 1) {
            fprintf(output_file_pointer, "%d, ", sstf_requests[i]);
        } else {
            fprintf(output_file_pointer, "%d ", sstf_requests[i]);
        }
    }

    fprintf(output_file_pointer, "\n\nSSTF - Total head movements = %d \n\n", sstf_movement);

    //---------------------------SCAN Section-------------------------
    fprintf(output_file_pointer, "SCAN DISK SCHEDULING ALGORITHM \n\n");

    // computes the sstf requests (using sorted array) and returns head movement value
    int scan_movement = scan(sortedrequests);

    for (int i = 0; i < COUNT; i++) {
        if (i != COUNT - 1) {
            fprintf(output_file_pointer, "%d, ", scan_requests[i]);
        } else {
            fprintf(output_file_pointer, "%d ", scan_requests[i]);
        }
    }

    fprintf(output_file_pointer, "\n\nSCAN - Total head movements = %d \n\n", scan_movement);
    //---------------------------CSCAN Section-------------------------
    fprintf(output_file_pointer, "C-SCAN DISK SCHEDULING ALGORITHM \n\n");

    // computes the scan requests (using sorted array) and returns head movement value
    int cscan_movement = cscan(sortedrequests);

    for (int i = 0; i < COUNT; i++) {
        if (i != COUNT - 1) {
            fprintf(output_file_pointer, "%d, ", cscan_requests[i]);
        } else {
            fprintf(output_file_pointer, "%d ", cscan_requests[i]);
        }
    }

    fprintf(output_file_pointer, "\n\nC-SCAN - Total head movements = %d \n\n", cscan_movement);
    //---------------------------LOOK Section-------------------------
    fprintf(output_file_pointer, "LOOK DISK SCHEDULING ALGORITHM \n\n");

    // computes the look requests (using sorted array) and returns head movement value
    int look_movement = look(sortedrequests);

    for (int i = 0; i < COUNT; i++) {
        if (i != COUNT - 1) {
            fprintf(output_file_pointer, "%d, ", look_requests[i]);
        } else {
            fprintf(output_file_pointer, "%d ", look_requests[i]);
        }
    }

    fprintf(output_file_pointer, "\n\nLOOK - Total head movements = %d \n\n", look_movement);
    //---------------------------C-LOOK Section-------------------------
    fprintf(output_file_pointer, "C-LOOK DISK SCHEDULING ALGORITHM \n\n");

    // computes the clook requests (using sorted array) and returns head movement value
    int clook_movement = clook(sortedrequests);

    for (int i = 0; i < COUNT; i++) {
        if (i != COUNT - 1) {
            fprintf(output_file_pointer, "%d, ", clook_requests[i]);
        } else {
            fprintf(output_file_pointer, "%d ", clook_requests[i]);
        }
    }

    fprintf(output_file_pointer, "\n\nC-LOOK - Total head movements = %d ", clook_movement);
    return 0;
}

void bubblesort(int *requests_array) {
    // populates sortedrequests array using bubblesort
    for (int i = 0; i < COUNT; i++) {
         sortedrequests[i] = requests_array[i];
    }

    for (int i = 0; i < COUNT; i++) {
        for (int j = 0; j < COUNT - 1; j++) {
            if (sortedrequests[j] > sortedrequests[j+1]) {
                int temp = sortedrequests[j];
                sortedrequests[j] = sortedrequests[j+1];
                sortedrequests[j+1] = temp;
            }
        }
    }
}

int fcfs(int *requests_array) {
    int head_movement = 0;
    int current = start;

    // fills fcfs_requests in the order they arrive
    for (int i = 0; i < COUNT; i++) {
        head_movement += abs(requests_array[i] - current);
        current = requests_array[i];
        fcfs_requests[i] = requests_array[i];
    }

    return head_movement;
}

int sstf(int *requests_array) {
    int head_movement = 0;
    int counter = 0;
    int current = start;
    int temp_requests[COUNT];

    // temporary array
    for (int i = 0; i < COUNT; i++) {
         temp_requests[i] = requests_array[i];
    }

	while(counter != COUNT) {
        int min = INT_MAX;
        int index = 0;

        // finds next shortest seek time
        for (int i = 0; i < COUNT; i++) {
           int diff = abs(temp_requests[i] - current);
           if (diff < min) {
               min = diff;
               index = i;
            }
        }
        
        head_movement += min;
        current = temp_requests[index];
        sstf_requests[counter] = temp_requests[index];
        // sets already satisfied requests to a high value 
        temp_requests[index] = INT_MAX;
        counter++;
    }

	return head_movement;
}

int scan(int *sortedrequests) {
    int partition = 0;
    int current = start;
    int index;
    
    if (strcmp(direction, "LEFT") == 0) {
        // computes the boundary where the left is less than current and right is greather than current
        for (int i = 0; i < COUNT; i++) {
            if (current >= sortedrequests[i]) {
                partition++;
            }
        }

        index = partition;
        int countleft = 0;

        while (index != 0) {
            scan_requests[countleft] = sortedrequests[index - 1];
            countleft++;
            index--;
        }

        while (partition != COUNT) {
            scan_requests[partition] = sortedrequests[partition];
            partition++;
        }

        return abs(current - 0) + abs(0 - sortedrequests[COUNT - 1]);
    } else if (strcmp(direction, "RIGHT") == 0) {
        // computes the boundary where the left is less than current and right is greather than current
        for (int i = 0; i < COUNT; i++) {
            if (current > sortedrequests[i]) {
                partition++;
            }
        }

        index = 0;
        int countright = partition;

        while (countright != COUNT) {
            scan_requests[index] = sortedrequests[countright];
            index++;
            countright++;
        }

        while (partition != 0) {
            scan_requests[COUNT - partition] = sortedrequests[partition - 1];
            partition--;
        }

        return abs(current - (CYLINDERS - 1)) + abs((CYLINDERS - 1) - sortedrequests[0]);
    } else {
        fprintf(stderr, "Something went wrong with scan! \n");
        return -1;
    }
}

int cscan(int *sortedrequests) {
    int partition = 0;
    int current = start;
    int index;

    if (strcmp(direction, "LEFT") == 0) {
        //computes the boundary where the left is less than current and right is greather than current
        for (int i = 0; i < COUNT; i++) {
            if (current >= sortedrequests[i]) {
                partition++;
            }
        }

        index = partition;
        int countleft = 0;
        int end = sortedrequests[partition];

        while (index != 0) {
            cscan_requests[countleft] = sortedrequests[index - 1];
            countleft++;
            index--;
        }

        index = COUNT - 1;
        while (partition != COUNT) {
            cscan_requests[partition] = sortedrequests[index];
            partition++;
            index--;
        }

        return abs(current - 0) + abs(0 - (CYLINDERS - 1)) + abs((CYLINDERS - 1) - end);
    } else if (strcmp(direction, "RIGHT") == 0) {
        //computes the boundary where the left is less than current and right is greather than current
        for (int i = 0; i < COUNT; i++) {
            if (current > sortedrequests[i]) {
                partition++;
            }
        }

        index = 0;
        int countright = partition;
        int end = sortedrequests[partition - 1];

        while (countright != COUNT) {
            cscan_requests[index] = sortedrequests[countright];
            index++;
            countright++;
        }

        index = 0;
        while (partition != 0) {
            cscan_requests[COUNT - partition] = sortedrequests[index];
            partition--;
            index++;
        }

        return abs(current - (CYLINDERS - 1)) + abs((CYLINDERS - 1) - 0) + abs(0 - end);
    } else {
        fprintf(stderr, "Something went wrong with c-scan! \n");
        return -1;
    } 
}

int look(int *sortedrequests) {
    int partition = 0;
    int current = start;
    int index;

    if (strcmp(direction, "LEFT") == 0) {
        //computes the boundary where the left is less than current and right is greather than current
        for (int i = 0; i < COUNT; i++) {
            if (current >= sortedrequests[i]) {
                partition++;
            }
        }

        index = partition;
        int countleft = 0;

        while (index != 0) {
            look_requests[countleft] = sortedrequests[index - 1];
            countleft++;
            index--;
        }

        while (partition != COUNT) {
            look_requests[partition] = sortedrequests[partition];
            partition++;
        }

        return abs(current - sortedrequests[0]) + abs(sortedrequests[0] - sortedrequests[COUNT - 1]);
    } else if (strcmp(direction, "RIGHT") == 0) {
        //computes the boundary where the left is less than current and right is greather than current
        for (int i = 0; i < COUNT; i++) {
            if (current > sortedrequests[i]) {
                partition++;
            }
        }

        index = 0;
        int countright = partition;

        while (countright != COUNT) {
            look_requests[index] = sortedrequests[countright];
            index++;
            countright++;
        }

        while (partition != 0) {
            look_requests[COUNT - partition] = sortedrequests[partition - 1];
            partition--;
        }

        return abs(current - sortedrequests[COUNT - 1]) + abs(sortedrequests[COUNT - 1] - sortedrequests[0]);
    } else {
        fprintf(stderr, "Something went wrong with look! \n");
        return -1;
    }
}

int clook(int *sortedrequests) {
    int partition = 0;
    int current = start;
    int index;

    if (strcmp(direction, "LEFT") == 0) {
        //computes the boundary where the left is less than current and right is greather than current
        for (int i = 0; i < COUNT; i++) {
            if (current >= sortedrequests[i]) {
                partition++;
            }
        }

        index = partition;
        int countleft = 0;
        int end = sortedrequests[partition];

        while (index != 0) {
            clook_requests[countleft] = sortedrequests[index - 1];
            countleft++;
            index--;
        }

        index = COUNT - 1;
        while (partition != COUNT) {
            clook_requests[partition] = sortedrequests[index];
            partition++;
            index--;
        }

        return abs(current - sortedrequests[0]) + abs(sortedrequests[0] - sortedrequests[COUNT - 1]) + abs(sortedrequests[COUNT - 1] - end);
    } else if (strcmp(direction, "RIGHT") == 0) {
        //computes the boundary where the left is less than current and right is greather than current
        for (int i = 0; i < COUNT; i++) {
            if (current > sortedrequests[i]) {
                partition++;
            }
        }

        index = 0;
        int countright = partition;
        int end = sortedrequests[partition - 1];

        while (countright != COUNT) {
            clook_requests[index] = sortedrequests[countright];
            index++;
            countright++;
        }

        index = 0;
        while (partition != 0) {
            clook_requests[COUNT - partition] = sortedrequests[index];
            partition--;
            index++;
        }

        return abs(current - sortedrequests[COUNT - 1]) + abs(sortedrequests[COUNT - 1] - sortedrequests[0]) + abs(sortedrequests[0] - end);

    } else {
        fprintf(stderr, "Something went wrong with c-look! \n");
        return -1;
    } 
}