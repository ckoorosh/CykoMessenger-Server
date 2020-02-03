/* Cyko_JSON */
/* JSON parser in C :) */

#ifndef CYKO_JSON__H
#define CYKO_JSON__H
#include <stddef.h>

#define ck_JSON_NULL   0
#define ck_JSON_Number 1
#define ck_JSON_String 2
#define ck_JSON_Array  3
#define ck_JSON_Object 4


/* The ckJSON structure: */

typedef struct ckJSON
{
	/* next/prev pointers as links in the structure */
	struct ckJSON *next;
	struct ckJSON *prev;

	/* An array or object add will have a child pointer pointing to a add in the array/object. */
	struct ckJSON *child;

	/* The type of the add, as above. */
	int type;

	/* If the type is 2 or ck_JSON_String */
	char *valuestring;

	/* If the type is 1 or ck_JSON_Number */
	double valuedouble;

	/* The add's name */
	char *string;
} ckJSON;


/******* The functions' prototypes : ********/

// Duplicate strings :)
static unsigned char* DuplicateString_ckJSON(unsigned char* string);
// Create new item
ckJSON *NewItem_ckJSON();

// JSON text outputing
ckJSON * Parse_ckJSON(const char *value);
void ParseArray_ckJSON(ckJSON *, const char *, const char *, int &, int );
void ParseString_ckJSON(ckJSON *json, const char *value, const char *str, int &start, int state);
void ParseObject_ckJSON(ckJSON *json, const char *value, const char *str, int &start, int state);
char *PrintUnformatted_ckJSON(ckJSON *json);
void PrintArray_ckJSON(ckJSON *array, char *print);
void PrintString_ckJSON(ckJSON *string, char *print);
void PrintObject_ckJSON(ckJSON *object, char *print);
void Delete_ckJSON(ckJSON *rmv);	// deleting a JSON root

// Getting items from JSON
int GetArraySize_ckJSON(const ckJSON *array);	// returning the number of items in an array
ckJSON *GetArrayItem_ckJSON(const ckJSON *array, int index); //getting items from arrays
ckJSON *GetObjectItem_ckJSON(const ckJSON * const object, const char * const string);

// Creating JSON and items
ckJSON *CreateNumber_ckJSON(double num);
ckJSON *CreateString_ckJSON(const char *string);
ckJSON *CreateArray_ckJSON();
ckJSON *CreateObject_ckJSON();
void AddItemToArray_ckJSON(ckJSON *array, ckJSON *item);
void AddItemToObject_ckJSON(ckJSON * object, char * string, ckJSON * item);
ckJSON *cJSON_Duplicate(const ckJSON *item);


#endif 
