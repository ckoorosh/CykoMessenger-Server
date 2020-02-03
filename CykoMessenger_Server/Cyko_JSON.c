/* Cyko_JSON */
/* JSON parser in C :) */

#include <string.h>
#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <limits.h>
#include <ctype.h>

#include "Cyko_JSON.h"


// Duplicate strings :)
static unsigned char* DuplicateString_ckJSON(unsigned char* string)
{
	size_t length = 0;
	unsigned char *copy = NULL;

	if (string == NULL)	return NULL;

	length = strlen((const char*)string) + 1; //+1 as sizeof("")
	copy = (unsigned char*)malloc(length);
	if (copy == NULL)	return NULL;
	memcpy(copy, string, length);

	return copy;
}

// Create new item
ckJSON *NewItem_ckJSON()
{
	ckJSON* node = (ckJSON*)malloc(sizeof(ckJSON));
	if (node)	memset(node, '\0', sizeof(ckJSON));
	return node;
}

// Delete a cJSON structure
void Delete_ckJSON(ckJSON *rmv)
{
	ckJSON *next = NULL;
	// searching through the list
	while (rmv != NULL)
	{
		next = rmv->next;
		// if the item the child pointer in the ckJSON struct (type should be !=0 which is the NULL)
		if (!(rmv->type) && (rmv->child != NULL))	Delete_ckJSON(rmv->child);
		// if the item is the valuestring in the ckJSON struct
		if (!(rmv->type) && (rmv->valuestring != NULL))	free(rmv->valuestring);
		// if the item is the string in the ckJSON struct
		if (!(rmv->type) && (rmv->string != NULL))	free(rmv->string);
		free(rmv);
		// go to next node
		rmv = next;
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////////

int GetArraySize_ckJSON(const ckJSON *array)
{
	ckJSON *child = NULL;
	int size = 0;

	if (array == NULL)return 0;

	child = array->child;

	while (child != NULL)
	{
		size++;
		child = child->next;
	}

	return size;
}

ckJSON* GetArrayItem_ckJSON (const ckJSON *array, int index)
{
	if (index < 0)return NULL;

	ckJSON *curchild = NULL;

	if (array == NULL)return NULL;

	curchild = array->child;
	while ((curchild != NULL) && (index > 0))
	{
		index--;
		curchild = curchild->next;
	}

	return curchild;
}

int case_insensitive_strcmp(const unsigned char *string1, const unsigned char *string2)
{
	if ((string1 == NULL) || (string2 == NULL))
	{
		return 1;
	}

	if (string1 == string2)
	{
		return 0;
	}

	for (; tolower(*string1) == tolower(*string2); (void)string1++, string2++)
	{
		if (*string1 == '\0')
		{
			return 0;
		}
	}

	return tolower(*string1) - tolower(*string2);
}

ckJSON * GetObjectItem_ckJSON(ckJSON *  object, const char * const string)
{	
	ckJSON *curelement = object->child;

	while ((curelement != NULL) )
	{
		if((strcmp(string, curelement->string) == 0))return curelement;
		curelement = curelement->next;
	}
	
	return NULL;
}

///////////////////////////////////////////////////////////////////////////////////////////////////

/* Add item to array/object. */
void AddItemToArray_ckJSON(ckJSON *array, ckJSON *item)
{
	ckJSON *child = NULL;

	if ((item == NULL) || (array == NULL))return;

	child = array->child;

	if (child == NULL)
	{
		/* list is empty, start new one */
		array->child = item;
	}
	else
	{
		/* append to the end */
		while (child->next)
		{
			child = child->next;
		}
		child->next = item;
		item->prev = child;
	}

	return;
}

void AddItemToObject_ckJSON(ckJSON * object, char *string, ckJSON * item)
{
	if (!item) return;

	if (item->string)free(item->string);
	item->string = (char*)DuplicateString_ckJSON((unsigned char*)string);

	AddItemToArray_ckJSON(object, item);
}

ckJSON * CreateNumber_ckJSON(double num)
{
	ckJSON *item = NewItem_ckJSON();
	if (item)
	{
		item->type = ck_JSON_Number;
		item->valuedouble = num;
	}

	return item;
}

ckJSON * CreateString_ckJSON(const char *string)
{
	ckJSON *item = NewItem_ckJSON();
	if (item)
	{
		item->type = ck_JSON_String;
		item->valuestring = (char*)DuplicateString_ckJSON((unsigned char*)string);
		if (!item->valuestring)
		{
			Delete_ckJSON(item);
			return NULL;
		}
	}

	return item;
}

ckJSON * CreateArray_ckJSON()
{
	ckJSON *item = NewItem_ckJSON();
	if (item)
	{
		item->type = ck_JSON_Array;
	}

	return item;
}

ckJSON * CreateObject_ckJSON()
{
	ckJSON *item = NewItem_ckJSON();
	if (item)
	{
		item->type = ck_JSON_Object;
	}

	return item;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////

/* Duplication */
ckJSON * Duplicate_ckJSON(const ckJSON *item)
{
	ckJSON *newitem = NULL;
	ckJSON *child = NULL;
	ckJSON *next = NULL;
	ckJSON *newchild = NULL;

	/* Bail on bad ptr */
	if (!item)
	{
		if (newitem != NULL)
		{
			Delete_ckJSON(newitem);
		}

		return NULL;
	}
	/* Create new item */
	newitem = NewItem_ckJSON();
	if (!newitem)
	{
		if (newitem != NULL)
		{
			Delete_ckJSON(newitem);
		}

		return NULL;
	}
	/* Copy over all vars */
	newitem->type = item->type ;
	newitem->valuedouble = item->valuedouble;
	if (item->valuestring)
	{
		newitem->valuestring = (char*)DuplicateString_ckJSON((unsigned char*)item->valuestring);
		if (!newitem->valuestring)
		{
			if (newitem != NULL)
			{
				Delete_ckJSON(newitem);
			}

			return NULL;
		}
	}
	if (item->string)
	{
		newitem->string = (item->type) ? item->string : (char*)DuplicateString_ckJSON((unsigned char*)item->string);
		if (!newitem->string)
		{
			if (newitem != NULL)
			{
				Delete_ckJSON(newitem);
			}

			return NULL;

		}
	}
	
	return newitem;

}

/* Render a cJSON item/entity/structure to text. */

char *PrintUnformatted_ckJSON(ckJSON *json)
{
	char *print = (char *)calloc(100,1000);
	ckJSON *temp;

	strcpy(print, "{");
	if (json->child == NULL) {

		strcat(print, "}");
		return print;
	}
	
	temp = json->child;
	for (; temp != NULL;) {

		if (temp->type != 4)strcat(print, "\"");
		if(temp->type!=4)strcat(print, temp->string);
		if (temp->type != 4)strcat(print, "\":");
		switch (temp->type)
		{
		case 2: PrintString_ckJSON(temp, print); break;
		case 3: PrintArray_ckJSON(temp, print); break;
		case 4: PrintObject_ckJSON(temp, print); break;
		default:break;
		}
		temp = temp->next;

		if (temp != NULL)strcat(print, ",");
	}


	strcat(print, "}");
	return print;

}

void PrintArray_ckJSON(ckJSON *array, char *print)
{
	ckJSON *temp;
	strcat(print, "[");

	if (array->child == NULL) {
		strcat(print, "]");
		return;
	}
	temp = array->child;
	for (; temp != NULL;) {
		switch (temp->type)
		{
		case 2: PrintString_ckJSON(temp, print); break;
		case 3: PrintArray_ckJSON(temp, print); break;
		case 4: PrintObject_ckJSON(temp, print); break;
		default:break;
		}
		temp = temp->next;

		if (temp != NULL)strcat(print, ",");
	} 

	strcat(print, "]");
	return;
}

void PrintString_ckJSON(ckJSON *string, char *print)
{
	char temp[2047];
	sprintf(temp, "\"%s\"", string->valuestring);
	strcat(print, temp);
}

void PrintObject_ckJSON(ckJSON *object, char *print)
{
	ckJSON *temp;
	strcat(print, "{");
	if (object->child == NULL) {

		strcat(print, "}");
		return;
	}
	temp = object->child;
	for (; temp != NULL;) {

		strcat(print, "\"");
		strcat(print, temp->string);
		strcat(print, "\":");
		switch (temp->type)
		{
		case 2: PrintString_ckJSON(temp, print); break;
		case 3: PrintArray_ckJSON(temp, print); break;
		case 4: PrintObject_ckJSON(temp, print); break;
		default:break;
		}
		temp = temp->next;

		if (temp != NULL)strcat(print, ",");
	} 

	strcat(print, "}");
	return;
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////

/************************* Parse JSON **********************************/

ckJSON * Parse_ckJSON(const char *value) {
	
	if (value == NULL)return NULL;

	char *str;
	ckJSON* json=CreateObject_ckJSON();
	int index=0,temp=0;
	while (1) {
		str = (char*)malloc(1000);
		for (index; value[index] && value[index] != '\"'; index++);
		if (value[index] == '\0')return json;
		for (temp = index + 1; value[temp] && value[temp] != '\"'; temp++) str[temp - (index + 1)] = value[temp];
		str[temp-index-1] = '\0';
		index = temp + 2;
		temp = 0;
		switch (value[index])
		{
		case '\"':ParseString_ckJSON(json, value, str, index,0); break;
		case '{':ParseObject_ckJSON(json, value, str, index,0); break;
		case '[':ParseArray_ckJSON(json, value, str, index,0); break;
		default:
			break;
		}
		free(str);
	}
	return json;
}

/* Build an array from input text. */
void ParseArray_ckJSON(ckJSON *json, const char *value, const char *str, int &start,int state) {
	
	int index,flag=0;
	ckJSON* item = CreateArray_ckJSON();
	start++;
	while (1)
	{
		switch (value[start])
		{
		case '\"':ParseString_ckJSON(item, value, "", start, 1); break;
		case '{':ParseObject_ckJSON(item, value, "", start, 1); break;
		case '[':ParseArray_ckJSON(item, value, "", start, 1); break;
		case ',': break;
		case ']':flag = 1; break;
		default:
			break;
		}
		if (flag)break;
		start++;
	}

	if (state)AddItemToArray_ckJSON(json, item);
	else AddItemToObject_ckJSON(json, (char*)str, item);

}

/* Parse the input text into an unescaped cinput, and populate item. */
void ParseString_ckJSON(ckJSON *json, const char *value, const char *str, int &start,int state) {
	char* temp = (char*)malloc(1000);
	int i;
	for (i = start+1; value[i] != '\"'; i++)
		temp[i - start-1] = value[i];
	temp[i - start - 1] = '\0';
	start = i;
	ckJSON *item=CreateString_ckJSON(temp);
	if (state)AddItemToArray_ckJSON(json, item);
	else AddItemToObject_ckJSON(json, (char*)str, item);
	start++;
}

/* Build an object from the text. */
void ParseObject_ckJSON(ckJSON *json, const char *value, const char *str, int &start,int state) {
	
	ckJSON *item;
	int index = 0;
	char *temp;
	int flag = 0;
	item = CreateObject_ckJSON();

	while (1)
	{
		temp = (char*)malloc(1000);
		while (value[start] != '\"') {
			if (value[start] == '}') { flag = 1; break; }
			start++;
			if (value[start] == '}') { flag = 1; break; }
		}
		if (flag)break;
		start++;
		index = 0;
		for (index = start; value[index] && value[index] != '\"'; index++) temp[index - (start)] = value[index];
		temp[index-start] = '\0';
		start = index + 2;
		index = 0;

		if (value[start]=='\"') ParseString_ckJSON(item, value, temp, start, 0);
		free(temp);
	}

	if (state)AddItemToArray_ckJSON(json, item);
	else AddItemToObject_ckJSON(json, (char*)str, item);

}


