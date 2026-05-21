#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define FILE_NAME "records_ascii.txt"
#define MAX_RECORDS 100

// Define the structure
typedef struct {
    int id;
    char name[50];
    float score;
} Student;

// Function Prototypes
void storeASCIIFile(Student arr[], int n);
int createIndexMap(long seek_positions[]);
void displayRecordAtPosition(long position);

int main() {
    // 1. Initialize an array of structures
    Student students[] = {
        {101, "Alice", 92.5},
        {102, "Bob", 88.0},
        {103, "Christopher", 95.75}, // Notice variable name lengths
        {104, "Dan", 73.4},
        {105, "Eva_Marie", 99.1}
    };
    int n = sizeof(students) / sizeof(students[0]);

    // Array to hold the starting byte positions of each record
    long seek_positions[MAX_RECORDS];
    int total_records = 0;

    // Task 1: Store the array of structures in ASCII format
    storeASCIIFile(students, n);

    // Task 2: Map out the starting seek position of each record
    total_records = createIndexMap(seek_positions);

    // Print the index table to understand what happened
    printf("\n--- Generated Seek Positions Array ---\n");
    for (int i = 0; i < total_records; i++) {
        printf("Record %d starts at byte offset: %ld\n", i, seek_positions[i]);
    }

    // Task 3: Display records given a specific seek position
    printf("\n--- Fetching Records using Seek Positions ---\n");
    
    int target_index;
    printf("Enter record index to display (0 to %d): ", total_records - 1);
    if (scanf("%d", &target_index) == 1) {
        if (target_index >= 0 && target_index < total_records) {
            long pos = seek_positions[target_index];
            displayRecordAtPosition(pos);
        } else {
            printf("Invalid index!\n");
        }
    }

    return 0;
}

// 1. Function to store array of structures in ASCII format
void storeASCIIFile(Student arr[], int n) {
    FILE *file = fopen(FILE_NAME, "w"); // "w" for plain text/ASCII mode
    if (file == NULL) {
        perror("Error creating file");
        exit(1);
    }

    for (int i = 0; i < n; i++) {
        // Each record is written as a clear line of text
        fprintf(file, "%d %s %.2f\n", arr[i].id, arr[i].name, arr[i].score);
    }

    fclose(file);
    printf("Successfully wrote %d records to %s in ASCII format.\n", n, FILE_NAME);
}

// 2. Function to create an array of seek positions of the beginning of each record
int createIndexMap(long seek_positions[]) {
    FILE *file = fopen(FILE_NAME, "r");
    if (file == NULL) {
        perror("Error opening file for indexing");
        return 0;
    }

    int count = 0;
    char buffer[256];
    
    // The very first record always starts at byte 0
    long current_pos = ftell(file); 

    // Loop through the file line by line
    while (fgets(buffer, sizeof(buffer), file) != NULL) {
        seek_positions[count] = current_pos;
        count++;

        // Get the position of the NEXT line before we read it
        current_pos = ftell(file); 
        
        if (count >= MAX_RECORDS) break;
    }

    fclose(file);
    return count; // Returns total number of records indexed
}

// 3. Function to display the record given the position
void displayRecordAtPosition(long position) {
    FILE *file = fopen(FILE_NAME, "r");
    if (file == NULL) {
        perror("Error opening file to read record");
        return;
    }

    // Jump directly to the saved byte offset
    if (fseek(file, position, SEEK_SET) == 0) {
        int id;
        char name[50];
        float score;

        // Read and parse the ASCII formatted line at this exact position
        if (fscanf(file, "%d %s %f", &id, &name, &score) == 3) {
            printf("\n[Record Found at Offset %ld]\n", position);
            printf("ID:    %d\n", id);
            printf("Name:  %s\n", name);
            printf("Score: %.2f\n", score);
        } else {
            printf("Error: Could not parse record layout at position %ld\n", position);
        }
    } else {
        printf("Error: Failed to execute fseek to position %ld\n", position);
    }

    fclose(file);
}