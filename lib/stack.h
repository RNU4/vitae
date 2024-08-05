#ifndef STACK_H_   /* Include guard */
#define STACK_H_

#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#define _type void*
#define STACK_GROW_AMOUNT 4
typedef struct stack{
    int top;
    int size;
    _type* data;
} stack_t;

stack_t* stack_new(int size){
    stack_t* s = (stack_t*)calloc(sizeof(stack_t),1);
    s->top = 0;
    s->size = size;
    s->data = (_type *)calloc(sizeof(_type),size);
    return s;
}

int grow_stack(stack_t* s,size_t size){
    s->size+=size;
    s->data = (_type*)realloc(s->data,s->size*sizeof(_type));
    if (s->data == NULL){
        printf("ERROR REALLOC FAILED\n");
        return 0;
    }
    return 1;
}

int64_t stack_push(stack_t *s, _type o){
    if (s->top+2 >= s->size){
        grow_stack(s,STACK_GROW_AMOUNT);
    }
    s->data[s->top++]=o;
    return s->top;
}

stack_t* stack_copy(stack_t* s,int start,int end){

    /*Safety checks*/
    if ((start-end)>0 && start>=0 && end>0 && end<s->top){
        return NULL;
    }

    stack_t* tmp = stack_new(end-start);
    for (int i = start; i<end; i++){
        stack_push(tmp,s->data[i]);
    }
    return tmp;
}

int64_t stack_switch_top(stack_t *s){
    if (s->top < 1){
        return -1;
    }
    _type o1 = s->data[s->top-1];
    _type o2 = s->data[s->top-2];
    s->data[s->top-2]=o1;
    s->data[s->top-1]=o2;

    return s->top;
}

int64_t stack_switch(stack_t *s,int pos1,int pos2){
    if (s->top >= pos1 || s->top >= pos2){
        return -1;
    }
    _type o1 = s->data[pos1];
    _type o2 = s->data[pos2];
    s->data[pos2]=o1;
    s->data[pos1]=o2;

    return s->top;
}


int64_t stack_flip(stack_t *s,int pos1,int pos2){

    for (int i = 0; i<s->top/2; i++){
        stack_switch(s,i,(s->top-1)-i);
    }
    return s->top;
}

_type stack_pop(stack_t *s){
    if (s->top-1<0){
        printf("ERROR STACK UNDERFLOW\n");
        exit(2);
    }

    return s->data[--(s->top)];
}

_type stack_peak(stack_t *s){
    return s->data[s->top - 1];
}

_type stack_check(stack_t *s,int i){
    if (i<0 || i >= s->top){
        /*stack over or underflow*/
        printf("Stack over or underflow error: stack top %d got %d\n",s->top,i);
        return NULL;
    }
    return s->data[i];
}

int stack_free(stack_t *s){
    free(s->data);
    s->data = NULL;
    free(s);
    s = NULL;
    return 1;
}
#endif