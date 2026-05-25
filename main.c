#include <asm-generic/errno-base.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <argp.h>
#include <error.h>
#include <stdlib.h>
#include <stdbool.h>

// Constants for argument parsing (flags)
const char *argp_program_version = "grep-in-c 0.0.4-alpha";
static char doc[] = "A grep clone";

/* Description of accepted arguments */
static char args_doc[] = "[QUIET?][PARTIAL?] FILE PATTERN";

/* Options the program understands */
static struct argp_option options[] = {
    {"quiet", 'q', 0, OPTION_ARG_OPTIONAL, "Don't print app banner"},
    {"file", 'f', "FILE", 0, "File to which the pattern should be applied to"},
    {"pattern", 'p', "PATTERN", 0, "Pattern to search for in <file>"},
    {"partial", 'i', 0, 0, "Partial match pattern against input file"},
    {"debug", 'd', 0, 0, "Show debug output"},
    {"color", 'c', 0, 0, "Color the matched pattern in output"},
    { 0 },
};

/* Used by main to communicate with `parse_opt` */
struct arguments {
    char *args[3]; // Change this every time you expect positional arguments
    int silent;
    char *file;
    char *pattern;
    bool partial;
    bool debug;
    bool color;
};

/* Parse a single argument */
static error_t parse_opt(int key, char *arg, struct argp_state *state)
{
    /* Get `input` argument from argp_parse, which we know is a
    pointer to our arguments structure. */
    struct arguments *arguments = state->input;

    switch (key) {
        case 'q':
            arguments->silent = 1;
            break;
        case 'f':
            arguments->file = arg;
            break;
        case 'p':
            arguments->pattern = arg;
            break;
        case 'i':
            arguments->partial = true;
            break;
        case 'd':
            arguments->debug = true;
            break;
        case 'c':
            arguments->color = true;
            break;
        default:
            //printf("k: %c\n", key);
            return ARGP_ERR_UNKNOWN;
    }
    return 0;
}

/* Our argp parser */
static struct argp argp = { options, parse_opt, args_doc, doc};

// Global variables
char* P_NAME = "GrepInC";
char* SYNTAX = "[OPTIONS] <file> <pattern>";
char DATA[2048];

// Function signatures
void compare(FILE* f, char* pattern, bool color);
void partial(FILE* f, char* pattern, bool color);
void colored_output(char *pattern);

// Main
int main(int argc, char **argv) {

  struct arguments arguments;

  /* Default values */
  arguments.silent = 0;
  arguments.file = "";
  arguments.pattern = "";
  arguments.partial = false;
  arguments.debug = false;
  arguments.color = false;

  /* Parse our arguments; every option seen by parse_opt will be reflected in
   * arguments. */
  argp_parse(&argp, argc, argv, 0, 0, &arguments);

  if (arguments.debug == 1) {
    /* Debugging print */
    printf("arguments.silent: %s\narguments.file: %s\narguments.pattern: "
           "%s\narguments.partial: %s\narguments.debug: %s\n",
           arguments.silent ? "yes" : "no", arguments.file, arguments.pattern,
           arguments.partial ? "yes" : "no", arguments.debug ? "yes" : "no");
  }

  if (arguments.silent != 1) {
    printf("%s: The new grep!\n", P_NAME);
  }

  if (argc < 3) {
    error(2, 0, "[\033[91mERROR\033[0m]: Not enough arguments.\n%s", SYNTAX);
  }

  if (strcmp(arguments.file, "") == 0) {
    error(ENOENT, ENOENT, "[\033[91mERROR\033[0m]: No file supplied");
  }

  if (strcmp(arguments.pattern, "") == 0) {
    error(2, 0, "[\033[91mERROR\033[0m]: No pattern supplied");
  }

  char *pattern = arguments.pattern;
  bool color = arguments.color;

  // Open the file
  FILE *fptr = fopen(arguments.file, "r");

  // Check that file exists
  if (fptr == NULL) {
    error(ENOENT, ENOENT, "[\033[91mERROR\033[0m]");
  }

  // stat structure
  /// This structure gets information about the target file and holds many
  /// useful information, like the size of the target file, user GID, etc.
  struct stat sb;

  int result = stat(arguments.file, &sb); // Store the result of stat()

  // Check that:
  // 	1. stat() function call completed successfully
  // 	AND
  // 	2. The file size is smaller or equal to 0
  if ((result == 0) && (sb.st_size <= 0)) {
    fprintf(stderr, "[\033[91mERROR\033[0m]: File is empty!\n");
    exit(2);
  }

  if (arguments.partial) {
    // Find substring within file pointer (partial matches)
    partial(fptr, pattern, color);
  } else {
    // Compare input string (argv[2]) to lines within the file pointer to find
    // exact matches
    compare(fptr, pattern, color);
  }

  // close file when done
  fclose(fptr);

  return 0; // Graceful exit
}

// Compares argv[2] with each line in file referenced by argv[1]
// So far this function only prints exact matches
void compare(FILE* f, char* pattern, bool color) {
    // TODO: Add colored output for matches
    while (fgets(DATA, sizeof(DATA), f) != NULL) {
        DATA[strcspn(DATA, "\n")] = 0; // Strip \n from lines read
        pattern[strcspn(pattern, "\n")] = 0; // Strip \n from argv[2]
        
        //printf("[fgets()]: %s\n", DATA); // Debug messages.
        
        if (strcmp(pattern, DATA)==0) {
            if (color) {
                printf("\033[91m%s\033[0m\n", DATA);
            } else {
                printf("%s\n", DATA);
            }
        }
    }
}

// Find partial matches between supplied pattern and file
void partial(FILE* f, char* pattern, bool color) {
    // TODO: Add colorizing matching pattern
    while (fgets(DATA, sizeof(DATA), f) != NULL) {

        // Debug print
        //printf("[fgets()]: %s\n", DATA);

        if (strstr(DATA, pattern) != NULL) {
            if (color) {
                colored_output(pattern);
            } else {
                printf("%s\n", DATA);
            }
        }
    }
}

void colored_output(char *pattern) {

    /* Variables needed for the colored output to work */
    char *match_start = strstr(DATA, pattern);
    size_t match_length = strlen(pattern);
    size_t dt_length = strlen(DATA);
    char *after_match = (match_start + match_length);

    /* Print the first segment of text (before the match) */
    fwrite(DATA, 1, (match_start - DATA), stdout);

    /* ANSI escape color code for "Bright Red" */
    fputs("\033[91m", stdout);

    /* Print the match */
    fwrite(match_start, 1, match_length, stdout);

    /* ANSI escape color code "RESET" */
    fputs("\033[0m", stdout);

    /* Print the remaining part of the text. */
    fwrite(after_match, 1, (dt_length - (match_start - DATA) - match_length), stdout);

    /* new line */
    printf("\n");
}