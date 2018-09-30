#include "simpleCSVsorter.h"


int merge_numeric = 1;


// sorting function that calls merging_int or merging_string depending on data type
void sort(entry** entries, entry** internal_buffer,int sorting_index ,int low, int high) {

    int mid;

    if (low < high) {
        mid = (low + high) / 2;
        sort(entries, internal_buffer, sorting_index,low, mid);
        sort(entries, internal_buffer, sorting_index,mid + 1, high);
        if(merge_numeric == 1){
            merging_int(entries, internal_buffer, sorting_index, low, mid, high);
        }
        else {
            merging_string(entries, internal_buffer, sorting_index, low, mid, high);
        }
    } else {
        return;
    }
}
//


// function to count the number of fields needed
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
//

// function to count the number of lines in the opened file in memory
int countlines (FILE *fin)
{
    int  nlines  = 0;
    char line[BUFFSIZE];

    while(fgets(line, BUFFSIZE, fin) != NULL) {
        nlines++;
    }

    return nlines;
}
//

// add fields from a line in the opened file to an entry element in a the entry array
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

                        size += strlen(field) + 1;
                        array_entry->fields[i] = realloc(array_entry->fields[i], size);
                        sprintf(array_entry->fields[i] + index, ",%s", field);
                        index += strlen(field) + 1;

                }



            }
        }

        i++;



    }

    free(temp);


    return 0;

}

//


// build the array of entries through opening a file in memory and counting the number of entries needed to build the array
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

//

int main(int argc, char* argv[]) {

// check for the number of arguments
    if( argc != 3 ){
        fprintf(stderr, "INVALID NUMBER OF INPUTS\n");
        return 0;
    }
//

// check for flag -c in input
    if(strcmp(argv[1], "-c") != 0){
        fprintf(stderr, "INVALID COMMAND\n");
        return 0;
    }
//


// get data piped from stdin and put it in memory. At the end there would be a memory buffer having all the contents
// passed in from stdin. using this buffer, will open a file in memory using fmemopen
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
//

// call the function that builds the array
    int entries_count = -1;
    int fields_count = -1;
    int i = 0;
    int j = 0;

    entry** entries = load_array(&entries_count, &fields_count, file_in_memory);
//

// determine if column to be sorted exists in fields
    for(i = 0; i < fields_count; i++){
        if(strcmp(entries[0]->fields[i], argv[2]) == 0){
            sorting_index = i;
            break;
        }
    }


    if(sorting_index == -1){
        fprintf(stderr, "INVALID COLUMN NAME\n");
        goto NAME_NOT_FOUND;
    }
//

//determine the type of entries to be sorted (numeric vs strings)
    int k ;

    for(k=1; k < entries_count -1; k++){

        if(strcmp(entries[k]->fields[sorting_index], "") == 0){
            continue;
        }


        int entry_length =  (int) strlen(entries[k]->fields[sorting_index]);

        for(i = 0; i < entry_length; i++){


            if(!(isdigit(entries[k]->fields[sorting_index][i]))){
                merge_numeric = 0;
                break;
            }
        }
    }

    i = 0;


//

// load the internal buffer needed by mergesort
    entry** internal_buffer = (entry**) malloc(sizeof(entry*) * entries_count);

    while(i < entries_count){

        internal_buffer[i] = (entry*) malloc(sizeof(entry));
        i++;
    }
//

// call sort on the array
        sort(entries, internal_buffer, sorting_index, 1, entries_count - 2);
//


// print the sorted array
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
//

// free memory
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
//

    return 0;
}