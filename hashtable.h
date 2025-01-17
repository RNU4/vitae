#ifndef HASH_TABLE_H_   /* Include guard */
#define HASH_TABLE_H_
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <stdbool.h>
#include <stdint.h>


#define TABLE_SIZE 11
#define MAX_NAME 32
//#define hash_table_insert(ht,name ,p) hash_table_insert_(ht,name,(void*)p)
//#define max(X, Y) (((X) > (Y)) ? (X) : (Y))

//#define hash_active 0x0DEADBEEF // the chance that the program will crash is EXTREMELY SMALL
typedef struct hash_element {
    void* value;
    char* name;
    struct hash_element* next;
}hash_element_t;

typedef struct hashtable {
    hash_element_t** hash_table;
    unsigned int size;
} hashtable_t;

unsigned int hash(hashtable_t* ht,char *name){
    int length = strlen(name);
    unsigned int hash_value = 0;
    for (int i=0; i< length; i++){
        hash_value +=name[i]*name[i]*name[i] ^ hash_value;
        //Magic number 5 idk it works and really well
        //hash_value += ((hash_value * name[i]));
    }
    return hash_value % ht->size;
}

void init_hash_table(hashtable_t* ht,int size){

    ht->hash_table = (hash_element_t**)malloc(sizeof(hash_element_t)*size);

    for (int i=0; i<size; i++){
        ht->hash_table[i] = (hash_element_t*)malloc(sizeof(hash_element_t));
        ht->hash_table[i]->value=NULL;
        ht->hash_table[i]->next=NULL;
        ht->hash_table[i]->name = "\0";
    }
    ht->size=size;

}

void print_table(hashtable_t* ht){
    printf("------------------------\n");
    for (int i=0; i<ht->size; i++){
        if (ht->hash_table[i]->name[0] == 0){ //!this is probaly gonna crash someday
            printf("%i\t---\n",i);
        } else{
            printf("%i\t",i);//,ht->hash_table[i]->name,(int*)ht->hash_table[i]->value);

            hash_element_t* tmp = ht->hash_table[i];
            while(tmp->next !=NULL){ //!this is probaly gonna crash someday
                printf(" [\"%s\"] = %d ",tmp->name,(int*)tmp->value);
                tmp = tmp->next;
            }

            printf("\n");
        }
    }
    printf("------------------------\n");
}

bool hash_table_insert(hashtable_t* ht,char *name ,void* p){
    /*Create hash element*/
    hash_element_t* element = (hash_element_t*)malloc(sizeof(hash_element_t));

    element->name = (char*)malloc(sizeof(char)*MAX_NAME);
    strcpy(element->name,name);
    element->value = p;
    element->next = NULL;


    //if (p==NULL) return false;
    unsigned int index = hash(ht,name);
    //printf("\t%s.\t=%u\n",name,index);
    element->next = ht->hash_table[index];
    ht->hash_table[index]=element;

    //ht->hash_table[index]=element;
    return true;

}


bool hash_table_delete(hashtable_t* ht,char *name){

    unsigned int index = hash(ht,name);
    hash_element_t *tmp = ht->hash_table[index];
    hash_element_t *prev = NULL;
    while(tmp !=NULL && strncmp(tmp->name,name,MAX_NAME)!=0){
        prev = tmp;
        tmp = tmp->next;
    }
    if (tmp == NULL) return NULL;
    if (prev == NULL) {
        ht->hash_table[index] = tmp->next;
    }else{
        prev->next = tmp->next;
    }
    printf("freed tmp\n");
    free(tmp);

    return true;

}

void* hash_table_get(hashtable_t* ht,char *name){
    unsigned int index = hash(ht,name);
    hash_element_t *tmp = ht->hash_table[index];
    while(tmp !=NULL && strncmp(tmp->name,name,MAX_NAME)!=0){
        tmp=tmp->next;
    }

    /*so it dont crash by reading a null pointer*/
    return tmp == NULL ? NULL : tmp->value;
}


hash_element_t* hash_table_get_element(hashtable_t* ht,char *name){
    unsigned int index = hash(ht,name);
    hash_element_t *tmp = ht->hash_table[index];
    //printf("NAME {%s}\n",ht->hash_table[index]->name);
    while(tmp !=NULL && strncmp(tmp->name,name,MAX_NAME)!=0){
        tmp=tmp->next;
    }
    //printf("NAME {%s} VALUE %d\n",ht->hash_table[index]->name,tmp->value);
    return tmp;
}
bool hash_table_check(hashtable_t* ht,char *name){
    unsigned int index = hash(ht,name);
    hash_element_t *tmp = ht->hash_table[index];

    while(tmp !=NULL  && strncmp(tmp->name,name,MAX_NAME)!=0){
        tmp=tmp->next;
    }
    if (tmp!=NULL)
        return true; else return false;

}

#endif