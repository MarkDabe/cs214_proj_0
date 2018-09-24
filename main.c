#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define BUFFSIZE 1024


typedef struct entry{

    char** fields;
    size_t length;

}entry;

void trimTrailing(char * str)
{
    int index, i;

    index = -1;

    i = 0;
    while(str[i] != '\0')
    {
        if(str[i] != ' ')
        {
            index= i;
        }

        i++;
    }

    str[index + 1] = '\0';
}

void trimTrailingQoute(char * str)
{
    size_t length = strlen(str);

    int i = (int) length - 2;

    while(str[i] == ' ')
    {
        i--;
    }

    str[i + 1] = '"';
    str[i + 2] = '\0';
}


int removeSubstring(char *s,const char *toremove)
{
    while( s=strstr(s, toremove) )
    {
        memmove(s, s + strlen(toremove), 1 + strlen(s + strlen(toremove)));
    }

    return 0;
}

int sanitize_content(char *token){

    removeSubstring(token, "\n");
    removeSubstring(token, "\r");

    return 0;

}


void merging(entry** entries, entry** internal_buffer, int sorting_index, int low, int mid, int high) {



    int l1 = 0;
    int l2 = 0;
    int i = 0;


    for(l1 = low, l2 = mid + 1, i = low; l1 <= mid && l2 <= high; i++) {


        if(strcmp(entries[l1]->fields[sorting_index], entries[l2]->fields[sorting_index]) <= 0){

            memcpy(internal_buffer[i], entries[l1], sizeof(entries[l1]));
            l1++;
        }
        else{
            memcpy(internal_buffer[i], entries[l2], sizeof(entries[l2]));
            l2++;
        }
    }

    while(l1 <= mid) {
        memcpy(internal_buffer[i], entries[l1], sizeof(entries[l1]));
        i++;
        l1++;
    }

    while(l2 <= high){
        memcpy(internal_buffer[i], entries[l2], sizeof(entries[l2]));
        i++;
        l2++;
    }

    for(i = low; i <= high; i++) {
        memcpy(entries[i], internal_buffer[i], sizeof(internal_buffer[i]));
    }

}

void sort(entry** entries, entry** internal_buffer,int sorting_index ,int low, int high) {

    int mid;

    if (low < high) {
        mid = (low + high) / 2;
        sort(entries, internal_buffer, sorting_index,low, mid);
        sort(entries, internal_buffer, sorting_index,mid + 1, high);
        merging(entries, internal_buffer, sorting_index,low, mid, high);
    } else {
        return;
    }
}

    int countfields(char* line){
    int count = 0;
    int i =0;

    for(i = 0; line[i] != '\0'; ++i)
    {
        if(line[i] == ',')
            ++count;
    }

    return count + 1;

}


int countlines (FILE *fin)
{
    int  nlines  = 0;
    char line[BUFFSIZE];

    while(fgets(line, BUFFSIZE, fin) != NULL) {
        nlines++;
    }

    return nlines;
}


int add_fields(entry* array_entry,int* fields_count,  char* line){

    int i = 0;
    char* field = NULL;


    if(*fields_count == -1){
        *fields_count = countfields(line);
    }



    array_entry->length = strlen(line) + 1;
    char* temp = (char*) malloc(array_entry->length);
    strncpy(temp,line,array_entry->length);
    array_entry->fields = (char**) malloc((*fields_count) * sizeof(char*));

    while ((field = strsep(&temp, ",")) != NULL && i < 28) {

        sanitize_content(field);
        trimTrailing(field);
        /* note the trailing field will contain newline. */
        if(strcmp(field, "") == 0){
            array_entry->fields[i] = malloc(sizeof("") + 1);
            strncpy(array_entry->fields[i] ,"\0", sizeof("") + 1);
        }
        else {

            array_entry->fields[i] = (char *) malloc((strlen(field) + 1) * sizeof(char));
            strncpy(array_entry->fields[i], field, strlen(field) + 1);
            if(array_entry->fields[i][0] == '"'){

                size_t index = strlen(field);
                size_t size = strlen(field) + 1;

                while(field[strlen(field)-1] != '"') {

                    field = strsep(&temp, ",");
                    if(field[strlen(field)-1] == '"'){
                        trimTrailingQoute(field);
                    }
                    size += strlen(field);
                    array_entry->fields[i] = realloc(array_entry->fields[i], size);
                    memcpy(array_entry->fields[i] + index, field, strlen(field) + 1);
                    index += strlen(field);
                }



            }
        }

        i++;



    }

    free(temp);


    return 0;

}

entry** load_array(int* entries_count, int* fields_count, char* file_in_memory){

    int i = 0;

    FILE* fptr = fmemopen(file_in_memory, strlen(file_in_memory) + 1 , "r");

    if(fptr == NULL){
        perror(fptr);
        return NULL;
    }


    *entries_count = countlines(fptr);

    rewind(fptr);

    entry** buffer = (entry **) malloc(sizeof(entry*) * (*entries_count));


    char line_buffer[2048] ={0};

    while (fgets(line_buffer, sizeof(line_buffer), fptr) != NULL) {

        buffer[i] = (entry*) malloc(sizeof(entry));

        add_fields(buffer[i], fields_count,line_buffer);


        i++;


    }



    fclose(fptr);



    return buffer;
}

int main(int argc, char* argv[]) {


    if( argc != 3 ){
        fprintf(stderr, "INVALID NUMBER OF INPUTS\n");
        return 0;
    }


    if(strcmp(argv[1], "-c") != 0){
        fprintf(stderr, "INVALID COMMAND\n");
        return 0;
    }



    int coutner = 0;
    int file_in_memory_size = 100;
    int ptr_switch_to = 2;

    char* file_in_memory = NULL;
    char* file_in_memory1 = (char*) malloc((size_t ) file_in_memory_size * sizeof(char));
    char* file_in_memory2 = NULL;


    int sorting_index = -1;


    char next;
    while((next=getchar())!=EOF){
        if(coutner >= file_in_memory_size){
            if(ptr_switch_to == 1){
                file_in_memory_size *= 2;
                file_in_memory1 = realloc(file_in_memory2, file_in_memory_size * sizeof(char));
                ptr_switch_to = 2;

            } else if(ptr_switch_to == 2){
                file_in_memory_size *= 2;
                file_in_memory2 = realloc(file_in_memory1, file_in_memory_size * sizeof(char));
                ptr_switch_to = 1;
            }

        }

        if(ptr_switch_to == 1){
            file_in_memory2[coutner] = next;
        }else{
            file_in_memory1[coutner] = next;
        }

        coutner++;
    }

    if(ptr_switch_to == 1) {

        file_in_memory = file_in_memory2;
    }else{

        file_in_memory = file_in_memory1;
    }


    int entries_count = -1;
    int fields_count = -1;
    int i = 0;
    int j = 0;

    entry** entries = load_array(&entries_count, &fields_count, file_in_memory);

    for(i = 0; i < fields_count; i++){
        if(strcmp(entries[0]->fields[i], argv[2]) == 0){
            sorting_index = i;
            break;
        }
    }

    i = 0 ;

    if(sorting_index == -1){
        fprintf(stderr, "INVALID COLUMN NAME\n");
        goto NAME_NOT_FOUND;
    }


    entry** internal_buffer = (entry**) malloc(sizeof(entry*) * entries_count);

    while(i < entries_count){

        internal_buffer[i] = (entry*) malloc(sizeof(entry));
        i++;
    }



        sort(entries, internal_buffer, sorting_index, 1, entries_count - 2);




    for(i = 0; i < entries_count - 1; i++) {

        for(j= 0; j < fields_count ; j++){

            printf("%s", entries[i]->fields[j]);
            if(j == fields_count - 1){
                break;
            }
            printf(",");
        }

        printf("\n");

    }


   for(i = 0; i < entries_count -1; i++){
        free(internal_buffer[i]);
    }

    free(internal_buffer);


   NAME_NOT_FOUND:   for(i = 0; i < entries_count - 2; i++){

        for(j= 0; j < fields_count - 1; j++){

            free(entries[i]->fields[j]);
        }

        free(entries[i]->fields);

        free(entries[i]);
    }

    free(entries);


    free(file_in_memory);


    return 0;
}