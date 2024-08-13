#ifndef PREPROSESOR_H
#define PREPROSESOR_H
 
/**
 * Preprocesses the assembly source file, handling macro definitions and expansions.
 *
 * @param file_base_name The base name of the source file (without extension).
 * @param order_tree A pointer to the order tree data structure.
 * @return Returns SUCCESS if preprocessing is successful, REAL_ERROR if there's a critical error,
 *         or an appropriate error code otherwise.
 */
int preprocessor(const char*, Tree*);

#endif 
