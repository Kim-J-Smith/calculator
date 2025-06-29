/**
 * @file        kim_calculator.c
 * @author      Kim
 * @version     0.1.0
 * @date        2025-6-26
 */
# include <stdlib.h>
# include <string.h>
# include "kim_calculator.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/**
 * @fn      pri_count
 * @attention only private use
 * @brief   count the number of the numbers in string
 * @param   self the string
 * @retval  the number of the numbers in string[uint32_t]
 */
static inline i32 pri_count(const char* self)
{
    i32 count = 0;
    u32 i;
    u8 flag_is_number = 0;
    u8 flag_has_point = 0;
    for(i = 0; self[i] != '\0'; i++) {
        if(flag_is_number == 1) {
            /* in number */
            if(self[i] >= '0' && self[i] <= '9') {
                continue;
            }
            else if(self[i] == '.') {
                if(flag_has_point == 1) { return -1; }
                flag_has_point = 1;
                continue;
            }
            else {
                count += flag_is_number;
                flag_has_point = 0;
                flag_is_number = 0;
            }
        }
        else {
            /* not in number */
            if(self[i] >= '0' && self[i] <= '9') {
                flag_is_number = 1;
            }
        }
    }
    count += flag_is_number;
    return count;
}

/**
 * @fn      Kim_calculator_load
 * @brief   Parse string content and then store them into a array
 * @param   self the string
 * @param   OUT_num [OUT] the number of numbers in the string
 * @retval  the pointer of array 
 * @attention the array is alloced in heap, remember to free it!
 */
token_t* Kim_calculator_load(const char* self, i32* OUT_num)
{
    i32 _num = pri_count(self);
    if(_num <= -1) { return NULL; }
    if(OUT_num != NULL) {
        *OUT_num = _num;
    }

    token_t* token_array = (token_t*) malloc(sizeof(token_t) * _num);
    if(token_array == NULL) { return NULL; }

    memset(token_array, 0, sizeof(token_t) * _num);

    u32 token_ref = 0;
    u8 allow_unary = 1; /* allow negative single number such as '-3' */
    u16 flag_negative = 0;
    u8 flag_is_number = 0;
    u64 flag_has_point = 0; /* the bits num after '.' */
    u64 _tmp_number = 0;
    for(u32 i = 0; self[i] != '\0'; i++) {
        if(flag_is_number == 1) {
            /* now, is in number */
            if(self[i] >= '0' && self[i] <= '9') {
                _tmp_number = (_tmp_number*10) + (self[i] - '0');
                if(flag_has_point) {
                    flag_has_point *= 10;
                }
                continue;
            }
            else if(self[i] == '.') {
                flag_has_point = 1;
                continue;
            }
            else {
                /* use int type or float type to store the number */
                i32 negative_factor = (flag_negative % 2) ? -1 : 1;
                flag_negative = 0;
                if(flag_has_point == 0) {
                    token_array[token_ref].data.int_data = negative_factor * _tmp_number;
                    token_array[token_ref].flag_int_or_float = Type_int;
                } else {
                    token_array[token_ref].data.float_data = 
                        negative_factor * (double)_tmp_number / flag_has_point;
                    token_array[token_ref].flag_int_or_float = Type_float;
                }
                _tmp_number = 0;
                flag_has_point = 0;
                flag_is_number = 0;
                token_ref++;
                i--;
            }
        }
        else {
            /* now, is not in number */
            if(self[i] >= '0' && self[i] <= '9') {
                i--;
                flag_is_number = 1;
                allow_unary = 0;
            }
            else if(self[i] == '+') {
                token_array[token_ref-1].Aoperator = AOperator_add;
            }
            else if(self[i] == '-') {
                /* check whether '-' means subtract or just negative single */
                if(allow_unary == 1) {
                    flag_negative += 1;
                    allow_unary = 0;
                } else {
                    token_array[token_ref-1].Aoperator = AOperator_sub;
                }              
            }
            else if(self[i] == '*') {
                token_array[token_ref-1].Aoperator = AOperator_mul;
            }
            else if(self[i] == '/') {
                token_array[token_ref-1].Aoperator = AOperator_div;
            }
            else if(self[i] == '(') {
                token_array[token_ref].bracket_before_num += 1;
                allow_unary = 1;
            }
            else if(self[i] == ')') {
                token_array[token_ref-1].bracket_after_num += 1;
            }
            else {
                continue;
            }
        }
    }
    /* use int type or float type to store the number */
    if(flag_is_number == 1 && token_ref < _num) {
        if(flag_has_point == 0) {
            token_array[token_ref].data.int_data = _tmp_number;
            token_array[token_ref].flag_int_or_float = Type_int;
        } else {
            token_array[token_ref].data.float_data = (double)_tmp_number / flag_has_point;
            token_array[token_ref].flag_int_or_float = Type_float;
        }
    }
    

    return token_array;
}

/**
 * @attention only private use
 */
static inline void pri_token_mul(token_t* now, token_t* next)
{
    if(now->flag_int_or_float == Type_int) {
        if(next->flag_int_or_float == Type_int) {
            /* now:int next:int */
            now->data.int_data *= next->data.int_data;
        } else {
            /* now:int next:float */
            now->data.float_data = 
                now->data.int_data * next->data.float_data;
            now->flag_int_or_float = Type_float;
        }
    } else {
        if(next->flag_int_or_float == Type_int) {
            /* now:float next:int */
            now->data.float_data *= next->data.int_data;
        } else {
            /* now:float next:float */
            now->data.float_data *= next->data.float_data;
        }
    }

    now->Aoperator = next->Aoperator;
    now->bracket_after_num = next->bracket_after_num;
    now->next_pos += 1 + next->next_pos;
}

/**
 * @attention only private use
 */
static inline void pri_token_div(token_t* now, token_t* next)
{
    if(now->flag_int_or_float == Type_int) {
        if(next->flag_int_or_float == Type_int) {
            /* now:int next:int */
            now->data.int_data /= next->data.int_data;
        } else {
            /* now:int next:float */
            now->data.float_data = 
                now->data.int_data / next->data.float_data;
            now->flag_int_or_float = Type_float;
        }
    } else {
        if(next->flag_int_or_float == Type_int) {
            /* now:float next:int */
            now->data.float_data /= next->data.int_data;
        } else {
            /* now:float next:float */
            now->data.float_data /= next->data.float_data;
        }
    }

    now->Aoperator = next->Aoperator;
    now->bracket_after_num = next->bracket_after_num;
    now->next_pos += 1 + next->next_pos;
}

/**
 * @attention only private use
 */
static inline void pri_token_add(token_t* now, token_t* next)
{
    if(now->flag_int_or_float == Type_int) {
        if(next->flag_int_or_float == Type_int) {
            /* now:int next:int */
            now->data.int_data += next->data.int_data;
        } else {
            /* now:int next:float */
            now->data.float_data = 
                now->data.int_data + next->data.float_data;
            now->flag_int_or_float = Type_float;
        }
    } else {
        if(next->flag_int_or_float == Type_int) {
            /* now:float next:int */
            now->data.float_data += next->data.int_data;
        } else {
            /* now:float next:float */
            now->data.float_data += next->data.float_data;
        }
    }

    now->Aoperator = next->Aoperator;
    now->bracket_after_num = next->bracket_after_num;
    now->next_pos += 1 + next->next_pos;
}

/**
 * @attention only private use
 */
static inline void pri_token_sub(token_t* now, token_t* next)
{
    if(now->flag_int_or_float == Type_int) {
        if(next->flag_int_or_float == Type_int) {
            /* now:int next:int */
            now->data.int_data -= next->data.int_data;
        } else {
            /* now:int next:float */
            now->data.float_data = 
                now->data.int_data - next->data.float_data;
            now->flag_int_or_float = Type_float;
        }
    } else {
        if(next->flag_int_or_float == Type_int) {
            /* now:float next:int */
            now->data.float_data -= next->data.int_data;
        } else {
            /* now:float next:float */
            now->data.float_data -= next->data.float_data;
        }
    }

    now->Aoperator = next->Aoperator;
    now->bracket_after_num = next->bracket_after_num;
    now->next_pos += 1 + next->next_pos;
}

/**
 * @fn      Kim_calculator_run
 * @attention recursive call
 * @brief   deal with the token_array
 * @param   token_array the token array pointer
 * @param   array_length the length of token array
 * @retval  None
 */
void Kim_calculator_run(token_t* token_array, i32 array_length)
{
    /* Firstly, deal with the '()' */
    for(i32 i = 0; i < array_length; ) { 
        if(token_array[i].bracket_before_num >= (u16)1) {
            token_array[i].bracket_before_num --;
            Kim_calculator_run(token_array + i, array_length - i);
        } 
        else if(token_array[i].bracket_after_num >= (u16)1) {
            break; /* go to the handler of * and / */
        }
        else {
            /* move to next token */
            i += token_array[i].next_pos + 1;
        }
    }

    /* Then, the handler for '*' '/' */
    for(i32 i = 0; i < array_length; ) {
        i32 add_to_next = 1 + token_array[i].next_pos;

        if(token_array[i].bracket_after_num >= (u16)1) {
            break; /* go to the handler of + and - */
        }
        /* Then, the '*' and '/' */
        else if(token_array[i].Aoperator == AOperator_mul) {
            pri_token_mul(token_array + i, token_array + i + add_to_next);
        }
        else if(token_array[i].Aoperator == AOperator_div) {
            pri_token_div(token_array + i, token_array + i + add_to_next);
        }
        else {
            /* move to next token */
            i += token_array[i].next_pos + 1;
        }
    }

    /* Finally, the handler for ')' '+' '-' */
    for(i32 i = 0; i < array_length; ) {
        i32 add_to_next = 1 + token_array[i].next_pos;

        /* Firstly, deal with the '()' */
        if(token_array[i].bracket_after_num >= (u16)1) {
            token_array[i].bracket_after_num --;
            return; /* back to the caller function */
        }
        /* Then, the '+' and '-' */
        else if(token_array[i].Aoperator == AOperator_add) {
            pri_token_add(token_array + i, token_array + i + add_to_next);
        }
        else if(token_array[i].Aoperator == AOperator_sub) {
            pri_token_sub(token_array + i, token_array + i + add_to_next);
        }
        else {
            /* move to next token */
            i += token_array[i].next_pos + 1;
        }
    }

    return;
}

/**
 * @fn      Kim_calculator_check_bracket
 * @brief   check the match of the token array
 * @param   token_array the token array
 * @param   array_length the length of the token array
 * @retval  0 means match_success; others means something wrong. 
 *          @c `1`: more ')' @c `2`: more '('
 */
i32 Kim_calculator_check_bracket(const token_t* token_array, u32 array_length)
{
    u32 i = 0;
    i32 num_before_bracket = 0;
    for(i = 0; i < array_length; i++) {
        num_before_bracket += (i32)token_array[i].bracket_before_num;
        num_before_bracket -= (i32)token_array[i].bracket_after_num;

        if(num_before_bracket < 0) {
            return 1; /* more ')' */
        }
    }

    if(num_before_bracket != 0) {
        return 2; /* more '(' */
    }
    return 0; /* everything goes well */
}

/**
 * @fn      Kim_calculator
 * @brief   Parse the string, and then give the result
 * @param   _string the string
 * @retval  the result, with type, value, and except_flag
 * @note    `There_is_something_wrong` member value of return struct contain 
 *          the except_number @c `1`: no alloc @c `2`: 0 number @c `3`: "()" problem
 */
calculator_result_t Kim_calculator(const char* _string)
{
    i32 _num = 0;
    calculator_result_t the_result;
    token_t* _tmp = Kim_calculator_load(_string, &_num);

    /* except handler */
    if(_tmp == NULL) {
        the_result.There_is_something_wrong = 1; /* no alloc */
        the_result.data.int_data = 0;
        the_result.flag_int_or_float = Type_int;
        return the_result;
    }
    /* except handler */
    if(_num == 0) {
        the_result.There_is_something_wrong = 2; /* 0 number */
        the_result.data.int_data = 0;
        the_result.flag_int_or_float = Type_int;
        return the_result;
    }
    /* except handler*/
    if( Kim_calculator_check_bracket(_tmp, _num) ) {
        free(_tmp); /* free the memory */
        the_result.There_is_something_wrong = 3; /* "()" problem */
        the_result.data.int_data = 0;
        the_result.flag_int_or_float = Type_int;
        return the_result;
    }
    Kim_calculator_run(_tmp, _num);

    the_result.flag_int_or_float = _tmp[0].flag_int_or_float; 
    the_result.data.int_data = _tmp[0].data.int_data;
    the_result.There_is_something_wrong = 0;
    free(_tmp);
    return the_result;
}

/**
 * @fn      Kim_calculator_error_get
 * @brief   get the error information of the result
 * @param   error_num the calculator_result_t.There_is_something_wrong
 * @param   OUT_information [OUT] the char array to get the error information
 * @param   info_max_size the size of the char array
 * @retval  None
 */
void Kim_calculator_error_get(u32 error_num, char* OUT_information, u32 info_max_size)
{
    if(OUT_information == NULL) { return; }
    switch (error_num)
    {
    case 0:
        memcpy(OUT_information, "[calculator-error-number] success", info_max_size);
        break;
    case 1:
        memcpy(OUT_information, "[calculator-error-number] no alloc memory", info_max_size);
        break;
    case 2:
        memcpy(OUT_information, "[calculator-error-number] cannot parse number", info_max_size);
        break;
    case 3:
        memcpy(OUT_information, "[calculator-error-number] '()' problem", info_max_size);
        break;
    default:
        memcpy(OUT_information, "[calculator-error-number] unknown error", info_max_size);
        break;
    }
}

#ifdef __cplusplus
}
#endif /* __cplusplus */
