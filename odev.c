#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX_EXPR_SIZE 256
//TokenType enum is used for classification of the token types.
typedef enum {
    TOKEN_TYPE_NULLL,
    TOKEN_TYPE_NUMBER,
    TOKEN_TYPE_OPERATOR,
    TOKEN_TYPE_FUNCTION,
    TOKEN_TYPE_VARIABLE,
    TOKEN_TYPE_PARENTHESES,
    TOKEN_TYPE_COMMA
} TokenType;

//Token struct 
typedef struct {
    TokenType type;
    char value[257];
} Token;

// The aim of checkerToken array is to check if the expression should return "Error!" or not. It contains the raw-expression that is splitted by tokens. 
Token checkerToken[257];

// The postfix expression is kept in arrToken.
Token arrToken[256+1];

// Lookup arrays are for memorization of variables. Lookup array records the input as string and lookup_2 records integer value of that string.
char lookup[257][257];

// a is the index of arrToken
int a=0;

// b is the index of checkerToken
int b=0;

// this is flag that we use in possible error places. This is not a specific flag
int bigCheck=0;

int naming = 1;
int number_of_lines_errors=0;
int most_general_error_flag=0;

int withequal = 0;
// indicates the precedence of operators
int precedence(char operator)
{
    switch (operator) {
    case '+':
    case '-':
        return 1;
    case '*':
    case '/':
        return 2;
    case '&':
    case '|':
        return 0;
    
    default:
        return -1;
    }
}
// indicates operators
int isOperator(char ch)
{
    return (ch == '+' || ch == '-' || ch == '*' || ch == '&' || ch == '|' || ch == '/' || ch == '%');
}
// converts infix epression to postfix expression
int infixToPostfix(char* infix)
{
    a=0;
    bigCheck=0;
    //initializing arrToken and checkerToken
    for(int i=0;i<257;i++){
        arrToken[i].type = TOKEN_TYPE_NULLL;
        checkerToken[i].type = TOKEN_TYPE_NULLL;

        strcpy(arrToken[i].value,"");
        strcpy(checkerToken[i].value,"");

    }
    // stackToken includes operators, functions and parantheses  
    Token stackToken[256+1];
    // funcStack shows which function is the last one
    Token funcStack[257];
    // index for funcStack
    int funcCounter=0;

    int i=0, j=0;
    int len = strlen(infix);
    int top = -1;
    //printf("%s",infix);
    //printf(" ");
    //iterating through chars of infix expression
    while(i<len-1){
        //printf("%d",i);
        //printf(" ");

        //skipping blankspaces
        /*if(i==57 && number_of_lines_errors == 15){
            number_of_lines_errors++;
            printf("%c",infix[i]);
            printf("%c",infix[i+1]);
            printf("%c",infix[i+2]);
            printf("%c",infix[i+3]);
            printf("%c",infix[i+4]);

        }*/
        if (infix[i] == ' ' || infix[i] == '\t'){
            
            i++;
            continue;
        }
        Token token;
        
        //checking if an incoming char is digit and if so tokenizes it. Then, adding this token to arrToken and checkerToken.  
        if (isdigit(infix[i])) {
            token.type = TOKEN_TYPE_NUMBER;
            int k = 0;
            while(isdigit(infix[i])){
                token.value[k]=infix[i];
                k++;
                i++;
            }
            token.value[k] = '\0';
            arrToken[a++] = token;
            checkerToken[b++] = token;
            continue;
        }

        //checking if an incoming char is alphabetic and if so tokenizes it. Then, adding this token to arrToken and checkerToken.  
        else if (isalpha(infix[i])) {

            int k = 0;
            while(isalpha(infix[i])){
                token.value[k]=infix[i];
                k++;
                i++;
            }
            token.value[k] = '\0';
            //checking if the token is function or not, then adding into funcStack.
            if(strcmp(token.value, "xor") == 0 || strcmp(token.value, "ls") == 0||strcmp(token.value, "rs") == 0||strcmp(token.value, "lr") == 0||strcmp(token.value, "rr") == 0||strcmp(token.value, "not") == 0 ){
                token.type = TOKEN_TYPE_FUNCTION;
                funcStack[funcCounter++] = token;
                
            }else{ // if it is not a function, then it must be a variable. 
                token.type = TOKEN_TYPE_VARIABLE;
                arrToken[a++] = token;
            }
            checkerToken[b++] = token;
            continue;
        }

        //if the incoming char is comma, initially tokenizes it, then looks at the last function and add the operator of it to the stackToken.  
        // This is important part for the placement of the function (except not) operator to stack and postfix. 
        else if(infix[i] == ','){
            token.type = TOKEN_TYPE_COMMA;
            token.value[0] = ',';
            checkerToken[b++] = token;
            if(funcCounter==0){
                bigCheck=1;
                break;
            }
            if(strcmp(funcStack[funcCounter-1].value,"xor")==0){

                while (top > -1 && precedence(stackToken[top].value[0]) >= precedence('^') && stackToken[top].type != TOKEN_TYPE_PARENTHESES){
                    arrToken[a++] = stackToken[top--];
                }
                stackToken[++top].type = TOKEN_TYPE_OPERATOR;
                strcpy(stackToken[top].value,"^");                
            }else if(strcmp(funcStack[funcCounter-1].value,"ls")==0){

                while (top > -1 && precedence(stackToken[top].value[0]) >= -1 && stackToken[top].type != TOKEN_TYPE_PARENTHESES){
                    arrToken[a++] = stackToken[top--];
                }
                stackToken[++top].type = TOKEN_TYPE_OPERATOR;
                strcpy(stackToken[top].value,"<<");                
            }else if(strcmp(funcStack[funcCounter-1].value,"rs")==0){

                while (top > -1 && precedence(stackToken[top].value[0]) >= -1 && stackToken[top].type != TOKEN_TYPE_PARENTHESES){
                    arrToken[a++] = stackToken[top--];
                }
                stackToken[++top].type = TOKEN_TYPE_OPERATOR;
                strcpy(stackToken[top].value,">>");                
            }else if(strcmp(funcStack[funcCounter-1].value,"lr")==0){

                while (top > -1 && precedence(stackToken[top].value[0]) >= -1 && stackToken[top].type != TOKEN_TYPE_PARENTHESES){
                    arrToken[a++] = stackToken[top--];
                }
                stackToken[++top].type = TOKEN_TYPE_OPERATOR;
                strcpy(stackToken[top].value,"lr");                
            }else if(strcmp(funcStack[funcCounter-1].value,"rr")==0){

                while (top > -1 && precedence(stackToken[top].value[0]) >= -1 && stackToken[top].type != TOKEN_TYPE_PARENTHESES){
                    arrToken[a++] = stackToken[top--];
                }
                stackToken[++top].type = TOKEN_TYPE_OPERATOR;
                strcpy(stackToken[top].value,"rr");                
            }
            funcCounter--;
            i++;
            continue;
        }
       
        // if the character is '(' push it in the stackToken
        else if (infix[i] == '(') {
            token.type = TOKEN_TYPE_PARENTHESES;
            token.value[0]=infix[i];
            i++;
            token.value[i] = '\0';
            stackToken[++top]=token; 
            checkerToken[b++]=token;
            //if the last function is "not", execute not operation for postfix
            if(strcmp(funcStack[funcCounter-1].value,"not")==0){
                stackToken[++top].type = TOKEN_TYPE_OPERATOR;
                strcpy(stackToken[top].value,"~");
                funcCounter--;
            }
            continue;
        }
       
        // if the character is ')'
        // pop the stack and add it to the strcmp(stackToken[top].value[0], '(')!=0
        // output string until empty or '(' found
        else if (infix[i] == ')') {
            checkerToken[b].type=TOKEN_TYPE_PARENTHESES;
            strcpy(checkerToken[b++].value,")");
            while (top > -1 && stackToken[top].type != TOKEN_TYPE_PARENTHESES ){
                arrToken[a++] = stackToken[top--];
            }
            if (top > -1 && stackToken[top].type != TOKEN_TYPE_PARENTHESES ){
                bigCheck=1;
                
            }
            else{
                top--;
            }
            i++;
            continue;
        }
       
        // checking if the char is operator
        // push it to the stack after considering the precedence

        else if (isOperator(infix[i])) {
            token.type = TOKEN_TYPE_OPERATOR;
            token.value[0] = infix[i];
            token.value[1] = '\0';
            checkerToken[b++] = token;
            while (top > -1
                   && precedence(stackToken[top].value[0])
                          >= precedence(infix[i]))
                arrToken[a++] = stackToken[top--];
            stackToken[++top] = token;
            i++;
            continue;
        }

        else{
           
            bigCheck=1;
            break;
        }
        printf("%d",number_of_lines_errors);
        printf(" ");
        printf("%d",i);
        printf("   ");
    }
    
    // checking if the parantheses are balanced 
    while (top > -1) {
        if (stackToken[top].type == TOKEN_TYPE_PARENTHESES) {
            bigCheck=1;
        }
        arrToken[a++] = stackToken[top--];
    }
    
    return 0;
}
// big function for checking errors
int checkFunc(){
    // initializing parameters or flags to be used
    int isEqualParantheses=0;
    int count_func_with_comma=0;
    int comma_count=0;
    for(int k=0;k<b;k++){
        // two seperate number cannot be placed next to each other
        if(checkerToken[k].type == TOKEN_TYPE_NUMBER){
            if(checkerToken[k+1].type == TOKEN_TYPE_NUMBER){
                return 1;
            }else if(checkerToken[k+1].type == TOKEN_TYPE_VARIABLE){
                return 1;
            }
        }
        // two seperate variable cannot be placed next to each other
        if(checkerToken[k].type == TOKEN_TYPE_VARIABLE){
            if(checkerToken[k+1].type == TOKEN_TYPE_NUMBER){
                return 1;
            }else if(checkerToken[k+1].type == TOKEN_TYPE_VARIABLE){
                return 1;
            }
        }
        // two operator number cannot be placed next to each other
        if(checkerToken[k].type == TOKEN_TYPE_OPERATOR){
            if(checkerToken[k+1].type == TOKEN_TYPE_OPERATOR){
                return 1;
            }
        }
        //if the token is function
        //the next token should be left parantheses
        if(checkerToken[k].type == TOKEN_TYPE_FUNCTION){
            if(checkerToken[k+1].value[0] != '('){
                return 1;
            }
            
            if(strcmp(checkerToken[k].value,"not")!=0){
                count_func_with_comma++;
                int existingparantheses = isEqualParantheses;
                int tempParant = existingparantheses;
                int num=0;
                for(int p=k+1;;p++){
                    if(p==b){
                        return 1;
                    }
                    if(checkerToken[p].type ==TOKEN_TYPE_PARENTHESES && checkerToken[p].value[0]=='('){
                        tempParant++;
                    }
                    if(checkerToken[p].type ==TOKEN_TYPE_PARENTHESES && checkerToken[p].value[0]==')'){
                        tempParant--;
                        if(tempParant == existingparantheses){
                            break;
                        }
                        
                    }

                    if(tempParant==existingparantheses+1 && checkerToken[p].type ==TOKEN_TYPE_COMMA){
                        num++;
                    }
                    

                }
                if(num != 1){
                    return 1;
                }
            }

        }

        if(checkerToken[k].type == TOKEN_TYPE_PARENTHESES){
            if(checkerToken[k].value[0] == '('){
                isEqualParantheses++;
            }

            if(checkerToken[k].value[0] == ')'){
                
                isEqualParantheses--;
                // checking if the last function has more than one comma or not 
                // checking whether the part from the comma to last function is balanced
                                
            }
        }
        //if it is comma
        // checking whether the part from the last function to the comma is balanced
        if(checkerToken[k].type == TOKEN_TYPE_COMMA){
            comma_count++;
            
        }
        
        
    }
    // checking if the parantheses are balanced
    if(isEqualParantheses != 0){
        return 1;
    }
    // checking if the comma count is the same with the functions other than "not" 
    // since "not" does not have comma
    if(comma_count!=count_func_with_comma){
        return 1;
    }
    return 0;
}

//function for checking if the given variable is in the lookup array
int isInsideLookup(char arr[]){
    
    for(int m=0;m<257;m++){
        if(strcmp(lookup[m], "")!=0 && strcmp(lookup[m], arr)==0){
            return m;
            break;
        }
    }
    return -1;
}
// This function calculates the postfix notation coming from infixToPostfix function
int evaluatePostfix(FILE *fp)
{
    // index for tstack
    int toptstack=0;
    
    // initializing stack to evaluate postfix
    Token tstack[256];
    for(int l=0;l<256;l++){ 
        tstack[l].type = TOKEN_TYPE_NULLL;
        strcpy(tstack[l].value,"");
    }
    //char* res = (char*) calloc(257, sizeof(char));
    char res[300];
    if(!(a==1 && arrToken[0].type == TOKEN_TYPE_NUMBER)){
        strcpy(res,"%%");
    }else{
        strcpy(res,"");
    }
    //iterating in postfix notation
    for (int i = 0; i< a; i++)
    {
        // if the type of the token is operator, then pop the last two element from tstak
        // evaluate the result with these values and operator
        // then, push this result back to the tstack
        if(arrToken[i].type == TOKEN_TYPE_OPERATOR){
            char op_val[257];
            strcpy(op_val, arrToken[i].value);
            Token tok1 = tstack[--toptstack];
            
            char outcome[257]="";
            if(strcmp(op_val,"~")==0){
                // NOT
                fprintf(fp,"%%");
                    fprintf(fp,"%d",naming);
                    fprintf(fp," = xor i32 ");
                    if(tok1.type == TOKEN_TYPE_VARIABLE){
                        fprintf(fp,"%%");
                    }
                    fprintf(fp,"%s",tok1.value);
                    fprintf(fp,",");
                    fprintf(fp,"%d",-1);
                    fprintf(fp,"\n");
            }else{
                Token tok2 = tstack[--toptstack];
                
                if(strcmp(op_val,"+")==0){
                    fprintf(fp,"%%");
                    fprintf(fp,"%d",naming);
                    fprintf(fp," = add i32 ");
                    if(tok2.type == TOKEN_TYPE_VARIABLE){
                        fprintf(fp,"%%");
                    }
                    fprintf(fp,"%s",tok2.value);
                    fprintf(fp,",");
                    if(tok1.type == TOKEN_TYPE_VARIABLE){
                        fprintf(fp,"%%");
                    }
                    fprintf(fp,"%s",tok1.value);
                    fprintf(fp,"\n");

                }else if(strcmp(op_val,"-")==0){
                    fprintf(fp,"%%");
                    fprintf(fp,"%d",naming);
                    fprintf(fp," = sub i32 ");
                    if(tok2.type == TOKEN_TYPE_VARIABLE){
                        fprintf(fp,"%%");
                    }
                    fprintf(fp,"%s",tok2.value);
                    fprintf(fp,",");
                    if(tok1.type == TOKEN_TYPE_VARIABLE){
                        fprintf(fp,"%%");
                    }
                    fprintf(fp,"%s",tok1.value);
                    fprintf(fp,"\n");

                }else if(strcmp(op_val,"*")==0){
                    fprintf(fp,"%%");
                    fprintf(fp,"%d",naming);
                    fprintf(fp," = mul i32 ");
                    if(tok2.type == TOKEN_TYPE_VARIABLE){
                        fprintf(fp,"%%");
                    }
                    fprintf(fp,"%s",tok2.value);
                    fprintf(fp,",");
                    if(tok1.type == TOKEN_TYPE_VARIABLE){
                        fprintf(fp,"%%");
                    }
                    fprintf(fp,"%s",tok1.value);
                    fprintf(fp,"\n");

                }else if(strcmp(op_val,"/")==0){
                    fprintf(fp,"%%");
                    fprintf(fp,"%d",naming);
                    fprintf(fp," = sdiv i32 ");
                    if(tok2.type == TOKEN_TYPE_VARIABLE){
                        fprintf(fp,"%%");
                    }
                    fprintf(fp,"%s",tok2.value);
                    fprintf(fp,",");
                    if(tok1.type == TOKEN_TYPE_VARIABLE){
                        fprintf(fp,"%%");
                    }
                    fprintf(fp,"%s",tok1.value);
                    fprintf(fp,"\n");

                }else if(strcmp(op_val,"%")==0){
                    fprintf(fp,"%%");
                    fprintf(fp,"%d",naming);
                    fprintf(fp," = srem i32 ");
                    if(tok2.type == TOKEN_TYPE_VARIABLE){
                        fprintf(fp,"%%");
                    }
                    fprintf(fp,"%s",tok2.value);
                    fprintf(fp,",");
                    if(tok1.type == TOKEN_TYPE_VARIABLE){
                        fprintf(fp,"%%");
                    }
                    fprintf(fp,"%s",tok1.value);
                    fprintf(fp,"\n");

                }else if(strcmp(op_val,"&")==0){
                    fprintf(fp,"%%");
                    fprintf(fp,"%d",naming);
                    fprintf(fp," = and i32 ");
                    if(tok2.type == TOKEN_TYPE_VARIABLE){
                        fprintf(fp,"%%");
                    }
                    fprintf(fp,"%s",tok2.value);
                    fprintf(fp,",");
                    if(tok1.type == TOKEN_TYPE_VARIABLE){
                        fprintf(fp,"%%");
                    }
                    fprintf(fp,"%s",tok1.value);
                    fprintf(fp,"\n");

                }else if(strcmp(op_val,"|")==0){
                    fprintf(fp,"%%");
                    fprintf(fp,"%d",naming);
                    fprintf(fp," = or i32 ");
                    if(tok2.type == TOKEN_TYPE_VARIABLE){
                        fprintf(fp,"%%");
                    }
                    fprintf(fp,"%s",tok2.value);
                    fprintf(fp,",");
                    if(tok1.type == TOKEN_TYPE_VARIABLE){
                        fprintf(fp,"%%");
                    }
                    fprintf(fp,"%s",tok1.value);
                    fprintf(fp,"\n");
                }else if(strcmp(op_val,"^")==0){
                    fprintf(fp,"%%");
                    fprintf(fp,"%d",naming);
                    fprintf(fp," = xor i32 ");
                    if(tok2.type == TOKEN_TYPE_VARIABLE){
                        fprintf(fp,"%%");
                    }
                    fprintf(fp,"%s",tok2.value);
                    fprintf(fp,",");
                    if(tok1.type == TOKEN_TYPE_VARIABLE){
                        fprintf(fp,"%%");
                    }
                    fprintf(fp,"%s",tok1.value);
                    fprintf(fp,"\n");

                }else if(strcmp(op_val,">>")==0){
                    fprintf(fp,"%%");
                    fprintf(fp,"%d",naming);
                    fprintf(fp," = ashr i32 ");
                    if(tok2.type == TOKEN_TYPE_VARIABLE){
                        fprintf(fp,"%%");
                    }
                    fprintf(fp,"%s",tok2.value);
                    fprintf(fp,",");
                    if(tok1.type == TOKEN_TYPE_VARIABLE){
                        fprintf(fp,"%%");
                    }
                    fprintf(fp,"%s",tok1.value);
                    fprintf(fp,"\n");
                }else if(strcmp(op_val,"<<")==0){
                    fprintf(fp,"%%");
                    fprintf(fp,"%d",naming);
                    fprintf(fp," = shl i32 ");
                    if(tok2.type == TOKEN_TYPE_VARIABLE){
                        fprintf(fp,"%%");
                    }
                    fprintf(fp,"%s",tok2.value);
                    fprintf(fp,",");
                    if(tok1.type == TOKEN_TYPE_VARIABLE){
                        fprintf(fp,"%%");
                    }
                    fprintf(fp,"%s",tok1.value);
                    fprintf(fp,"\n");

                }else if(strcmp(op_val,"lr")==0){
                    fprintf(fp, "%%");
                    fprintf(fp, "%d", naming);
                    fprintf(fp, " = srem i32 ");
                    if (tok1.type == TOKEN_TYPE_VARIABLE) {
                        fprintf(fp, "%%");
                    }
                    fprintf(fp, "%s", tok1.value);
                    fprintf(fp,", 32");
                    fprintf(fp, "\n");
                    naming++;

                    // Compute the shifted value
                    fprintf(fp, "%%");
                    fprintf(fp, "%d", naming);
                    fprintf(fp," = shl i32 ");
                    if(tok2.type == TOKEN_TYPE_VARIABLE){
                        fprintf(fp,"%%");
                    }
                    fprintf(fp,"%s",tok2.value);
                    fprintf(fp,",");
                    fprintf(fp,"%%");
                    fprintf(fp,"%d",naming-1);
                    
                    fprintf(fp,"\n");
                    naming++;

                    fprintf(fp,"%%");
                    fprintf(fp,"%d",naming);
                    fprintf(fp," = sub i32 32, ");
                    
                    if(tok1.type == TOKEN_TYPE_VARIABLE){
                        fprintf(fp,"%%");
                    }
                    fprintf(fp,"%s",tok1.value);
                    fprintf(fp,"\n");

                    naming++;
                    fprintf(fp,"%%");
                    fprintf(fp,"%d",naming);
                    fprintf(fp," = srem i32 ");

                    fprintf(fp,"%%");
                    fprintf(fp,"%d",naming-1);
                    fprintf(fp,",");
                    
                    fprintf(fp," 32");
                    fprintf(fp,"\n");
                    naming++;

                    fprintf(fp,"%%");
                    fprintf(fp,"%d",naming);
                    fprintf(fp," = ashr i32 ");
                    if(tok2.type == TOKEN_TYPE_VARIABLE){
                        fprintf(fp,"%%");
                    }
                    fprintf(fp,"%s",tok2.value);
                    fprintf(fp,", ");
                    fprintf(fp,"%%");
                    fprintf(fp,"%d",naming-1);
                    fprintf(fp,"\n");

                    naming++;

                    fprintf(fp,"%%");
                    fprintf(fp,"%d",naming);
                    fprintf(fp," = or i32 ");
                    fprintf(fp, "%%");
                    fprintf(fp, "%d", naming-1);
                    fprintf(fp,", ");
                    fprintf(fp, "%%");
                    fprintf(fp, "%d", naming-4);
                    
                    fprintf(fp,"\n");

                    //outcome = (val2 << val1%32) | (val2 >> (32-val1)%32);
                }else if(strcmp(op_val,"rr")==0){
                    fprintf(fp, "%%");
                    fprintf(fp, "%d", naming);
                    fprintf(fp, " = srem i32 ");
                    if (tok1.type == TOKEN_TYPE_VARIABLE) {
                        fprintf(fp, "%%");
                    }
                    fprintf(fp, "%s", tok1.value);
                    fprintf(fp,", 32");
                    fprintf(fp, "\n");
                    naming++;

                    // Compute the shifted value
                    fprintf(fp, "%%");
                    fprintf(fp, "%d", naming);
                    fprintf(fp," = ashr i32 ");
                    if(tok2.type == TOKEN_TYPE_VARIABLE){
                        fprintf(fp,"%%");
                    }
                    fprintf(fp,"%s",tok2.value);
                    fprintf(fp,",");
                    fprintf(fp,"%%");
                    fprintf(fp,"%d",naming-1);
                    
                    fprintf(fp,"\n");
                    naming++;

                    fprintf(fp,"%%");
                    fprintf(fp,"%d",naming);
                    fprintf(fp," = sub i32 32, ");
                    
                    if(tok1.type == TOKEN_TYPE_VARIABLE){
                        fprintf(fp,"%%");
                    }
                    fprintf(fp,"%s",tok1.value);
                    fprintf(fp,"\n");

                    naming++;
                    fprintf(fp,"%%");
                    fprintf(fp,"%d",naming);
                    fprintf(fp," = srem i32 ");

                    fprintf(fp,"%%");
                    fprintf(fp,"%d",naming-1);
                    fprintf(fp,",");
                    
                    fprintf(fp," 32");
                    fprintf(fp,"\n");
                    naming++;

                    fprintf(fp,"%%");
                    fprintf(fp,"%d",naming);
                    fprintf(fp," = shl i32 ");
                    if(tok2.type == TOKEN_TYPE_VARIABLE){
                        fprintf(fp,"%%");
                    }
                    fprintf(fp,"%s",tok2.value);
                    fprintf(fp,", ");
                    fprintf(fp,"%%");
                    fprintf(fp,"%d",naming-1);
                    fprintf(fp,"\n");

                    naming++;

                    fprintf(fp,"%%");
                    fprintf(fp,"%d",naming);
                    fprintf(fp," = or i32 ");
                    fprintf(fp, "%%");
                    fprintf(fp, "%d", naming-1);
                    fprintf(fp,", ");
                    fprintf(fp, "%%");
                    fprintf(fp, "%d", naming-4);
                    
                    fprintf(fp,"\n");

                    //outcome = (val2 >> val1%32) | (val2 << (32-val1)%32);
                }
            }
            
            tstack[toptstack].type=TOKEN_TYPE_VARIABLE;
            sprintf(tstack[toptstack].value,"%d",naming);
            naming++;
            //strcpy(tstack[toptstack].value, naming);
            toptstack++;
        // if the type of the token is number
        // directly push it to the tstack
        }else if(arrToken[i].type == TOKEN_TYPE_NUMBER){
            tstack[toptstack].type = TOKEN_TYPE_NUMBER;
            
            strcpy(tstack[toptstack].value,arrToken[i].value);
            toptstack++;
        // if the type of the token is variable
        // look at the lookup tables to find the int value of that variable
        // push this int value to the tstack
        }else if(arrToken[i].type == TOKEN_TYPE_VARIABLE){
            int flagifInsideLookUp = 0;
            for(int m=0;m<257;m++){
                if(strcmp(lookup[m], "")!=0 && strcmp(lookup[m], arrToken[i].value)==0){
                    flagifInsideLookUp = 1;
                    break;
                }
            }
            if(flagifInsideLookUp==0){
                printf("Error on line ");
                printf("%d",number_of_lines_errors);
                printf("!\n");
                most_general_error_flag = 1;
            }
            fprintf(fp,"%%");
            fprintf(fp,"%d",naming);
            fprintf(fp," = load i32, i32* ");
            fprintf(fp,"%%");
            fprintf(fp,"%s",arrToken[i].value);
            fprintf(fp,"\n");
            
            tstack[toptstack].type = TOKEN_TYPE_VARIABLE;
            
            sprintf(tstack[toptstack].value,"%d",naming);
            
            naming++;
            toptstack++;
        }
                
    }
    
    strcat(res, tstack[toptstack-1].value);
    if(withequal==0){
        fprintf(fp,"call i32 (i8*, ...) @printf(i8* getelementptr ([4 x i8], [4 x i8]* @print.str, i32 0, i32 0), i32 ");
        naming++;
        fprintf(fp, "%s",res);
        fprintf(fp," )");
        fprintf(fp,"\n");
    }else{
        fprintf(fp,"store i32 ");
        fprintf(fp, "%s",res);
        fprintf(fp,", ");
        fprintf(fp,"i32* ");
        fprintf(fp,"%%");
    }
    
    
    return 0;
}
int main(char *argv[])
{
    char *input_filename = argv[1];
    int ext_index = 0;
    while (input_filename[ext_index] != '.' && input_filename[ext_index] != '\0') {
        ext_index++;
    }
    char output_filename[ext_index + 4]; // Add space for ".ll"
    strncpy(output_filename, input_filename, ext_index);
    output_filename[ext_index] = '\0'; // Add null terminator
    strcat(output_filename, ".ll");


    FILE *readfp = fopen(input_filename, "r");
    if (readfp == NULL) {
        printf("Error opening file\n");
        exit(1);
    }
    FILE *fp = fopen(output_filename, "w");
    if (fp == NULL) {
        printf("Failed to open file\n");
        return 1;
    }
    
    fprintf(fp,"; ModuleID = 'advcalc2ir'\n");
    fprintf(fp,"declare i32 @printf(i8*, ...)\n");
    fprintf(fp, "@print.str = constant [4 x i8] c\"%%d\\0A\\00\"\n");
    fprintf(fp,"define i32 @main() {\n");

    //initializing the lookup arrays
    for(int k=0;k<257;k++){
        strcpy(lookup[k],"");
    }
    //index for lookup arrays
    int index_of_lookup=0;
    char line[256 +1] = "";
    while(fgets(line,sizeof(line),readfp)){
        number_of_lines_errors++;
        withequal=0;
        // This part enables to the code to continue if the input is blank line
        int spaceFlag=1;
        for(int j=0;line[j]!='\n';j++){
            if(!isspace(line[j])){
                spaceFlag=0;
                break;
            }
        }
        if(spaceFlag==1){
            
            continue;
        }
        // if the line does not include '='
        // execute the infixToPostfix and evaluatePostfix functions respectively.
        if(strchr(line,'=') == NULL){
            withequal=0;

            infixToPostfix(line);
            
            int check_number = checkFunc();
            if(check_number==1 || bigCheck==1){
                printf("Error on line ");
                printf("%d",number_of_lines_errors);
                printf("!\n");
                most_general_error_flag = 1;
                continue;
            }
            evaluatePostfix(fp);
            /*fprintf(fp,"call i32 (i8*, ...) @printf(i8* getelementptr ([4 x i8], [4 x i8]* @print.str, i32 0, i32 0), i32 ");
            naming++;
            fprintf(fp,"%s",toStoreRes);
            fprintf(fp," )");
            fprintf(fp,"\n");
            */
        
            
        }else{
        // splitting the line by =
        
            withequal=1;
            char temp[257];
            strcpy(temp,line);

            
            char* token = strtok(temp, "=");
           
            char* value = strtok(NULL, "=");
            char token_array[257] = "";
            
            // checking if the left side of = has any errors
            int hasError = 0;
            int my_flag=0;
            int i=0;
            while(token[i]!='\0'){
                
                if (isspace(token[i])) {
                    // skip whitespace characters
                    
                    i++;
                    continue;
                }
                
                if (!isalpha(token[i])){
                    hasError=1;
                    i++;
                }else {
                    if (my_flag == 1){
                        hasError=1;
                    }
                    int k = 0;
                    my_flag=1;
                    while(isalpha(token[i])){
                        token_array[k]=token[i];
                        k++;
                        i++;
                        
                    }
                    token_array[k] = '\0';
                }                 
                
            }
            if(hasError == 1){
                printf("Error on line ");
                printf("%d",number_of_lines_errors);
                printf("!\n");
                most_general_error_flag = 1;
                continue;
            }
            
            int ifInside = isInsideLookup(token_array);
            
            // calculate the right side by sending it to the infixToPostfix and evaluatePostfix functions respectively.
            infixToPostfix(value);
            int check_number = checkFunc();
            if(check_number==1 || bigCheck==1){
                printf("Error on line ");
                printf("%d",number_of_lines_errors);
                printf("!\n");
                most_general_error_flag = 1;
                continue;
            }
            if(ifInside == -1){
                fprintf(fp,"%%");
                fprintf(fp,"%s",token_array);
                fprintf(fp," = alloca i32");
                fprintf(fp,"\n");
            }/*else{

            }*/
            evaluatePostfix(fp);
            /*fprintf(fp,"store i32 ");
            fprintf(fp,"%s",toStoreRes);
            fprintf(fp,", ");
            fprintf(fp,"i32* ");
            fprintf(fp,"%%");
            fprintf(fp,token_array);
            fprintf(fp,"\n");*/
            fprintf(fp,"%s",token_array);
            fprintf(fp,"\n");
            // if the variable is not in lookup array
            // assign the variable and its value to lookup arrays separately
            if(ifInside==-1){
                strcpy(lookup[index_of_lookup],token_array);
                           
                index_of_lookup++;
            }
                                    
        }    
        strcpy(line,"");

    }
    fprintf(fp,"ret i32 0}");

    fclose(fp);
    fclose(readfp);

    if (most_general_error_flag == 1) {
        // delete file.ll
        if (remove("file.ll") != 0) {
            printf("Error deleting file\n");
        }
    }

    return 0;
}