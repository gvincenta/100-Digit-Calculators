/* Extended precision integer calculator 
 * Implements +, *, and ^ (power of) operations
 *
 * Skeleton code written by Jianzhong Qi, March 2018
 * Assignment done by Gilbert Vincenta, ID: 941088
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define INT_SIZE    100    /* max number of digits per integer value */
#define LINE_LEN    103    /* maximum length of any input line */
#define NUM_VARS    10    /* number of different huge int "variables" */

#define ASN_OP        '='    /* assignment operator */
#define ECH_OP        '?'    /* echo operator */
#define ADD_OP        '+'    /* addition operator */
#define MUL_OP        '*'    /* multiplication operator */
#define POW_OP        '^'    /* power of operator */

#define OPR1_POS    1    /* position of the first operand */
#define OPR2_POS    3    /* position of the second operand */
#define OP_POS        2    /* position of the operator */

#define CH_ZERO        '0'    /* character 0 */

#define EXIT_CMD     "exit"    /* command to exit */
#define PROMPT        "> "    /* command prompt */
#define CMT_FLAG    '%'        /* indicator for comment line */

typedef int digit_t;                /* a decimal digit */
typedef digit_t huge_t[INT_SIZE];    /* one huge int "variable" */

/* added by me */
#define LEN_ONE   1 /* length of 1 */
#define DIGIT_ZERO  0 /* number 0 */
#define DIGIT_ONE 1 /*number 1*/
#define OFFSET 10 /* upper limit value in one digit */
#define VAR_NOTATION 'n' /* all variables start with 'n' */
#define THE_SAME 1 /*operand 2 equals to operand 1*/
#define FIRST_DIGIT 0 /*first digit lies at index 0*/

/****************************************************************/

/* Jianzhong Qi's prototype */
void read_line(char *line, int max_len);
void init(huge_t vars[], int lens[]);
void echo(huge_t vars[], int lens[], int opr1_index);
void assign(huge_t vars[], int lens[], int opr1_index, char *opr2_str);
void add(huge_t vars[], int lens[], int opr1_index, char *opr2_str);
void multiply(huge_t vars[], int lens[], int opr1_index, char *opr2_str);
void power(huge_t vars[], int lens[], int opr1_index, char *opr2_str);

/* my prototype */
int check_digit_difference(int opr1_len, int opr2_len);

int text_to_array(char *opr2_str, digit_t A[]);

void add_loop(int opr2_len, int *opr1_len, int digit_diff, digit_t *opr1,
              digit_t *opr2);

void clone(digit_t *opr1, digit_t A[], int max_len);

int initialise(char *opr2_str, int lens[], digit_t temp[], int *opr2_len,
               huge_t vars[], int *digit_diff, int opr1_index,
               digit_t **opr2_int, int is_assign);

void multiply_digit(digit_t *one_digit, digit_t *many_digits,
                    int position_index, int many_digits_len, digit_t result[],
                    int *result_len);

void remove_zeros(int *len, digit_t *current_array);

void multiply_loop(int opr2_len, digit_t *opr2_int, int *opr1_len,
                   digit_t *opr1);
/****************************************************************/

/* main function controls all the action, do NOT modify this function */

int
main(int argc, char *argv[]) {
    char line[LINE_LEN+1];        /* to hold the input line */
    huge_t vars[NUM_VARS];        /* to hold 10 huge integers */
    int lens[NUM_VARS];            /* to hold the length of the 10 vars */
    
    int opr1_index;                /* index of the first operand in command */
    char op;                    /* operator in command */
    
    init(vars, lens);
    
    while (1) {
        printf(PROMPT);                        /* print prompt */
        read_line(line, LINE_LEN);            /* read one line of command */
        
        if (line[0] == CMT_FLAG) {            /* print comment in the test data
                                               */
            printf("%s\n", line);            /* used to simplify marking */
            continue;
        }
        
        if (strcmp(line, EXIT_CMD) == 0) {    /* see if command is "exit" */
            return 0;
        }
        
        opr1_index = line[OPR1_POS] - CH_ZERO;/* first var number at line[1] */
        op = line[OP_POS];                    /* operator at line[2] */
        
        if (op == ECH_OP) {                    /* print out the variable */
            echo(vars, lens, opr1_index);
            continue;
        }
        
        /* do the calculation, second operand starts at line[3] */
        if (op == ASN_OP) {
            assign(vars, lens, opr1_index, line+OPR2_POS);
        } else if (op == ADD_OP) {
            add(vars, lens, opr1_index, line+OPR2_POS);
        } else if (op == MUL_OP) {
            multiply(vars, lens, opr1_index, line+OPR2_POS);
        } else if (op == POW_OP) {
            power(vars, lens, opr1_index, line+OPR2_POS);
        }
    }
    
    /* all done; take some rest */
    return 0;
}

/* read a line of input into the array passed as argument */
void
read_line(char *line, int max_len) {
    int i = 0, c;
    while (((c = getchar()) != EOF) && (c != '\n') && (c != '\r')) {
        if (i < max_len) {
            line[i++] = c;
        } else {
            printf("Invalid input line, toooooooo long.\n");
            exit(0);
        }
    }
    line[i] = '\0';
}

/* print out a huge integer */
void echo(huge_t vars[], int lens[], int opr1_index) {
    int i;
    
    /* print the digits in a reverse order */
    for (i = lens[opr1_index]-1; i >= 0; i--) {
        printf("%d", vars[opr1_index][i]);
    }
    
    printf("\n");
}

/****************************************************************/

/* add code below to complete the function bodies,
 * add more functions where appropriate.
 */

/* set the vars array to zeros */
void
init(huge_t vars[], int lens[]){
    int index;
    for (index = 0 ; index < NUM_VARS; index++){
        vars[index][FIRST_DIGIT] = DIGIT_ZERO;
        
        /* initialises  each variable's length to 1  */
        lens[index] = LEN_ONE;
    }
}

/* process the '=' operator */
void
assign(huge_t vars[], int lens[], int opr1_index, char *opr2_str){
    int opr2_len = 0, digit_diff = 0;
    huge_t temp;
    digit_t *opr2_int; // pointer to second operand's integer value
    
    /* finds out the second operand */
    int assign_itself = initialise(opr2_str, lens, temp, &opr2_len, vars,
                                   &digit_diff, opr1_index, &opr2_int, 1);
    
    if (!assign_itself){
        /*assigns each digit of operand 2 to operand 1*/
        lens[opr1_index] = opr2_len;
        
        clone(opr2_int, &vars[opr1_index][FIRST_DIGIT], opr2_len);
        
        remove_zeros (&lens[opr1_index], &vars[opr1_index][FIRST_DIGIT]);
    }
}

/* process the '+' operator */
void
add(huge_t vars[], int lens[], int opr1_index, char *opr2_str){
    /* adds each digit from operand 2 to operand 1 */
    int digit_diff = 0, opr2_len = 0;
    digit_t *opr2_int;
    huge_t temp;
    
    initialise(opr2_str, lens, temp,&opr2_len, vars, &digit_diff, opr1_index,
               &opr2_int, 0);
    
    add_loop(opr2_len, &lens[opr1_index], digit_diff,
             &vars[opr1_index][FIRST_DIGIT], opr2_int);
    
    remove_zeros(&lens[opr1_index], &vars[opr1_index][FIRST_DIGIT]);
}

/* process the '*' operator */
void
multiply(huge_t vars[], int lens[], int opr1_index, char *opr2_str){
    /* long multiplication algorithm */
    int digit_diff = 0, opr2_len = 0;
    digit_t *opr2_int;
    huge_t temp;
    
    initialise(opr2_str, lens, temp, &opr2_len, vars, &digit_diff, opr1_index,
               &opr2_int, 0);
    
    multiply_loop(opr2_len, opr2_int, &lens[opr1_index],
                  &vars[opr1_index][FIRST_DIGIT]);
    
    remove_zeros(&lens[opr1_index], &vars[opr1_index][FIRST_DIGIT]);
}

/* process the '^' operator */
void
power(huge_t vars[], int lens[], int opr1_index, char *opr2_str){
    /* multiplies operand 1 against operand 1 as much as (operand2 - 1) times */
    int digit_diff = 0, opr2_len = 0;
    digit_t *opr2_int;
    huge_t temp;
    
    initialise(opr2_str, lens, temp, &opr2_len, vars, &digit_diff, opr1_index,
               &opr2_int, 0);
    
    if (opr2_len == LEN_ONE && *opr2_int == DIGIT_ZERO){
        /* power of zero */
        lens[opr1_index] = LEN_ONE;
        vars[opr1_index][FIRST_DIGIT] = DIGIT_ONE;
        return;
    }
    
    /* clones operand 1 */
    huge_t clone_opr1;
    clone(&vars[opr1_index][FIRST_DIGIT], clone_opr1, lens[opr1_index]);
    
    huge_t k = {DIGIT_ONE};
    digit_t add[LEN_ONE] = {DIGIT_ONE};
    int k_len = LEN_ONE, add_len = LEN_ONE, clone_len= lens[opr1_index],
    index = 0;
    
    while (k_len != opr2_len){
        /* step 1 : multiplies operand 1 against itself until both lengths
         are equal */
        digit_diff =  check_digit_difference(k_len, add_len);
        add_loop(add_len, &k_len, digit_diff, k, add);
        multiply_loop(clone_len, &clone_opr1[FIRST_DIGIT], &lens[opr1_index],
                      &vars[opr1_index][FIRST_DIGIT]);
    }
    
    for (index = (k_len - 1); index >= 0; index--){
        /* step 2 : multiplies operand 1 against itself until k is equivalent
         to operand 2 */
        while (k[index] != opr2_int[index]){
            add_loop(add_len, &k_len, digit_diff, k, add);
            multiply_loop(clone_len, &clone_opr1[FIRST_DIGIT],
                          &lens[opr1_index], &vars[opr1_index][FIRST_DIGIT]);
        }
    }
    
    remove_zeros(&lens[opr1_index], &vars[opr1_index][FIRST_DIGIT]);
}

/* Added functions */
void
multiply_digit(digit_t *one_digit, digit_t *many_digits, int position_index,
               int many_digits_len, digit_t result[], int *result_len){
    int temp_len = 0, digit_diff, index;
    huge_t temp;
    digit_t prev_excess = 0, quotient = 0, carry = 0;
    
    /* multiplication by 10 according to one_digit's index */
    for (index = 0; index < position_index && index < INT_SIZE; index++){
        temp[index] = DIGIT_ZERO;
        temp_len++;
    }
    
    /* multiplies the one with many digits by a single digit */
    for (index = 0; index < many_digits_len && temp_len < INT_SIZE;
         index++){
        digit_t tmp = many_digits[index] * (*one_digit);
        digit_t remainder = tmp % OFFSET;
        quotient = tmp / OFFSET;
        carry = (remainder + prev_excess) / OFFSET;
        temp[temp_len++] = (remainder + prev_excess) % OFFSET;
        prev_excess = quotient + carry;
    }
    
    if (prev_excess && temp_len < INT_SIZE){
        /* reached the end of many_digits, but still got excess */
        temp[temp_len++] = prev_excess;
    }
    
    if (!position_index){
        /* initialises result because it's the very first multiplication */
        clone(temp, result, temp_len);
        *result_len = temp_len;
    }
    
    else {
        /* accumulates into result array */
        digit_diff = check_digit_difference(*result_len, temp_len);
        add_loop(temp_len, result_len, digit_diff, result, temp);
    }
}

int
check_digit_difference(int opr1_len, int opr2_len){
    /* checks which variable has more number of digits */
    return (opr1_len - opr2_len);
}

int
text_to_array(char *opr2_str, digit_t A[]){
    /* collects the numbers entered into an array with reversed format */
    int max, counter;
    max = counter = strlen(opr2_str);
    
    for (; *opr2_str ; opr2_str++){
        counter --;
        A[counter] = (digit_t)(*opr2_str - CH_ZERO);
    }
    
    return max;
}

void
clone(digit_t *opr1, digit_t A[], int max_len){
    /* clones operand 1 */
    int index;
    
    for(index = 0; index < max_len; index++){
        A[index] = opr1[index];
    }
}

void
add_loop(int opr2_len, int *opr1_len, int digit_diff, digit_t *opr1,
         digit_t *opr2){
    /* adds each digit from operand 2 to operand 1 */
    int counter;
    digit_t excess = 0;
    
    for (counter = 0; counter < opr2_len && counter < *opr1_len;
         counter++){
        /* loops to the edge of operand 1 or operand 2 and adds up their
         corresponding digits */
        opr1[counter] += excess;
        excess = (opr1[counter] + opr2[counter]) / OFFSET;
        opr1[counter] = (opr1[counter] + opr2[counter]) % OFFSET;
    }
    
    /* adds the excess into the next index of operand 1 */
    
    if (digit_diff < 0){
        for (; counter < opr2_len; counter++){
            /* operand 1 has less number of digits than operand 2 */
            digit_t tmp = opr2[counter] + excess;
            excess = tmp / OFFSET;
            opr1[counter] = tmp % OFFSET;
            *opr1_len += 1;
            
            if (excess && (counter + 1) == opr2_len &&
                (counter + 1) < INT_SIZE){
                /* reached the end of operand 2 and still have excess */
                opr1[counter + 1] = excess;
                *opr1_len += 1;
            }
        }
    }
    else if (digit_diff > 0 && excess){
        /* operand 1 has more digits than operand 2 */
        while (excess){
            /* keeps adding excess into corresponding index of operand 1 */
            digit_t tmp = opr1[counter] + excess;
            excess = tmp / OFFSET;
            opr1[counter] = tmp % OFFSET;
            counter++;
            if (excess && counter == *opr1_len && counter < INT_SIZE){
                /* reached the end of operand 1 and still have excess */
                opr1[counter] = excess;
                *opr1_len += 1;
                excess = 0;
            }
        }
        
    }
    else if (!(digit_diff) && excess && counter < INT_SIZE){
        /* operand 1 and operand 2 have the same number of digits */
        opr1[counter] = excess;
        *opr1_len += 1;
    }
}

int
initialise(char *opr2_str, int lens[], digit_t temp[], int *opr2_len,
           huge_t vars[], int *digit_diff, int opr1_index, digit_t **opr2_int,
           int is_assign){
    /* checks if operand 2 is in vars */
    int is_variable = (*opr2_str == VAR_NOTATION);
    int var_index;
    
    if (is_variable){
        var_index = opr2_str[1] - CH_ZERO;
        
        if (var_index == opr1_index && !is_assign){
            /* operand2 is the same as operand 1,
             creates local copy at temp array */
            *opr2_len = lens[opr1_index];
            clone(&vars[var_index][FIRST_DIGIT], temp, *opr2_len);
            *opr2_int = &temp[FIRST_DIGIT];
        }
        
        else if (var_index == opr1_index && is_assign){
            /* assign function assigns operand 1 to itself */
            return THE_SAME;
        }
        
        else{
            /* points to the value of operand 2 */
            *opr2_len = lens[var_index];
            *opr2_int = &vars[var_index][FIRST_DIGIT];
        }
    }
    
    else{
        /* converts characters into integers */
        *opr2_len = text_to_array(opr2_str, temp);
        *opr2_int = &temp[FIRST_DIGIT];
    }
    
    *digit_diff = check_digit_difference(lens[opr1_index], *opr2_len);
    return 0;
}

void
remove_zeros(int *len, digit_t *current_array){
    /* checks for any leading zeros and removes them */
    int index = (*len - 1), zero_trigger = 0, zero_counter = 0;
    
    if (current_array[index] == DIGIT_ZERO){
        for (; index >= 0; index--){
            /* identifies and counts the number of leading zeros */
            zero_trigger = (current_array[index] == DIGIT_ZERO);
            
            /* counts only when it's proven to be a leading zero */
            if (zero_trigger){
                zero_counter += zero_trigger;
            }
            
            /* no longer a leading zero */
            else {
                break;
            }
        }
        /* ignores leading zero during echo */
        *len = (*len) - zero_counter;
        if (*len == 0){
            /* the whole array is filled with 0s */
            *len = LEN_ONE;
            *current_array = DIGIT_ZERO;
        }
    }
}

void
multiply_loop(int opr2_len, digit_t *opr2_int, int *opr1_len, digit_t *opr1){
    /* long multiplication algorithm */
    int index, result_len;
    huge_t result;
    
    for (index = 0; index < opr2_len; index++){
        /* multiplies each digit of operand 2 to operand 1, then accumulate
         the result */
        multiply_digit((opr2_int+index), opr1, index, *opr1_len, result,
                       &result_len);
    }
    
    /* put final result back into operand 1 */
    clone(result, opr1, result_len);
    *opr1_len = result_len;
    
}



/* Algorithms are fun */
