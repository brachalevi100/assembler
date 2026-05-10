#ifndef VECTOR_H
#define VECTOR_H

/**
 * @struct Vector
 * A dynamic array structure that holds items with automatic resizing capabilities.
 */
typedef struct vector {
	void** item;                /**< Array of items */
	int count_place;            /**< Total allocated places in the array */
	int count_item;             /**< Current count of items in the array */
	void* (*contractor)(void* copy); /**< Constructor function for items */
	void (*destructor)(void* item); /**< Destructor function for items */
} Vector;

/**
 * Creates and initializes a new Vector.
 *
 * @param contractor A function pointer that constructs a new item.
 * @param destructor A function pointer that destructs an item.
 * @return A new Vector on success, or NULL on memory allocation failure.
 */
Vector* vector(void* (*contractor)(void* copy), void(*destructor)(void* item));

/**
 * Inserts an item into the Vector.
 *
 * @param to_copy The item to be copied and inserted.
 * @param vector The Vector to insert the item into.
 * @return A pointer to the inserted item on success, or NULL on memory allocation failure.
 */
void* insert_vector(void* to_copy, Vector* vector);

/**
 * Retrieves a pointer to the first item in the Vector.
 *
 * @param vector The Vector to retrieve the first item from.
 * @return A pointer to the first item in the Vector.
 */
void** first_vector(Vector* vector);

/**
 * Retrieves a pointer to the last item in the Vector.
 *
 * @param vector The Vector to retrieve the last item from.
 * @return A pointer to the last item in the Vector.
 */
void** last_vector(Vector* vector);

/**
 * Gets the index of a specific item in the Vector.
 *
 * @param vector The Vector to search for the item in.
 * @param item The item to search for.
 * @return The index of the item if found, or -1 if not found.
 */
int get_item_count(Vector* vector, void* item);

/**
 * Frees the memory allocated for the Vector and its items.
 *
 * @param vector A pointer to the Vector to be freed.
 */
void free_vector(Vector** vector);

#endif
