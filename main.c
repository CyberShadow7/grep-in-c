#include <stdio.h>
#include <string.h>
#include <sys/stat.h>

char* P_NAME = "Grab";
char* SYNTAX = "<program name> [OPTIONS] <file> <pattern>";
char DATA[256];

// Function signatures
void compare(FILE* f, char* string);


// Main
int main(int argc, char** argv) {
    printf("%s: The new grep!\n", P_NAME); // Introduction message. TODO: Make this disappear if -q is passed.
    
    if (argc < 3) {
        printf("[ERROR]: Need 3 arguments!\n");
        printf("Syntax: %s\n", SYNTAX);
        return -1;
    }

    char *string = argv[2];

    // Open the file
    FILE *fptr = fopen(argv[1], "r");

	// Check that file exists
    if (fptr == NULL) {
      perror("[ERROR]: File does NOT exist");
      return 2;
    }

	// stat structure
	/// This structure gets information about the target file and holds many
	/// useful information, like the size of the target file, user GID, etc.
	struct stat sb;

	int result = stat(argv[1], &sb); // Store the result of stat()

	// Check that:
	// 	1. stat() function call completed successfully
	// 	AND
	// 	2. The file size is smaller or equal to 0
	if ((result == 0) && (sb.st_size <= 0)) {
        perror("[ERROR]: File is empty!");
		return 3;
    }
    
	// Compare input string (argv[2]) to lines within the file pointer to find exact matches
    compare(fptr, string);
    
    
    // close file when done
    fclose(fptr);
    
    return 0; // Graceful exit
}

// Compares argv[2] with each line in file referenced by argv[1]
// So far this function only prints exact matches
void compare(FILE* f, char* string) {
    // TODO: Add colored output for matches
    while (fgets(DATA, sizeof(DATA), f) != NULL) {
        DATA[strcspn(DATA, "\n")] = 0; // Strip \n from lines read
        string[strcspn(string, "\n")] = 0; // Strip \n from argv[2]
        
        //printf("[fgets()]: %s\n", DATA); // Debug messages.
        
        if (strcmp(string, DATA)==0) {
            printf("[MATCH]: %s\n", DATA);
        } else {
            printf("-\n");
        }
    }
}