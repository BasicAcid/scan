#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <sys/stat.h>
#include <unistd.h>

#define MAX_CMD_LENGTH 1024
#define MAX_PATH_LENGTH 256
#define MAX_TIMESTAMP_LENGTH 32

void initialize_scanner(void);
void scan_document(const char* output_dir, int color_mode, const char* filename);
void print_usage(const char* program_name);

int main(int argc, char *argv[]) {
    int opt;
    int initialize_flag = 0;
    int color_flag = 0;
    char *output_dir = NULL;
    char *filename = NULL;

    // Get default output directory from environment
    output_dir = getenv("SCAN_DIR");
    if (output_dir == NULL) {
        fprintf(stderr, "Please set the env variable \"SCAN_DIR\" first.\n");
        return 1;
    }

    // Parse command line arguments
    while ((opt = getopt(argc, argv, "ico:f:h")) != -1) {
        switch (opt) {
            case 'i':
                initialize_flag = 1;
                break;
            case 'c':
                color_flag = 1;
                break;
            case 'o':
                output_dir = optarg;
                break;
            case 'f':
                filename = optarg;
                break;
            case 'h':
                print_usage(argv[0]);
                return 0;
            default:
                print_usage(argv[0]);
                return 1;
        }
    }

    if (initialize_flag) {
        initialize_scanner();
        return 0;
    }

    scan_document(output_dir, color_flag, filename);
    return 0;
}

void initialize_scanner(void) {
    FILE *fp;
    char buffer[MAX_CMD_LENGTH];

    fp = popen("scanimage -L", "r");
    if (fp == NULL) {
        perror("Failed to execute scanimage command");
        exit(1);
    }

    while (fgets(buffer, sizeof(buffer), fp) != NULL) {
        printf("%s", buffer);
    }

    pclose(fp);
}

void scan_document(const char* output_dir, int color_mode, const char* filename) {
    char final_filename[MAX_PATH_LENGTH];
    char timestamp[MAX_TIMESTAMP_LENGTH];
    char command[MAX_CMD_LENGTH];
    time_t now;
    struct tm *tm_info;
    struct stat st;

    // Generate timestamp and filename if not provided
    if (filename == NULL) {
        time(&now);
        tm_info = localtime(&now);
        strftime(timestamp, MAX_TIMESTAMP_LENGTH, "%Y-%m-%dT%H-%M-%S", tm_info);
        snprintf(final_filename, MAX_PATH_LENGTH, "%s/scan_%s.png", output_dir, timestamp);
    } else {
        snprintf(final_filename, MAX_PATH_LENGTH, "%s/%s", output_dir, filename);
    }

    // Check if file already exists
    if (stat(final_filename, &st) == 0) {
        fprintf(stderr, "Error: File %s already exists. Aborting scan.\n", final_filename);
        return;
    }

    // Construct scanimage command
    snprintf(command, MAX_CMD_LENGTH, "scanimage --format=png %s > \"%s\"",
             color_mode ? "--mode=Color" : "", final_filename);

    // Execute scan command
    if (system(command) != 0) {
        fprintf(stderr, "Error executing scan command\n");
        return;
    }

    printf("Scan saved as %s\n", final_filename);
}

void print_usage(const char* program_name) {
    printf("Usage: %s [options]\n", program_name);
    printf("Options:\n");
    printf("  -i           Initialize the scanner\n");
    printf("  -c           Scan in color\n");
    printf("  -o <dir>     Output directory for scanned files\n");
    printf("  -f <file>    Specify the filename\n");
    printf("  -h           Show this help message\n");
}
