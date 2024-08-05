#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "structs.h"
#include "./lib/stack.h"
#include "tokenizer.h"
#include "./shunting_yard.h"
#include "assembler.h"
stack_t *tokenize(tokenizer_t* tokenizer,char *code);

char* type_to_string[] = {
    "KEYWORD",
    "IDENTIFIER",
    "NUMBER",
    "FUNCTION",
    "STRING",
    "OPERATOR",
    "COMMENT",
    "NEWLINE",
    "EOF",
}; 
/*
line 1 type IDENTIFIER : [function] 
line 1 type FUNCTION : [add] 
line 1 type OPERATOR : [(]
line 1 type IDENTIFIER : [int]
line 1 type IDENTIFIER : [a]
line 1 type OPERATOR : [,]
line 1 type IDENTIFIER : [int]
line 1 type IDENTIFIER : [b]
line 1 type OPERATOR : [)]
line 1 type KEYWORD : [int]
*/

int main(){
    //char lualike_code[] = "function add(int* arg1,int arg2,int arg4) int:";//"function add(int a, int b) int\n return a + b \nend\n";

    char lualike_code[] = "max(2.3,3)\n;";
    //har lualike_code[] = "(*(x.y*2 + 1))";
    tokenizer_t* tokenizer = calloc(sizeof(tokenizer_t),1);
    tokenize(tokenizer,lualike_code);
    for (int i = 0; i<tokenizer->tokens->top; i++){
        printf("line %d type %s : [%s] \n",((token_t*)stack_check(tokenizer->tokens,i))->line_number,
        type_to_string[((token_t*)stack_check(tokenizer->tokens,i))->type],
        ((token_t*)stack_check(tokenizer->tokens,i))->value);
    }


    printf("\nassembler:\n");

    assemble(tokenizer);

}