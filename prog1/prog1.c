#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define FILE_NAME "records.dat"
#define TEMP_FILE "temp.dat"

// Define a sample structure for the records
typedef struct {
    int id;
    char name[50];
    float score;
} Record;

// Function Prototypes
void storeRecords(int n);
void getMthRecord(int m);
void deleteRecord(int idToDelete);
void displayAllRecords(); // Helper function to verify results

int main() {
    int choice, n, m, id;

    while (1) {
        printf("\n--- Binary File Operations ---\n");
        printf("1. Store N Records\n");
        printf("2. Display M-th Record (Using fseek)\n");
        printf("3. Delete a Record by ID\n");
        printf("4. Display All Records\n");
        printf("5. Exit\n");
        printf("Enter your choice: ");
        scanf("%d", &choice);

        switch (choice) {
            case 1:
                printf("Enter number of records to store: ");
                scanf("%d", &n);
                storeRecords(n);
                break;
            case 2:
                printf("Enter the record index to fetch (0-indexed): ");
                scanf("%d", &m);
                getMthRecord(m);
                break;
            case 3:
                printf("Enter the Record ID to delete: ");
                scanf("%d", &id);
                deleteRecord(id);
                break;
            case 4:
                displayAllRecords();
                break;
            case 5:
                exit(0);
            default:
                printf("Invalid choice! Please try again.\n");
        }
    }
    return 0;
}

// 1. Function to store N records in a binary file
void storeRecords(int n) {
    FILE *file = fopen(FILE_NAME, "wb"); // Open in write-binary mode
    if (file == NULL) {
        perror("Error opening file");
        return;
    }

    Record r;
    for (int i = 0; i < n; i++) {
        printf("\nEnter details for record %d:\n", i + 1);
        printf("ID: ");
        scanf("%d", &r.id);
        printf("Name: ");
        scanf("%s", r.name);
        printf("Score: ");
        scanf("%f", &r.score);

        // Write the structure block directly to the binary file
        fwrite(&r, sizeof(Record), 1, file);
    }

    fclose(file);
    printf("\nSuccessfully stored %d records.\n", n);
}

// 2. Function to get and display the m-th record using fseek
void getMthRecord(int m) {
    FILE *file = fopen(FILE_NAME, "rb"); // Open in read-binary mode
    if (file == NULL) {
        perror("Error opening file");
        return;
    }

    Record r;
    // Calculate offset: jump 'm' positions from the beginning (SEEK_SET)
    int offset = m * sizeof(Record);
    
    // fseek returns 0 on success
    if (fseek(file, offset, SEEK_SET) == 0) {
        // Attempt to read the record at this position
        if (fread(&r, sizeof(Record), 1, file) == 1) {
            printf("\n--- Record at Index %d ---\n", m);
            printf("ID: %d\n", r.id);
            printf("Name: %s\n", r.name);
            printf("Score: %.2f\n", r.score);
        } else {
            printf("\nError: Record index %d does not exist or is out of bounds.\n", m);
        }
    } else {
        printf("\nError moving file pointer.\n");
    }

    fclose(file);
}

// 3. Function to delete a record (The "Thinking" Part)
void deleteRecord(int idToDelete) {
    FILE *file = fopen(FILE_NAME, "rb");
    FILE *temp = fopen(TEMP_FILE, "wb");

    if (file == NULL) {
        perror("Error opening source file");
        if (temp) fclose(temp);
        return;
    }
    if (temp == NULL) {
        perror("Error opening temporary file");
        fclose(file);
        return;
    }

    Record r;
    int found = 0;

    // Read every record from the source file
    while (fread(&r, sizeof(Record), 1, file) == 1) {
        // If it matches the ID to delete, skip writing it to the temp file
        if (r.id == idToDelete) {
            found = 1;
            continue; 
        }
        // Write all other records to the temp file
        fwrite(&r, sizeof(Record), 1, temp);
    }

    fclose(file);
    fclose(temp);

    if (found) {
        // Remove old file and rename the temporary file to replace it
        remove(FILE_NAME);
        rename(TEMP_FILE, FILE_NAME);
        printf("\nRecord with ID %d successfully deleted.\n", idToDelete);
    } else {
        // Clean up temp file if nothing was actually deleted
        remove(TEMP_FILE);
        printf("\nRecord with ID %d not found.\n", idToDelete);
    }
}

// Helper function to view everything currently in the file
void displayAllRecords() {
    FILE *file = fopen(FILE_NAME, "rb");
    if (file == NULL) {
        printf("\nNo records found or file does not exist.\n");
        return;
    }

    Record r;
    int index = 0;
    printf("\n--- Current File Content ---\n");
    printf("%-5s %-10s %-20s %-10s\n", "Idx", "ID", "Name", "Score");
    while (fread(&r, sizeof(Record), 1, file) == 1) {
        printf("%-5d %-10d %-20s %-10.2f\n", index++, r.id, r.name, r.score);
    }
    fclose(file);
}