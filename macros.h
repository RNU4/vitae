
#ifndef _MACRO_H_
#define _MACRO_H_

#define arg_count(value) (program.opcode += (value + 1))

#ifdef code_test
#define jump_next(value)  \

#else

#define jump_next(value)  \
    goto *jump_table[value]; \
    \

#endif

#define popr(type) (*(--type))

#ifdef debug

#define push_x(SP,type) \
        *(SP++) = *((type*)((program.u8 + 1))); \
        printf("%d:push %d\n",sizeof(type),*((type*)((program.u8 + 1)))); \
        jump_next(*arg_count(sizeof(type))); \

#define pop_x(SP,type) \
        SP--; \
        printf("%d:pop\n",sizeof(type)); \
        jump_next(*arg_count(0)); \


#define manual_push_x(SP,type,value) \
        *((SP++)) = (type)value; \
        printf("%d:push %d\n",sizeof(type),(type)value); \

#define add_x(SP,type) \
        printf("%d:add %lld + %lld\n",sizeof(type),*(SP-1),*(SP-2)); \
        *(SP-1) += *(--SP); \
        jump_next(*arg_count(0)); \

#define math_x(SP,type,operation) \
        printf("%d:math %d %s %d\n",sizeof(type),*(SP-1),#operation,*(SP-2)); \
        *(SP-1) operation *(--SP); \
        jump_next(*arg_count(0)); \


//! seems to work... but idk seems kinda sketchy
/*
#define convert_x(SP1,SP2,type1,type2) \
        printf("%d-%d:convert %s to %s\n",sizeof(type1),sizeof(type2),#type1,#type2); \
        *((type2*)(SP2-1)) = (type2)(*((type1*)(SP1-1))); \
        jump_next(*arg_count(0)); \

*/
#define convert_x(SP1,SP2,type1,type2) \
        printf("%d-%d:convert %s to %s\n",sizeof(type1),sizeof(type2),#type1,#type2); \
        *((type2*)(SP2++)) = (type2)(*((type1*)(--SP1))); \
        jump_next(*arg_count(0)); \

#define jmp_x(type) \
   printf("goto %d\n",(*((type*)(program.u8 + 1)))); \
   printf("instruction %d\n",*((program_start.u8 + (*((type*)(program.u8 + 1)))))); \
   jump_next(*(program.u8 = (program_start.u8 + (*((type*)(program.u8 + 1))))));  \

#define jmpr_x(type) \
   printf("relative goto %d\n",(*((type*)(program.u8 + 1)))); \
   printf("instruction %d\n",*((program.u8 + (*((type*)(program.u8 + 1)))))); \
   jump_next(*(program.u8 += (*((type*)(program.u8 + 1)))));  \

#define jmpcr_x(type) \
   printf("relative goto %d\n",(*((type*)(program.u8 + 1)))); \
   printf("instruction %d\n",*((program.u8 + (*((type*)(program.u8 + 1)))))); \
   printf("jump %d %s\n",*(sp.pointer-1),*(sp.pointer-1) == 1 ? "true":"false" ); \
jump_next(*(program.u8 += popr(sp.pointer) == 0 ? (*(type*)(program.u8 + 1))  : 1 + sizeof(pointer_type))); \


#define jmpc_x() \
jump_next(*(program.u8 = popr(sp.pointer) == 0 ? (program_start.u8 + (*(pointer_type*)(program.u8 + 1)))  : program.u8 + 1 + sizeof(pointer_type*))); \
 //jump_next(*(code = (number_u*)(((stack[--sp].i >> 31) + 1) * (int)(pcode + (*(code + 1)).i) + ((stack[sp].i >> 31) & 1) * ((int)code + 2))));

/*
    *fstack = dsp; 
    *(++fstack) = sp; // dsp = *(++fstack) = sp; ?
    dsp = sp;
    fstack++ = program.u8 + 1 + sizeof(pointer_type)); //  get distance from start + 2 
    jump_next(*(code += (*(pointer_type*)(program.u8 + 1))])); 
*/


// dsp = *(++dstack) = (pointer_type)sp.pointer; ? 
#define call_x() \
    printf("call jump pos %d\n", *(pointer_type*)(program.u8 + 1)); \
    *(dstack++) = (pointer_type*)(program.u8 + 1 + sizeof(pointer_type));  \
    *(dstack++) = dp.pointer;  \
    *(dstack++) = sp.pointer; \
    dp = sp; \
    jump_next(*(program.u8 += (*(pointer_type*)(program.u8 + 1))));  \

#define ret_x()\
    sp.pointer = *(--dstack); \
    dp.pointer = *(--dstack); \
    printf("ret return pos %d\n", (pointer_type)(*(dstack-1)) - (pointer_type)(program_start.u8)); \
    jump_next(*(program.u8 = (uint8_t*)(*(--dstack)))); \

//return varibles
#define retc_x()\
    sp.pointer = (pointer_type*)(((uint8_t*)*(--dstack)) + (*(uint8_t*)(program.u8 + 1))); \
    dp.pointer = *(--dstack); \
    printf("ret return pos %d\n", (pointer_type)(*(dstack-1)) - (pointer_type)(program_start.u8)); \
    jump_next(*(program.u8 = (uint8_t*)(*(--dstack)))); \


// stack[sp - 2].type = stack[sp - 2].type sign stack[--sp].type;
//! for some weird unexplanable reason it needs to done like this? possibly because of compiler code?
#define compare_x(SP,type,operation) \
        printf("%d: %d %s %d = %d\n",sizeof(type),*(SP-1),#operation,*(SP-2),(*(SP-1) operation *(SP-2))); \
        *((--SP)-1) = *(SP-1) operation *(SP-2); \
        jump_next(*arg_count(0)); \
        


/* //!temp varible needed
because for some reason the right side 
gets calculated first, so 
*(type*)((*(--sp.pointer))) = (type)(*(--SP)); is not possible

*(type*)((*(--sp.pointer))) = (type)(*(--SP)); is not possible

*/
//! can be optimized?(compiler might already do it) just using the temp for now
#define set_x(SP,temp,type) \
        temp = (type*)((*(--sp.pointer))); \
        *temp = (type)(*(--SP)); \
        printf("%d: adress %d = %d\n",sizeof(type),temp,(type)(*(SP))); \
        jump_next(*arg_count(0)); \
//    stack[dsp+stack[--sp].i] = stack[--sp]; // -1 = -2 "pop" 2

#define get_x(SP,type) \
        (type)(*(SP++)) = *(type*)((*(--sp.pointer))); \
        printf("%d: get adress %d value %d\n",sizeof(type),(type)(*(SP))); \
        jump_next(*arg_count(0)); \


//     stack[dsp+stack[--sp].i] = stack[--sp]; // -1 = -2 "pop" 2

//! can be optimized?(compiler might already do it) just using the temp for now
#define dp_set_x(SP,temp,type) \
        temp = (type*)(dp.u8+popr(sp.pointer)); \
        *temp = (type)(*(--SP)); \
        printf("%d:dp_adress %d = %d\n",sizeof(type),temp,(type)(*(SP))); \
        jump_next(*arg_count(0)); \

#define dp_get_x(SP,type) \
        *(SP++) = *(type*)((dp.u8+*(--sp.pointer))); \
        printf("%d:get adress %d value %d\n",sizeof(type),(type)(*(SP))); \
        jump_next(*arg_count(0)); \

#define get_dp() \
        *(sp.pointer++) = dp.pointer; \
        printf("get dp adress\n"); \
        jump_next(*arg_count(0)); \

#define get_sp() \
        *(sp.pointer) = ++sp.pointer; \
        printf("get sp adress\n"); \
        jump_next(*arg_count(0)); \




#else ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#define push_x(SP,type) \
        *(SP++) = *((type*)((program.u8 + 1))); \
        jump_next(*arg_count(sizeof(type))); \

#define pop_x(SP,type) \
        SP--; \
        jump_next(*arg_count(0)); \


#define manual_push_x(SP,type,value) \
        *((SP++)) = (type)value; \

#define add_x(SP,type) \
        *(SP-1) += *(--SP); \
        jump_next(*arg_count(0)); \

#define math_x(SP,type,operation) \
        *(SP-1) operation *(--SP); \
        jump_next(*arg_count(0)); \


#define convert_x(SP1,SP2,type1,type2) \
        *((type2*)(SP2++)) = (type2)(*((type1*)(--SP1))); \
        jump_next(*arg_count(0)); \

#define jmp_x(type) \
   jump_next(*(program.u8 = (program_start.u8 + (*((type*)(program.u8 + 1))))));  \

#define jmpr_x(type) \
   jump_next(*(program.u8 += (*((type*)(program.u8 + 1)))));  \

#define jmpcr_x(type) \
jump_next(*(program.u8 += popr(sp.pointer) == 0 ? (*(type*)(program.u8 + 1))  : 1 + sizeof(pointer_type))); \


#define jmpc_x() \
jump_next(*(program.u8 = popr(sp.pointer) == 0 ? (program_start.u8 + (*(pointer_type*)(program.u8 + 1)))  : program.u8 + 1 + sizeof(pointer_type*))); \

#define call_x() \
    *(dstack++) = (pointer_type*)(program.u8 + 1 + sizeof(pointer_type)*2);  \
    *(dstack++) = dp.pointer;  \
    *(dstack++) = (pointer_type*)(sp.u8 - *(program.u8 + 1 + sizeof(pointer_type))); \
    dp = sp; \
    jump_next(*(program.u8 += (*(pointer_type*)(program.u8 + 1))));  \

#define ret_x()\
    sp.pointer = *(--dstack); \
    dp.pointer = *(--dstack); \
    jump_next(*(program.u8 = (uint8_t*)(*(--dstack)))); \

#define retc_x()\
    sp.pointer = (pointer_type*)(((uint8_t*)*(--dstack)) + (*(uint8_t*)(program.u8 + 1))); \
    dp.pointer = *(--dstack); \
    jump_next(*(program.u8 = (uint8_t*)(*(--dstack)))); \

#define compare_x(SP,type,operation) \
        *(SP++) = *(--SP) operation *(--SP); \
        jump_next(*arg_count(0)); \
        

#define set_x(SP,temp,type) \
        temp = (type*)((*(--sp.pointer))); \
        *temp = (type)(*(--SP)); \
        jump_next(*arg_count(0)); \

#define get_x(SP,type) \
        (type)(*(SP++)) = *(type*)((*(--sp.pointer))); \
        jump_next(*arg_count(0)); \

#define dp_set_x(SP,temp,type) \
        temp = (type*)(dp.u8+popr(sp.pointer)); \
        *temp = (type)(*(--SP)); \
        jump_next(*arg_count(0)); \

#define dp_get_x(SP,type) \
        *(SP++) = *(type*)((dp.u8+*(--sp.pointer))); \
        jump_next(*arg_count(0)); \



#define dp_set_const_x(SP,temp,type) \
        *(type*)(dp.u8+(*(pointer_type*)(program.u8 + 1))) = (type)(*(--SP)); \
        jump_next(*arg_count(sizeof(pointer_type))); \

#define dp_get_const_x(SP,type) \
        *(SP++) = *(type*)(dp.u8+(*(pointer_type*)(program.u8 + 1))); \
        jump_next(*arg_count(sizeof(pointer_type))); \



#define get_dp() \
        *(sp.pointer++) = dp.pointer; \
        jump_next(*arg_count(0)); \

#define get_sp() \
        *(sp.pointer) = ++sp.pointer; \
        jump_next(*arg_count(0)); \

#endif


#endif