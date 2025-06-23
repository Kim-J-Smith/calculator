# pragma once

typedef char                i8;
typedef short               i16;
typedef int                 i32;
typedef long long           i64;

typedef unsigned char       u8;
typedef unsigned short      u16;
typedef unsigned int        u32;
typedef unsigned long long  u64;

typedef enum {
    AOperator_add = (1 << 0),
    AOperator_sub = (1 << 1),
    AOperator_muti = (1 << 2),
    AOperator_div = (1 << 3),
    AOperator_null = 0
}Arith_operator_t;

typedef enum {
    Type_int = 0,
    Type_float = 1
}Type_int_or_float_t;

union _data{
    i64 int_data;
    double float_data;
};

typedef struct {
    union _data data;
    /* arithmetic operator */
    Arith_operator_t Aoperator; 
    u16 bracket_before_num;
    u16 bracket_after_num;
    /* when the next is next one, next_pos == 0 (default)*/
    u32 next_pos; 
    Type_int_or_float_t flag_int_or_float; /* 0-->int 1-->float */
}token_t;

typedef struct {
    union _data data;
    Type_int_or_float_t flag_int_or_float; /* 0-->int 1-->float */
}calculator_result_t;

# define AOPERATOR_MASK         ((u32)0xff)
# ifndef NULL
    #define NULL ((void*)0)
# endif /* NULL */

calculator_result_t Kim_calculator(const char* _string);
