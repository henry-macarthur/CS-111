#include "SortedList.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sched.h>

void SortedList_insert(SortedList_t *list, SortedListElement_t *element)
{

    if(list == NULL || element == NULL) //invalid input
        return;

    
    //not sure what the specs are here for if the list is empty, but 
    //either both pointers point to the head node or they are nullptr
    if(list->next == NULL) //either null or head points to itselft
    {
        if(opt_yield & INSERT_YIELD) {
            //fprintf(stderr, "in \n");
            sched_yield();
        }
        list->next = element;
        list->prev = element;
        element->next = list;
        element->prev = list;
        return; //we are done
    }
    //list is not empty, find where we need to insert
    SortedListElement_t* temp = list->next; //this is how we iterate
    //printf("%s \n", temp->key);
    while(temp != list && strcmp(element->key, temp->key) < 0) //while element is less than list item, traverse
    {
        temp = temp->next;
    } //so if we either get to end or find first node bigger, stop

    if(opt_yield & INSERT_YIELD)
    {
        //fprintf(stderr, "in \n");
        sched_yield();
    }
    //insert time
    element->next = temp;
    element->prev = temp->prev;
    temp->prev->next = element;
    temp->prev = element;

    
    //element is now inserted
}

int SortedList_delete( SortedListElement_t *element)
{
    if(element->next->prev != element || element->prev->next != element)
        return 1; //corrupted pointers
    if(opt_yield & DELETE_YIELD)
    {
        //fprintf(stderr, "del \n");
        sched_yield();
    }
    //do i need to use free?
    element->prev->next = element->next;
    element->next->prev = element->prev;

    return 0;
}

SortedListElement_t *SortedList_lookup(SortedList_t *list, const char *key)
{
    SortedListElement_t* tmp = list->next;
    while(tmp != list)
    {
        if(opt_yield & LOOKUP_YIELD)
            sched_yield();
        if(strcmp(tmp->key, key) == 0)//is a match
        {
            return tmp;
        }
        tmp = tmp->next;
    } //if we reach the head again then there is no match, return null

    return NULL;
}

int SortedList_length(SortedList_t *list)
{
    SortedListElement_t* tmp = list->next;
    int counter = 0;
    if(list->next == NULL)
        return 0;
    
    while(tmp != list)
    {
        // fprintf(stderr, "mhkhj \n");
        if(tmp->prev->next != tmp || tmp->next->prev != tmp)
            return -1; //corrupted
        
        if(opt_yield & LOOKUP_YIELD)
            sched_yield();
        counter++;
        tmp = tmp->next;
    }

    return counter;
}

