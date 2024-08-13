#ifndef ASSEMBLER_H
#define ASSEMBLER_H

#define MAX_ADDRESS 924

enum {
	output,
	no_output,
};

/**
 * @brief Main function to perform the assembly process.
 *
 * @param file_count Number of files to process.
 * @param name_file Array of file names to process.
 * @return Returns 0 on successful completion.
 */
int assembler(int file_count, char** name_file);


#endif
