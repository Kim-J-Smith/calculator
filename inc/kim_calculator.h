# pragma once

/* typedef of different int type */
typedef char                i8;
typedef short               i16;
typedef int                 i32;
typedef long long           i64;

typedef unsigned char       u8;
typedef unsigned short      u16;
typedef unsigned int        u32;
typedef unsigned long long  u64;

/* typedef of a enum to indicate the arithmetic operator */
typedef enum {
    AOperator_add = (1 << 0),
    AOperator_sub = (1 << 1),
    AOperator_mul = (1 << 2),
    AOperator_div = (1 << 3),
    AOperator_null = 0
}Arith_operator_t;

/* typedef of a enum to indicate the type (int or float) */
typedef enum {
    Type_int = 0,
    Type_float = 1
}Type_int_or_float_t;

/* typedef of a union to store the data, 
 * which is whether int or float type */
union _data{
    i64 int_data;
    double float_data;
};

/**
 * @struct      token_t
 * @brief       A struct that store a number's value and other information
 * @param       data store the value
 * @param       Aoperator store the operator after the number
 * @param       bracket_before_num store the number of brackets before the number
 * @param       bracket_after_num store the number of brackets after the number  
 * @param       next_pos store the distance from now to next, if the next token is near,
 *                  the member variable should be set to 0 by default
 * @param       flag_int_or_float store the type of data ( Type_int:0 or Type_float:1 )     
 */
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

/* the result information */
typedef struct {
    union _data data;
    Type_int_or_float_t flag_int_or_float; /* 0-->int 1-->float */
    u32 There_is_something_wrong; /* not 0 means something wrong happened */
}calculator_result_t;

#ifdef __cplusplus
    #define NULL nullptr
#else
    #define NULL ((void*)0)
#endif /* __cplusplus */

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

extern calculator_result_t Kim_calculator(const char* _string);
extern token_t* Kim_calculator_load(const char* self, i32* OUT_num);
extern void Kim_calculator_run(token_t* token_array, i32 array_length);

#ifdef __cplusplus
}
#endif /* __cplusplus */
