
#ifndef _SUPER_MACRO_H_
#define _SUPER_MACRO_H_
#define arg_count(value) (program.opcode += (value + 1))
#define jump_next(value)  \
    goto *jump_table[value]; \
    \


#define DP_GET_CONST_DP_GET_CONST_ADD_X(SP,type) \
        *(SP++) = *(type*)(dp.u8+(*(pointer_type*)(program.u8 + 1))) + *(type*)(dp.u8+(*(pointer_type*)(program.u8 + 1 + sizeof(pointer_type)))); \
        jump_next(*arg_count(sizeof(pointer_type)*2)); \


#define DP_GET_CONST_PUSH_ADD_X(SP,type) \
    *(SP++) = *(type*)(dp.u8+(*(pointer_type*)(program.u8 + 1))) + *(pointer_type*)(program.u8 + 1 + sizeof(pointer_type)); \
    jump_next(*arg_count(sizeof(pointer_type)*2)); \


/*
   printf("relative goto %d\n",(*((type*)(program.u8 + 1)))); \
   printf("instruction %d\n",*((program.u8 + (*((type*)(program.u8 + 1)))))); \
   printf("jump %d %s %d %s\n",*(SP-2),#operation,*(SP-1),(*(SP-2) operation *(SP-1)) == 1 ? "true":"false" ); \
*/

#define JMPCR_COMPARE_X(SP,type,operation) \
    jump_next(*(program.u8 += (*(--SP) operation *(--SP)) == 0 ? (*(type*)(program.u8 + 1))  : 1 + sizeof(pointer_type))) \


#endif