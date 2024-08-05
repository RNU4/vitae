#ifndef _STACK_H_
#define _STACK_H_
#include "./lib/stack.h"
#include "./hashtable.h"
// Define token types
#define pointer_size 4


enum token_type {
    KEYWORD = (uint8_t)0,
    IDENTIFIER, // +-*/%^#=<>;,.(){}[]
    NUMBER,
    FUNCTION,
    STRING,
    OPERATOR,
    COMMENT,
    NEWLINE,
    END_OF_FILE,

};

enum type_u {
    type_uint = (uint8_t)0,
    type_int,
    type_float,
    type_pointer,
    type_block, /* basically a struct*/
};
typedef struct
{
    /* data */
    char* name; 
    enum type_u type;
    uint8_t size; // 1 2 4 8
}type_t;

type_t* type_create(char* name,enum type_u type,uint8_t size){
    type_t* t = malloc(sizeof(type_t));
    t->name = name;
    t->type = type;
    t->size = size;
    return t;
}

typedef struct {
    char* type_name;
    uint8_t depth; // is it int, int* or int**
} value_t;

// Token structure

typedef struct {
    enum token_type type;
    char *value;
    int line_number; // what line the token is on for error reporting


    value_t data_type;
} token_t;

typedef struct {
    stack_t* tokens;
    char* ptr; // string pointer
    int current_line_number; // what line the token is on for error reporting
} tokenizer_t;




value_t* value_create(char* type_name,uint8_t size,uint8_t depth){
    value_t* val = malloc(sizeof(value_t));
    val->depth = depth;
    val->type_name = type_name;
    return val;
}


typedef struct {
    char* name;
    /*is_adress means that the variable is permanent adress for example 
    int[] str = "hello" is a permanent adress
    meaning that the adress of the variable is the same as what it is pointing to.
    in short &str == &(*str)
    where int* x = malloc(20) it would not be same
    in short &x != &(*x)
    */
    bool is_adress;
    value_t value;
} variable_t;

typedef struct {
    bool lvalue;
    bool rvalue;
    stack_t* l_tokens;
    stack_t* r_tokens;
    /*if it is first time assigment like x = 3 or sum*/
    //variable_t lvalue; // lvalue = rvalue

} expression_t;


typedef struct {
    char* name;
    stack_t* fields; // variable_t* 
    size_t size; // size of data block
} block_t;


typedef struct {
    char* name;
    value_t _return; // size and type
    hashtable_t arguments; // variable_t* 
    stack_t* argument_list; // variable_t* 

    stack_t* tokens; // tokens inside function;
    hashtable_t local_variables;
    bool referenced; // if never used dont include it in the code unless the INCLUDE_ALL_FUNCTIONS flag is set
} function_t;

function_t* function_create(){
    function_t* f = (function_t*)calloc(sizeof(function_t),1);

    f->argument_list = stack_new(5);
    init_hash_table(&f->arguments,10);

    init_hash_table(&f->local_variables,10);
    return f;
}


typedef struct {
    char* type_name;
    uint8_t depth; // is it int, int* or int**
} eval_number_t;









enum optimization_level {
    O0 = (uint8_t)0, /*default does nothing*/
    O1,
    OSIZE,
    OFAST, // ofast is designed to possibly break code but make it faster
};


enum compiler_flags {
    INLINE_FUNCTIONS = (uint8_t)0, /*inlines functions instead of calling them*/
    COMBINE_UINT_ADD, /*addu8 addu8 = addu16, this will break some code*/
    COMBINE_PUSH, /*push8 push8 = push16*/
    USE_SUPER_INSTRUCTIONS, /*combine instructions into a predefined larger instruction*/
    USE_CONST_INSTRUCTIONS, /*will use const versions of instructions when possible*/
    INCLUDE_ALL_FUNCTIONS, /*include all functions no matter if refrenced or not*/
    REMOVE_UNREFRENCED_VARIABLES, /*will remove variables who are not refrenced*/
    REMOVE_UNREFRENCED_DATA, /*will remove for example strings who are not refrenced*/
    PRECALCULATE_VARIABLES, /*will attempt to calculate stuff like x=2+5 into x=7*/

    /*this will do stuff like storing a 8 bit number and then make it into a 32 bit number 
    when using instructions requiring 32bit argument. (slows program down)*/
    MINIMIZE_DATA, 
    /*this will attempt to compress the bytecode, the trade off is the interpreter needs to unpack it.
    if the resulting binary is bigger it will be discarded for the original binary*/
    COMPRESS_DATA,

    //? implementation at later point
    TYPELESS, /*will not require types, this also disables a lot of low level control*/
    AUTOTYPE, /*will attempt to auto asign a type.*/
    AUTOCONVERT, /*will attempt to auto convert a type*/

    /*compiler flags that change how code is written*/
    USE_NEWLINE_INSTEAD_OF_COLON,
    };

typedef struct {
    tokenizer_t* tokenizer;
    hashtable_t global_variables;
    hashtable_t blocks; // basically structs
    hashtable_t types; // basically structs
    stack_t* rpn_tokens;

    stack_t* memory_to_clear;

    hashtable_t labels;
    hashtable_t functions;
    enum optimization_level optimization_level;
    int current_token;
    uint32_t flags;
    uint8_t* bytecode;

} assembler_t;


#endif