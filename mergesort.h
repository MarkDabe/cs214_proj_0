#ifndef _MERGESORT_H
#define _MERGESORT_H



#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include "mergesort.h"

#define BUFFSIZE 1024



typedef struct entry{

    char** fields;
    size_t length;

}entry;

void merging_string(entry** entries, entry** internal_buffer, int sorting_index, int low, int mid, int high);
void merging_int(entry** entries, entry** internal_buffer, int sorting_index, int low, int mid, int high);
int sanitize_content(char *token);
char* clean_string(char* string);
int removeSubstring(char *s,const char *toremove);
char* trimSpaces(char * str);

#endif
