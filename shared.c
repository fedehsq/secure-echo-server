#include "shared.h"
extern int BUFFER_SIZE;
extern int BLOCK_SIZE;

/* Read config from file and setup global variables */
void setup_params() {
    // body of file
    char *config_strings;
    char* token; 
    // pointer to file
    FILE *config;
    // bytes read from file
    handle_null(config_strings = (char*)calloc(CONFIG_LEN, sizeof(char)), "malloc");
    handle_null(config = fopen(CONFIG, "r"), "open");
    handle_zero(fread(config_strings, sizeof(char), CONFIG_LEN, config), "read");
    token = strtok(config_strings, " \n");
    while (token != NULL) {
        // set buffer size
        if (strncmp(token, "BUFFER_SIZE", strlen("BUFFER_SIZE")) == 0) {
            // control parameters
            char *invalid;
            token = strtok(NULL, " \n");
            BUFFER_SIZE = strtol(token, &invalid, 10);
            if (*invalid != (char)0) {
                fprintf(stderr, "Value must be an integer");
                exit(EXIT_FAILURE);
            } 
            continue;
        }
        // set block size
        if (strncmp(token, "BLOCK_SIZE", strlen("BLOCK_SIZE")) == 0) {
            // control parameters
            char *invalid;
            token = strtok(NULL, " \n");
            BLOCK_SIZE = strtol(token, &invalid, 10);
            if (*invalid != (char)0) {
                fprintf(stderr, "Value must be an integer");
                exit(EXIT_FAILURE);
            } 
            continue;
        }
        token = strtok(NULL, " \n");
    }
    if (fclose(config) != 0) {
        perror("fclose");
    }
    memset(config_strings, 0, sizeof(char));
    free(config_strings);
}