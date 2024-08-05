#include <stdint.h>
#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include "./lib/file_handler.h"
#include "./lib/opcodes.h"
#define byte uint8_t
#define pointer_type uint32_t
#define pointer_type_signed int32_t

#define opcode_type uint8_t

//#define debug
//#define code_test
#define LABEL_INIT(op) [op] = &&op##_label


#define UINT32TO8(number) \
    (uint8_t)((number) & 0xFF), \
    (uint8_t)(((number) >> 8) & 0xFF), \
    (uint8_t)(((number) >> 16) & 0xFF), \
    (uint8_t)(((number) >> 24) & 0xFF)


typedef union stack_pointer {
    uint8_t *u8;
    uint16_t *u16;
    uint32_t *u32;
    uint64_t *u64;
    int8_t *i8;
    int16_t *i16;
    int32_t *i32;
    int64_t *i64;
    
    uint32_t *pointer;
    int32_t *signed_pointer;

    opcode_type  *opcode;

    float *f;
    double *d;
}stack_pointer_u;

struct interpreter
{
    /* data */
    uint32_t temp;
}interpreter_t;


#include "macros.h"
#include "super_macros.h"

int main(){

    pointer_type** fstack = (pointer_type**)calloc(128,sizeof(pointer_type**));
    pointer_type** dstack = (pointer_type**)calloc(128,sizeof(pointer_type**));
    stack_pointer_u stack = (stack_pointer_u)(uint8_t*)calloc(128,1);
    stack_pointer_u sp = stack;
    stack_pointer_u dp = stack;

    stack_pointer_u temp;



    
  /*  uint8_t prog[] = {
        push32,UINT32TO8(8),
        alloc,
        push32,UINT32TO8(0),push32,UINT32TO8(var_sum), // sum = 0
        dp_set32,
        push32,UINT32TO8(0),push32,UINT32TO8(var_i),   // i = 1
        dp_set32,
        push32,UINT32TO8(200000000),        // number
        push32,UINT32TO8(var_i),dp_get32, // i
        lssu32, // i < number
        jmpcr,UINT32TO8(47),
        
        push32,UINT32TO8(var_i),dp_get32, // i
        push32,UINT32TO8(1), // number
        addi32, // number + i
        push32,UINT32TO8(var_i),dp_set32,   // i = number + i

        push32,UINT32TO8(var_sum),dp_get32, // sum
        push32,UINT32TO8(var_i),dp_get32, // i
        addi32, // sum + i
        push32,UINT32TO8(var_sum),dp_set32,   // sum = sum + i

        jmpr,UINT32TO8(-54),
        halt
    };*/
//1 1 1 1 1

    #define var_i 4
    #define var_sum 0
    uint8_t prog[] = {
        push32,UINT32TO8(8),
        alloc,
        push32,UINT32TO8(0), 
        dp_set_const32,UINT32TO8(var_sum), // sum = 0
        push32,UINT32TO8(0),   
        dp_set_const32,UINT32TO8(var_i), // i = 0


        /* while i < number */
        push32,UINT32TO8(200000000),        // number
        dp_get_const32,UINT32TO8(var_i), // i
         // i < number
        S_LSSJMPCR_I32,UINT32TO8(38), // if i>number jump to end
        
        /*i = number + i*/
        S_DPGETCPUSHADD_U32,UINT32TO8(var_i),UINT32TO8(1),
        dp_set_const32,UINT32TO8(var_i),   // i = number + i

        /*sum = sum + i*/
        S_DPGETCX2ADD_U32,UINT32TO8(var_sum),UINT32TO8(var_i), // get sum get i add
        dp_set_const32,UINT32TO8(var_sum),   // sum = sum + i

        /*jump back to while loop start*/
        jmpr,UINT32TO8(-43),
        halt
    };
    #undef var_i
    #undef var_sum

   // push32,UINT32TO8(var_sum)
   // push,4,UINT32TO8(var_sum)

    stack_pointer_u program_start = (stack_pointer_u)(uint8_t*)&prog;
    stack_pointer_u program = program_start;
    //program = read_file("./file.bin");

    static void *jump_table[opcode_count] = {
        LABEL_INIT(halt),
        LABEL_INIT(jmp),
        LABEL_INIT(jmpr),
        LABEL_INIT(jmpcr),
        LABEL_INIT(alloc),
        LABEL_INIT(call),
        LABEL_INIT(ret),
        LABEL_INIT(retc),
        


        LABEL_INIT(push64),LABEL_INIT(push32),
        LABEL_INIT(push16),LABEL_INIT(push8),

        LABEL_INIT(pop64),LABEL_INIT(pop32),
        LABEL_INIT(pop16),LABEL_INIT(pop8),

        LABEL_INIT(addi64),LABEL_INIT(subi64),
        LABEL_INIT(muli64),LABEL_INIT(divi64),

        LABEL_INIT(addi32),LABEL_INIT(subi32),
        LABEL_INIT(muli32),LABEL_INIT(divi32),

        LABEL_INIT(addi16),LABEL_INIT(subi16),
        LABEL_INIT(muli16),LABEL_INIT(divi16),

        LABEL_INIT(addi8),LABEL_INIT(subi8),
        LABEL_INIT(muli8),LABEL_INIT(divi8),

        LABEL_INIT(addu64),LABEL_INIT(subu64),
        LABEL_INIT(mulu64),LABEL_INIT(divu64),

        LABEL_INIT(addf64),LABEL_INIT(subf64),
        LABEL_INIT(mulf64),LABEL_INIT(divf64),
    
        LABEL_INIT(b8to16),   LABEL_INIT(b16to8),   LABEL_INIT(b32to8),    LABEL_INIT(b64to8),
        LABEL_INIT(b8to32),   LABEL_INIT(b16to32),  LABEL_INIT(b32to16),   LABEL_INIT(b64to16),
        LABEL_INIT(b8to64),   LABEL_INIT(b16to64),  LABEL_INIT(b32to64),   LABEL_INIT(b64to32),

        LABEL_INIT(dp_set8),  LABEL_INIT(dp_set16), LABEL_INIT(dp_set32),  LABEL_INIT(dp_set64),
        LABEL_INIT(dp_get8),  LABEL_INIT(dp_get16), LABEL_INIT(dp_get32),  LABEL_INIT(dp_get64),

        LABEL_INIT(dp_set_const8),  LABEL_INIT(dp_set_const16), LABEL_INIT(dp_set_const32),  LABEL_INIT(dp_set_const64),
        LABEL_INIT(dp_get_const8),  LABEL_INIT(dp_get_const16), LABEL_INIT(dp_get_const32),  LABEL_INIT(dp_get_const64),


        LABEL_INIT(S_DPGETCX2ADD_U8),
        LABEL_INIT(S_DPGETCX2ADD_U16),
        LABEL_INIT(S_DPGETCX2ADD_U32),
        LABEL_INIT(S_DPGETCX2ADD_U64),

        LABEL_INIT(S_DPGETCPUSHADD_U8),
        LABEL_INIT(S_DPGETCPUSHADD_U16),
        LABEL_INIT(S_DPGETCPUSHADD_U32),
        LABEL_INIT(S_DPGETCPUSHADD_U64),

        LABEL_INIT(S_LSSJMPCR_I32),
        LABEL_INIT(S_GRTJMPCR_I32),

        LABEL_INIT(S_LSSJMPCR_U32),
        LABEL_INIT(S_GRTJMPCR_U32),
        LABEL_INIT(S_EQLJMPCR_U32),
        LABEL_INIT(S_NQLJMPCR_U32),


        LABEL_INIT(lssu32),
        LABEL_INIT(grtu32),
    };
    //make unused labels cause error
    for (int i = 0; i <opcode_count; i++){
        if (jump_table[i] == 0)
            jump_table[i] = &&unkown_label;
    }

    double start_time = clock();
    goto *jump_table[*program.opcode];
   
    halt_label:
    printf("\ntime to run : %.10f\n",(double)((double)clock() - start_time) / (double)CLOCKS_PER_SEC);
    printf("halt\n");
    for (int i = 0; i < 10; i++) {
        printf("[%d] : %lu \n", i, *(uint32_t*)(stack.u8+i*4));
    }

    /*
    printf("program\n");
    for (int i = 0; i < 11; i++) {
        printf("[%d] : %d \n", i, *(byte*)(program_start+i));
    }
*/
    *(program_start.i8) = 22;
    printf("%d",*(program_start.i8));
    return 0;

    /*PUSH & POP */
    push64_label: push_x(sp.u64,uint64_t); pop64_label: pop_x(sp.u64,uint64_t);
    push32_label: push_x(sp.u32,uint32_t); pop32_label: pop_x(sp.u32,uint32_t);
    push16_label: push_x(sp.u16,uint16_t); pop16_label: pop_x(sp.u16,uint16_t);
    push8_label : push_x(sp.u8,uint8_t);  pop8_label:  pop_x(sp.u8,uint8_t);

    addi64_label: math_x(sp.i64,int64_t,+=);
    subi64_label: math_x(sp.i64,int64_t,-=);
    muli64_label: math_x(sp.i64,int64_t,*=);
    divi64_label: math_x(sp.i64,int64_t,/=);

    addi32_label: math_x(sp.i32,int32_t,+=);
    subi32_label: math_x(sp.i32,int32_t,-=);
    muli32_label: math_x(sp.i32,int32_t,*=);
    divi32_label: math_x(sp.i32,int32_t,/=);

    addi16_label: math_x(sp.i16,int16_t,+=);
    subi16_label: math_x(sp.i16,int16_t,-=);
    muli16_label: math_x(sp.i16,int16_t,*=);
    divi16_label: math_x(sp.i16,int16_t,/=);

    addi8_label: math_x(sp.i8,int8_t,+=);
    subi8_label: math_x(sp.i8,int8_t,-=);
    muli8_label: math_x(sp.i8,int8_t,*=);
    divi8_label: math_x(sp.i8,int8_t,/=);

    addu64_label: math_x(sp.u64,uint64_t,+=);
    subu64_label: math_x(sp.u64,uint64_t,-=);
    mulu64_label: math_x(sp.u64,uint64_t,*=);
    divu64_label: math_x(sp.u64,uint64_t,/=);

    addf64_label: math_x(sp.d,double,+=);
    subf64_label: math_x(sp.d,double,-=);
    mulf64_label: math_x(sp.d,double,*=);
    divf64_label: math_x(sp.d,double,/=);

    b8to16_label: convert_x(sp.u8,sp.u16,uint8_t,uint16_t);
    b8to32_label: convert_x(sp.u8,sp.u32,uint8_t,uint32_t);
    b8to64_label: convert_x(sp.u8,sp.u64,uint8_t,uint64_t);

    b16to8_label : convert_x(sp.u16,sp.u8,uint16_t,uint8_t);
    b16to32_label: convert_x(sp.u16,sp.u32,uint16_t,uint32_t);
    b16to64_label: convert_x(sp.u16,sp.u64,uint16_t,uint64_t);

    b32to8_label : convert_x(sp.u32,sp.u8,uint32_t,uint8_t);
    b32to16_label: convert_x(sp.u32,sp.u16,uint32_t,uint16_t);
    b32to64_label: convert_x(sp.u32,sp.u64,uint32_t,uint64_t);

    b64to8_label : convert_x(sp.u64,sp.u8,uint64_t,uint8_t);
    b64to16_label: convert_x(sp.u64,sp.u16,uint64_t,uint16_t);
    b64to32_label: convert_x(sp.u64,sp.u32,uint64_t,uint32_t);

    dp_set8_label : dp_set_x(sp.u8,temp.u8,uint8_t);
    dp_set16_label: dp_set_x(sp.u16,temp.u16,uint16_t);
    dp_set32_label: dp_set_x(sp.u32,temp.u32,uint32_t);
    dp_set64_label: dp_set_x(sp.u64,temp.u64,uint64_t);

    dp_get8_label : dp_get_x(sp.u8,uint8_t);
    dp_get16_label: dp_get_x(sp.u16,uint16_t);
    dp_get32_label: dp_get_x(sp.u32,uint32_t);
    dp_get64_label: dp_get_x(sp.u64,uint64_t);

    dp_set_const8_label : dp_set_const_x(sp.u8,temp.u8,uint8_t);
    dp_set_const16_label: dp_set_const_x(sp.u16,temp.u16,uint16_t);
    dp_set_const32_label: dp_set_const_x(sp.u32,temp.u32,uint32_t);
    dp_set_const64_label: dp_set_const_x(sp.u64,temp.u64,uint64_t);

    dp_get_const8_label : dp_get_const_x(sp.u8,uint8_t);
    dp_get_const16_label: dp_get_const_x(sp.u16,uint16_t);
    dp_get_const32_label: dp_get_const_x(sp.u32,uint32_t);
    dp_get_const64_label: dp_get_const_x(sp.u64,uint64_t);

    lssu32_label: compare_x(sp.u32,uint32_t,<);
    grtu32_label: compare_x(sp.u32,uint32_t,>);

    /*SUPER OPCODES*/
    
    S_DPGETCX2ADD_U8_label : DP_GET_CONST_DP_GET_CONST_ADD_X(sp.u8,uint8_t);
    S_DPGETCX2ADD_U16_label: DP_GET_CONST_DP_GET_CONST_ADD_X(sp.u16,uint16_t);
    S_DPGETCX2ADD_U32_label: DP_GET_CONST_DP_GET_CONST_ADD_X(sp.u32,uint32_t);
    S_DPGETCX2ADD_U64_label: DP_GET_CONST_DP_GET_CONST_ADD_X(sp.u64,uint64_t);

    S_DPGETCPUSHADD_U8_label: DP_GET_CONST_PUSH_ADD_X(sp.u8,uint8_t);
    S_DPGETCPUSHADD_U16_label: DP_GET_CONST_PUSH_ADD_X(sp.u16,uint16_t);
    S_DPGETCPUSHADD_U32_label: DP_GET_CONST_PUSH_ADD_X(sp.u32,uint32_t);
    S_DPGETCPUSHADD_U64_label: DP_GET_CONST_PUSH_ADD_X(sp.u64,uint64_t);

    S_LSSJMPCR_I32_label: JMPCR_COMPARE_X(sp.i32,int32_t,<);
    S_GRTJMPCR_I32_label: JMPCR_COMPARE_X(sp.i32,int32_t,>);

    
    S_LSSJMPCR_U32_label: JMPCR_COMPARE_X(sp.u32,uint32_t,<);
    S_GRTJMPCR_U32_label: JMPCR_COMPARE_X(sp.u32,uint32_t,>);
    S_EQLJMPCR_U32_label: JMPCR_COMPARE_X(sp.u32,uint32_t,==);
    S_NQLJMPCR_U32_label: JMPCR_COMPARE_X(sp.u32,uint32_t,!=);

//JMPCR_COMPARE_X
    jmp_label : jmp_x(uint32_t);
    jmpr_label: jmpr_x(int32_t);
    jmpcr_label: jmpcr_x(int32_t);

    call_label: call_x();
    ret_label: ret_x();
    retc_label: retc_x();


    unkown_label: /*instruction not found*/
    printf("Error unkown instruction %d, halting program\n",*(program.u8));
    goto halt_label;
    
    //? what size tho... pointer type for now i guess
    alloc_label:
    #ifdef debug
    printf("4:alloc %d \n",(*(sp.pointer-1)));
    #endif
    sp.u8+=popr(sp.pointer);
    jump_next(*(arg_count(0)));

    goto halt_label;
    

}

