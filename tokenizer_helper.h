#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "./lib/stack.h"
#include "structs.h"

#ifndef TOKENIZER_HELPER_H_   /* Include guard */
#define TOKENIZER_HELPER_H_

int token_get_next_first_char(tokenizer_t* tokenizer, int i){
    if (i+1 < tokenizer->tokens->top){
        return (((token_t*)tokenizer->tokens->data[i+1])->value[0]);
    }else{
        return -1;
    }
}

token_t* next_token(tokenizer_t* tokenizer, int i){
    if (i+1 < tokenizer->tokens->top ){
        return ((token_t*)tokenizer->tokens->data[i+1]);
    } else{
        return NULL;
    }
}

token_t* check_token(stack_t* stack, int i){
    if (i < stack->top && i>=0){
        return ((token_t*)stack->data[i]);
    } else{
        return NULL;
    }
}

#endif