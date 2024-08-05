#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "./lib/stack.h"
#include "structs.h"
#include "tokenizer_helper.h"


bool assembler_is_function_definition(token_t* token){
    if (token->type == KEYWORD && strcmp(token->value,"function") == 0){
        return true;
    } 
    return false;
}

/*check if the type/block is defined*/
bool assembler_is_type_or_block(assembler_t* assembler,char* str){

    if (hash_table_check(&assembler->blocks,str) || hash_table_check(&assembler->types,str)){
        return true;
    }
    return false;
}

int assembler_count_stars(assembler_t* assembler){
    token_t* token;
    int stars = -1;

    do{
        token = check_token(assembler->tokenizer->tokens,assembler->current_token++);
        assert(token != NULL);
        stars++;
    } while(token->value[0]=='$');
    assembler->current_token--;
    return stars;
}

value_t function_definition_get_argument_value_at(function_t* function, int i){
    value_t* v = (value_t*)stack_check(function->argument_list,i);
    if (v == NULL){
        return (value_t){.type_name = "void",.depth = 0};
    }
    return *v;

}

value_t assembler_handle_function_definition_return(assembler_t* assembler){

    token_t* token;
    value_t value = {
        .type_name = "void",
        .depth = 0,
    };

    token = check_token(assembler->tokenizer->tokens,++assembler->current_token);
    assert(token != NULL);
    /*check type*/
    if (assembler_is_type_or_block(assembler,token->value)){
        value.type_name = token->value;
    }else{
        printf("Error expected type or block as return at line %d, got '%s'",token->line_number,token->value);
        exit(2);
    }

    token = check_token(assembler->tokenizer->tokens,++assembler->current_token);
    assert(token != NULL);

    if (token->value[0] == '*'){
        value.depth  = assembler_count_stars(assembler);
    }


    token = check_token(assembler->tokenizer->tokens,assembler->current_token);
    assert(token != NULL);
    if (token->value[0] != ':'){
        printf("Error expected ':' got %s",token->value);
    }

    return value;

}

function_t* assembler_handle_function_definition(assembler_t* assembler){
    /*current token is function*/
    /*function example(int x,int y) int*/
    token_t* token;

    function_t* function_definition = function_create();
    //function_definition->arguments
    /*get name of function*/
    token = check_token(assembler->tokenizer->tokens,++assembler->current_token);
    assert(token != NULL);
    if (token->type == IDENTIFIER){
        /* //TODO check if function already is defined, but not its body and compare to make sure the function definition is the same*/
        function_definition->name = token->value;
    }else{
        printf("error at line %d expected function name got '%s'\n",token->line_number,token->value);
        exit(2);
    }

    /* is '('*/
    token = check_token(assembler->tokenizer->tokens,++assembler->current_token);
    assert(token != NULL);

    if ( token->value[0] == '(' ){
        
        while(true){            

            char* variable_name = NULL;

            value_t value = {
                .type_name = "uint",
                .depth = 0,
            };



            token = check_token(assembler->tokenizer->tokens,++assembler->current_token);
            assert(token != NULL);
            
            /*check if there is no arguments*/
            if (token->type==OPERATOR && token->value[0] == ')'){ 
                break;
            }

            /*check type*/
            if (assembler_is_type_or_block(assembler,token->value)){
                value.type_name = token->value;
            }else{
                printf("Error expected type or block for argument at line %d, got '%s'",token->line_number,token->value);
                exit(2);
            }


            /*pointer? or name*/
            token = check_token(assembler->tokenizer->tokens,++assembler->current_token);
            assert(token != NULL);

            /*is type pointer*/
            if (token->value[0] == '$'){ 
                value.depth = assembler_count_stars(assembler);
                token = check_token(assembler->tokenizer->tokens,assembler->current_token);
                assert(token != NULL);
            }

            /*we should be at name now*/
            if (token->type==IDENTIFIER){
                variable_name = token->value;
                assembler->current_token++;
            }else{
                /*Error expected argument name*/
                printf("Error expected argument name at line %d, got '%s'",token->line_number,token->value);
                exit(2);
            }

            /*insert argument into the function definition hashtable*/
            variable_t* var = (variable_t*)calloc(sizeof(variable_t),1);
            assert(variable_name != NULL);
            var->name = variable_name;
            var->value = value;
            /*check if argument already is defined */
            if (hash_table_check(&function_definition->arguments,variable_name)==true){
                printf("Error Repeated argument name at line %d, got '%s'",token->line_number,token->value);
                exit(2);
            }
            hash_table_insert(&function_definition->arguments,variable_name,(void*)var);



            /*check that we are indeed at a ',' or if finished ')'*/
            token = check_token(assembler->tokenizer->tokens,assembler->current_token);
            assert(token != NULL);

            if (token->value[0] == ','){
                continue;
            }else if ( token->value[0] == ')' ){
                break;
            }else{
                /*Error expected ',' or ')' */
                printf("Error expected ',' or ')' at line %d, got '%s'",token->line_number,token->value );
                exit(2);
            }
        }

    }else{
        printf("error at line %d expected '(' got '%s'\n",token->line_number,token->value);
        exit(2);
    }


    /*//TODO get return type*/
    function_definition->_return = assembler_handle_function_definition_return(assembler);

    printf("Function %s return %s %d\n",function_definition->name,function_definition->_return.type_name,function_definition->_return.depth);

    return function_definition;


}

bool assembler_is_function(assembler_t* assembler, token_t* token){
    if (token->type == FUNCTION){
        if (hash_table_check(&assembler->functions,token->value) == false){
            /*function refrenced but is not declared*/
            printf("error at line %d function %s has not been declared\n",token->line_number,token->value);
            exit(2);
        }
        return true;
    } 
    return false;
}

bool assembler_is_expression(assembler_t* assembler){
 //   if (token->type == IDENTIFIER){
    //    return true;
   // }
    return false;
}

bool assembler_is_newline(token_t* token){
    if (token->type == NEWLINE || token->type == EOF){
        return true;
    }
    return false;
}

bool assembler_is_assignment(token_t* token){
    /*the token could be '==' so make sure its only '='*/
    if (token->type == OPERATOR && strcmp(token->value,"=") == 0){
        return true;
    }
    return false;
}

bool populate_types_hashtable(assembler_t* assembler){
    hash_table_insert(&assembler->types,"int",(void*)type_create("int",type_int,4));
    hash_table_insert(&assembler->types,"int8",(void*)type_create("int8",type_int,1));
    hash_table_insert(&assembler->types,"int16",(void*)type_create("int16",type_int,2));
    hash_table_insert(&assembler->types,"int32",(void*)type_create("int32",type_int,4));
    hash_table_insert(&assembler->types,"int64",(void*)type_create("int64",type_int,8));

    hash_table_insert(&assembler->types,"uint",(void*)type_create("uint",type_uint,4));
    hash_table_insert(&assembler->types,"uint8",(void*)type_create("uint8",type_uint,1));
    hash_table_insert(&assembler->types,"uint16",(void*)type_create("uint16",type_uint,2));
    hash_table_insert(&assembler->types,"uint32",(void*)type_create("uint32",type_uint,4));
    hash_table_insert(&assembler->types,"uint64",(void*)type_create("uint64",type_uint,8));

    hash_table_insert(&assembler->types,"float",(void*)type_create("float",type_float,4));
    hash_table_insert(&assembler->types,"float32",(void*)type_create("float32",type_float,4));
    hash_table_insert(&assembler->types,"float64",(void*)type_create("float64",type_float,8));

    hash_table_insert(&assembler->types,"void",(void*)type_create("void",type_uint,0));

}

/*evaluates if the expression is valid and do auto conversion on numbers and handle pointer stuff

*/
stack_t* evaluate_stack(assembler_t* assembler, stack_t* stack){
    /*Remember to add all unused tokens to memory clear stack*/
    stack_t* num_stack = stack_new(10);
    stack_t* new_stack = stack_new(10);



    while(stack->top > 0){
        token_t* token = stack_pop(stack);
        if (token->type == NUMBER || token->type == IDENTIFIER){
            stack_push(num_stack,token);
        }else if (token->type == FUNCTION) {
            if (hash_table_check(&assembler->functions,token->value)){
                function_t* fn_def = hash_table_get(&assembler->functions,token->value);
                assert(fn_def != NULL);

                //Make sure there is enough arguments
                if (fn_def->argument_list->top > num_stack->top){
                    printf("Error expected %d arguments for %s, got %d",fn_def->argument_list->top,fn_def->name,num_stack->top);
                    exit(2);
                }
                /*
                for (int i = 0; i < fn_def->argument_list->top; i++){
                    token_t* num = (token_t*)stack_pop(num_stack);
                    if (num->type == NUMBER){
                        num->data_type = *((value_t*)fn_def->argument_list->data[i]);
                    }
                }
                */
            }else if (hash_table_check(&assembler->types,token->value)){
                type_t* type = hash_table_get(&assembler->types,token->value);

            }else{
                printf("Error at line %d unkown function %s",token->line_number,token->value);
                exit(2);
            }

        } else if (token->type == OPERATOR){

        }
    }

    return new_stack;

}


expression_t* evaluate_expression(assembler_t* assembler,expression_t* expression){

    /*Evaluate left side*/
    stack_t* lstack = expression->l_tokens;

    /*Evaluate right side*/
    stack_t* rstack = expression->r_tokens;



}

int assembler_get_pointer_count(stack_t* stack){

    int pointer_count = 0;
    for (int i = 0; i < stack->top; i++) {
        token_t* token = ((token_t*)stack->data[i]);
        assert(token != NULL);
        if (token->type == OPERATOR && token->value[0] == '$') pointer_count++;
    }
    return pointer_count;
}

/*
TODO should take strings into account when checking for $
*/
stack_t* assembler_prepare_for_shunting_yard(assembler_t* assembler,stack_t* stack, int total_pointer_count) {
    stack_t* temp_stack = stack_new(stack->top);  // Use capacity instead of top for stack_new

    int pointer_count = 0;
    for (int i = 0; i < stack->top; i++) {
        token_t* token = ((token_t*)stack->data[i]);
        
        if (token->type == OPERATOR && token->value[0] == '$') pointer_count++;

        if (token->type == OPERATOR && token->value[0] == '$' && pointer_count > total_pointer_count) {
            int left = 0;
            int right = 0;
            /*push to assembler->memory_to_clear so it can be cleared later*/
            stack_push(assembler->memory_to_clear,create_token(OPERATOR, "(", token->line_number));
            stack_push(temp_stack, stack_peak(assembler->memory_to_clear));
            
            stack_push(temp_stack, stack->data[i]);
            while(true) {
                i++;  // Move to the next token
                if (i >= stack->top) break;  // Prevent out-of-bounds access
                token = ((token_t*)stack->data[i]);
                assert(token != NULL);
                if (token->type == OPERATOR && token->value[0] == '(') left++;
                if (token->type == OPERATOR && token->value[0] == ')') right++;
                if (!(left != right || (token->type == IDENTIFIER || (token->type == OPERATOR && (token->value[0] == '.' || token->value[0] == '(' || token->value[0] == '$'))))) {i--; break;}
                stack_push(temp_stack, stack->data[i]);
            }
            
            stack_push(assembler->memory_to_clear,create_token(OPERATOR, ")", token->line_number));
            stack_push(temp_stack, stack_peak(assembler->memory_to_clear));


        } else {
            stack_push(temp_stack, stack->data[i]);
        }
    }

    if (total_pointer_count < pointer_count) {
        stack_t* tmp2 = assembler_prepare_for_shunting_yard(assembler,temp_stack, total_pointer_count + 1);
        stack_free(temp_stack);
        return tmp2;
    } else {
        return temp_stack;
    }
}

expression_t* assembler_handle_assignment(assembler_t* assembler,int statement_start,int statement_end){
    expression_t* expression = calloc(sizeof(expression_t),1);
    
    stack_t* temp_stack = stack_copy(assembler->tokenizer->tokens,statement_start,statement_end);
    stack_t* prepared_temp_stack = assembler_prepare_for_shunting_yard(assembler,temp_stack,0);
    stack_free(temp_stack);

    /*Run shunting yard to make it into rpn*/
    stack_t* rpn_stack = shunting_yard(assembler->tokenizer,prepared_temp_stack);
    stack_free(prepared_temp_stack);

    expression->l_tokens = rpn_stack;


    
    return expression;

}

expression_t* assembler_handle_expression(assembler_t* assembler,int statement_start){

    token_t* token = NULL;
    token_t* token_last = NULL;
    expression_t* expression = NULL;
    int assignment_pos = 0;
    int start = assembler->current_token;
    /*Find statement end && if its assignment*/
    do {
        token = ((token_t*)assembler->tokenizer->tokens->data[assembler->current_token++]);
        assert(token != NULL);
        if (token->value[0] == '=' && (token_last != NULL && (token_last->value[0]!='<' && token_last->value[0]!='>' && token_last->value[0]!='='))){
            /*check if there is multiple assigments and throw a error*/
            if (assignment_pos != 0){
                printf("error at line %d multiple assignments on a single line not allowed\n",token->line_number);
            }
            assignment_pos = assembler->current_token - 1;
        }
        token_last = token;
    } while(token->type != NEWLINE && token->type != END_OF_FILE);
    
    /*if assignment sign is found handle it*/
    if (assignment_pos != 0){

        printf("Assingment : \n");
        // L stack
        expression = assembler_handle_assignment(assembler,statement_start,assignment_pos);
        
        // R stack
        stack_t* r_stack = stack_copy(assembler->tokenizer->tokens,assignment_pos+1,assembler->current_token-1); /*-1 to not include NL/EOF +1 to not include "="*/
        stack_t* r_stack_prepared = assembler_prepare_for_shunting_yard(assembler,r_stack,0);

        expression->r_tokens = shunting_yard(assembler->tokenizer,r_stack_prepared);

        stack_free(r_stack);
        stack_free(r_stack_prepared);
    }else{
        expression = calloc(sizeof(expression_t),1);


        stack_t* l_stack = stack_copy(assembler->tokenizer->tokens,start,assembler->current_token-1); /*-1 to not include NL/EOF*/
        stack_t* l_stack_prepared = assembler_prepare_for_shunting_yard(assembler,l_stack,0);

        expression->l_tokens = shunting_yard(assembler->tokenizer,l_stack_prepared);
        
        stack_free(l_stack);
        stack_free(l_stack_prepared);
    }
    
    if (expression->l_tokens!= NULL){
        printf("\nltokens : \n");
        for (int i = 0; i<expression->l_tokens->top; i++){
        printf("%s ",((token_t*)expression->l_tokens->data[i])->value);
        }
    }
    
    if (expression->r_tokens!= NULL){
        printf("\nrtokens : \n");
        for (int i = 0; i<expression->r_tokens->top; i++){
        printf("%s ",((token_t*)expression->r_tokens->data[i])->value);
        }
    }

    exit(2);
    return expression;


}
uint8_t* assemble(tokenizer_t* tokenizer){

    assembler_t* assembler = (assembler_t*)calloc(sizeof(assembler_t),1);

    init_hash_table(&assembler->global_variables,20);

    init_hash_table(&assembler->functions,20);

    init_hash_table(&assembler->labels,20);

    init_hash_table(&assembler->blocks,20);

    init_hash_table(&assembler->types,20);
    assembler->memory_to_clear = stack_new(20);

    assembler->rpn_tokens = stack_new(20);

    populate_types_hashtable(assembler);


    assembler->tokenizer = tokenizer;
    int statement_start = 0;
    for (assembler->current_token = 0; assembler->current_token<assembler->tokenizer->tokens->top; assembler->current_token++){
        
        token_t* token = ((token_t*)tokenizer->tokens->data[assembler->current_token]);
        assert(token != NULL);

        if (assembler_is_function_definition(token)){
            function_t* f = assembler_handle_function_definition(assembler);
            hash_table_insert(&assembler->functions,f->name,(void*)f);
            print_table(&f->arguments);
            continue;
        }
        if (assembler_is_newline(token)){
            assembler->current_token++;
            continue;
        }

       // if (assembler_is_expression(assembler)){
        assembler_handle_expression(assembler,assembler->current_token);

         //   continue;
      //  }
        


      //  if (assembler_is_assignment(token)){
            //assembler_handle_assignment(assembler,statement_start);
       //     continue;
       // }

    }
    return (uint8_t*)0;
}

bool tokens_to_rpn(){

}
