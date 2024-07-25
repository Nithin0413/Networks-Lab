#include <stdio.h>
#include <stdlib.h>

int main()
{
    char* input = "124x64";
    char operator;
    long int num1, num2;
    
    // read the first number
    num1 = strtol(input, &input, 10);
    
    // read the operator
    operator = *input;
    input++;
    
    // read the second number
    num2 = strtol(input, &input, 10);
    
    // perform operation based on operator
    switch (operator) {
        case '+':
            printf("%ld + %ld = %ld\n", num1, num2, num1 + num2);
            break;
        case '-':
            printf("%ld - %ld = %ld\n", num1, num2, num1 - num2);
            break;
        case '*':
            printf("%ld * %ld = %ld\n", num1, num2, num1 * num2);
            break;
        case '/':
            printf("%ld / %ld = %ld\n", num1, num2, num1 / num2);
            break;
        default:
            printf("Invalid operator '%c'\n", operator);
            return 1;
    }
    
    return 0;
}
