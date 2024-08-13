#include "vector.h"
#include <stdlib.h>
#define BABASES_PLACE 10


Vector* vector(void* (*contractor)(void* copy), void(*destructor)(void* item)) {
	Vector* new_vector;
	new_vector = calloc(1, sizeof(Vector));
	if (new_vector == NULL)
		return NULL;
	new_vector->item = calloc(BABASES_PLACE, sizeof(void*));
	if (new_vector->item == NULL) {
		free(new_vector);
		return NULL;
	}
	new_vector->contractor = contractor;
	new_vector->destructor = destructor;
	new_vector->count_item = 0;
	new_vector->count_place = BABASES_PLACE;

	return 	new_vector;
}

void* insert_vector(void* to_copy, Vector* vector) {
	int i;
	void** temp;
	if (vector->count_item == vector->count_place) {
		temp = realloc(vector->item, (vector->count_place + BABASES_PLACE) * sizeof(void*));
		if (temp == NULL) {
			return NULL;
		}
		vector->count_place += BABASES_PLACE;
		vector->item = temp;
		vector->item[vector->count_item] = vector->contractor(to_copy);
		if (vector->item[vector->count_item] == NULL) {
			return NULL;
		}
		vector->count_item++;
		for (i = vector->count_item; i < vector->count_place; i++) {
			vector->item[i] = NULL;
		}
	}
	else {
		for (i = 0; i < vector->count_place; i++) {
			if (vector->item[i] == NULL) {
				vector->item[i] = vector->contractor(to_copy);
				if (vector->item[i] == NULL) {
					return NULL;
				}
				vector->count_item++;
				break;
			}
		}
	}
	return vector->item[vector->count_item - 1];
}

void** first_vector(Vector* vector) {
	return vector->item;
}
void** last_vector(Vector* vector) {
	return &vector->item[vector->count_item - 1];
}

int get_item_count(Vector* vector, void* item) {
	void** first_v = first_vector(vector);
	void** last_v = last_vector(vector);
	int count;
	for (count = 0; first_v <= last_v; first_v++) {
		if (*first_v == item)
			return count;
		count++;
	}
	return -1;
}

void free_vector(Vector** vector) {
	int i;
	for (i = 0; i < (*vector)->count_item; i++) {
		(*vector)->destructor((*vector)->item[i]);
	}
	free((*vector)->item);
	free((*vector));
	(*vector) = NULL;
}