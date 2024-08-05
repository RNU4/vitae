#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "./lib/stack.h"
#include "structs.h"
#include "./tokenizer_helper.h"
token_t* create_token(enum token_type type, char *value ,int line_number){
    token_t * t = (token_t*)malloc(sizeof(token_t));
    t->line_number = line_number;
    t->type = type;
    t->value = value;
    return t;
}

int is_keyword(char *str) {
    char *keywords[] = {
    "function", "if", "else", "elseif", "end", "for", 
    "while", "repeat", "until", "return", "local","const", 
    "true", "false", "nil",":",
    "uint","uint8","uint16","uint32","uint64", //uint
    "int8","int16","int","int32","int64", //int 
    "float","float32","float64", //float
    "pointer", /*idk i guess its a void pointer*/
    };

    printf("keyword '%s'\n",str);
    int length = sizeof(keywords) / sizeof(keywords[0]);
    for (int i = 0; i < length; i++) {
        if (strcmp(str, keywords[i]) == 0) {
            return 1;
        }
    }
    return 0;
}
/*count ; as a newline*/
int is_newline(char *ptr) {
    return (*ptr == '\n' || *ptr == ';');
}


bool tokenizer_is_keyword_or_identifier(char *ptr){
    if (isalpha(*ptr) || *ptr == '_' || *ptr == ':') {
            return true;
    } 
    return false;
}

bool tokenizer_handle_keyword_and_identifier(tokenizer_t* tokenizer){
    char *start = tokenizer->ptr;
    while (isalnum(*tokenizer->ptr) || *tokenizer->ptr == '_') {
        tokenizer->ptr++;
    }
    
    if (start == tokenizer->ptr){
        if (*start == ':'){
            tokenizer->ptr++;
        }else{
            return false;
        }
    }

    int length = tokenizer->ptr - start;
    
    char* value = malloc(length + 1);
    strncpy(value, start, length);
    value[length] = '\0';
    int type = is_keyword(value) ? KEYWORD : IDENTIFIER;

    stack_push(tokenizer->tokens,
    (void*)create_token(type,
    value,
    tokenizer->current_line_number
    ));
}

bool tokenizer_is_comment(char *ptr){
    if (*ptr == '-' && *(ptr + 1) == '-') {
        return true;
    } 
    return false;
}

bool tokenizer_is_string(char *ptr){
    return (*ptr == '"' || *ptr == '\'' || *ptr == '`');
}
bool tokenizer_handle_string(tokenizer_t* tokenizer){

    char quote = *tokenizer->ptr;
    tokenizer->ptr++;
    char *start = tokenizer->ptr;
    while (*tokenizer->ptr != quote && *tokenizer->ptr != '\0') {
        tokenizer->ptr++;
    }
    int length = tokenizer->ptr - start;
    char* value = malloc(length + 1);
    strncpy(value, start, length);
    value[length] = '\0';
    stack_push(tokenizer->tokens,(void*)create_token(STRING,value,tokenizer->current_line_number));


    if (*tokenizer->ptr == quote) {
        tokenizer->ptr++;
    }
}



bool tokenizer_handle_comment(char *ptr){
    ptr += 2; // already checked first 2 chars so skip them
    while (*ptr != '\n' && *ptr != '\0') {
        ptr++;
    }
}

bool tokenizer_handle_numbers(tokenizer_t* tokenizer){
        char *start = tokenizer->ptr;

        while (isdigit(*tokenizer->ptr) || *tokenizer->ptr=='.' ) {
            tokenizer->ptr++;
        }
        int length = tokenizer->ptr - start;
        char* value = (char*)malloc(length + 1);
        strncpy(value, start, length);
        value[length] = '\0';
        stack_push(tokenizer->tokens,(void*)create_token(NUMBER,value,tokenizer->current_line_number));
}

bool tokenizer_is_operator(char *ptr){
    if (strchr("+-*/%^#=<>;,.(){}[]$", *ptr) != NULL) {
        return true;
    }
    return false;
}

bool tokenizer_handle_operators(tokenizer_t* tokenizer){
    char* value = (char*)malloc(2);
    value[0] = *tokenizer->ptr;
    value[1] = '\0';
    stack_push(tokenizer->tokens,(void*)create_token(OPERATOR,value,tokenizer->current_line_number));
    tokenizer->ptr++;
}

// *(*(x.y)*2 + 1)
void tokenizer_last_pass(tokenizer_t* tokenizer){
    //check for functions or keywords acting like functions
    token_t* last_token = NULL;
    for (int i = 0 ; i < tokenizer->tokens->top; i++){
        token_t* token = (token_t*)stack_check(tokenizer->tokens,i);
        assert(token != NULL);

        if ((token->type == IDENTIFIER || token->type == KEYWORD) && token_get_next_first_char(tokenizer,i) == '('){
            if (last_token!=NULL){
                if (strcmp(last_token->value,"function") != 0)
                token->type = FUNCTION;
            }else{
            token->type = FUNCTION;

            }
        }

        /*convert pointer symbol '*' into '$' for easier handling*/
        if (token->type == OPERATOR && token->value[0] == '*' && (last_token == NULL || last_token->type != IDENTIFIER || last_token->type == OPERATOR) 
        && (last_token == NULL || (last_token!=NULL && last_token->value[0]!=')'))){
            token->value[0] = '$';
        }

        last_token = token;
    }
}

stack_t *tokenize(tokenizer_t* tokenizer,char *code) {
    tokenizer->ptr = code;
    tokenizer->current_line_number = 1; // first linenumber is 1
    tokenizer->tokens = stack_new(32);

    while (*(tokenizer->ptr) != '\0') {

        if (is_newline(tokenizer->ptr)){
            stack_push(tokenizer->tokens,(void*)create_token(NEWLINE,"NL",tokenizer->current_line_number));
            tokenizer->current_line_number++;
            tokenizer->ptr++;
            continue;
        }

        if (tokenizer_is_comment(tokenizer->ptr)){
            tokenizer_handle_comment(tokenizer->ptr); // remove the comment
            continue;
        }

        if (isdigit(*tokenizer->ptr)) {
            tokenizer_handle_numbers(tokenizer);
            continue;
        }

        if (tokenizer_is_operator(tokenizer->ptr)) {
            tokenizer_handle_operators(tokenizer);
            continue;
        }

        if (tokenizer_is_keyword_or_identifier(tokenizer->ptr)) {
            tokenizer_handle_keyword_and_identifier(tokenizer);
            continue;
        }

        if (tokenizer_is_string(tokenizer->ptr)){
            tokenizer_handle_string(tokenizer);
            continue;
        }

        tokenizer->ptr++;
    }

    tokenizer_last_pass(tokenizer);
    /*end of file*/
    stack_push(tokenizer->tokens,(void*)create_token(END_OF_FILE,"EOF",tokenizer->current_line_number));


    return tokenizer->tokens;
}