#include "mergesort.h"

void merging_string(entry** entries, entry** internal_buffer, int sorting_index, int low, int mid, int high) {



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



void merging_int(entry** entries, entry** internal_buffer, int sorting_index, int low, int mid, int high) {



    int l1 = 0;
    int l2 = 0;
    int i = 0;


    for(l1 = low, l2 = mid + 1, i = low; l1 <= mid && l2 <= high; i++) {


        if(atoi(entries[l1]->fields[sorting_index]) <= atoi(entries[l2]->fields[sorting_index])){

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

