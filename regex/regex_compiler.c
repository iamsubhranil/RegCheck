#include "regex_tree.h"
#include "../display.h"

#include <string.h>
#include <stdio.h>
#include <inttypes.h>
#include <stdlib.h>
/*
 *  Scanner
 *  ==========================================
 */

static char *source = NULL, *head = NULL;
static uint32_t position = 0;

void init_source(const char *s){
    source = strdup(s);
    head = source;
}

#define advance() {position++; head++;}

static Token token_new(TokenType type){
    Token t;
    t.type = type;
    t.value = sym_new(*head);
    t.position = position;
    advance();
    return t;
}

static Token nextToken(){
    switch(*head){
        case '\0':
        case '\n':
        case '\r':
            return token_new(END);
        case '*':
            return token_new(STAR);
        case '+':
            return token_new(PLUS);
        case '(':
            return token_new(LEFT_PARENTHESIS);
        case ')':
            return token_new(RIGHT_PARENTHESIS);
        case ' ':
            advance();
            return nextToken();
        case '.':
            return token_new(DOT);
        default:
            if((*head >= 'a' && *head <= 'z')
                    || (*head >= 'A' && *head <= 'Z')
                    || (*head >= '0' && *head <= '9'))
                return token_new(IDENTIFIER);
            else{
                err("Ignoring bad symbol '%c' at position %" PRIu32, *head, position);
                advance();
                return nextToken();
            }
    }
}

/*
 * ==========================================
 */

/*
 * Parser
 * ==========================================
 * Grammar :
 * Primary    : Grouping
 *            : Literal
 * Grouping   : "(" Expression ")"
 * Literal    : [a-z|A-Z|0-9]
 * WildCard   : Primary "*"
 * And        : WildCard AND WildCard
 * Or         : And OR And
 * Expression : Or
 */

static Token present, previous;
static bool started = false;

static void next(){
    previous = present;
    present = nextToken();
    dbg("Token : %s", tokenNames[present.type]);
}

static void start(){
    if(!started){
        next();
        started = true;
    }
}

static bool match(TokenType type){
    if(present.type == type){
        next();
        return true;
    }
    return false;
}

static void consume(TokenType type, const char *e){
    if(present.type == type){
        next();
    }
    else{
        err(e);
    }
}

static Expression *exp_new(){
    Expression *exp = (Expression *)malloc(sizeof(Expression));
    return exp;
}

static Expression* expression();

static Expression* primary(){
    dbg("In primary");
    if(match(LEFT_PARENTHESIS)){
        dbg("In lp");
        Expression *g = exp_new();
        g->type = GROUPING;
        g->starexp = expression();
        consume(RIGHT_PARENTHESIS, "Expected ')' after expression!");
        return g;
    }
    else if(match(IDENTIFIER)){
        Expression *lit = exp_new();
        lit->type = LITREAL;
        lit->litexp = previous;
        return lit;
    }
    else{
        err("Unexpected token : %s", tokenNames[present.type]);
        next();
        return NULL;
    }
}

static Expression* wildcard(){
    Expression *exp = primary();
    if(match(STAR)){
        Expression *s = exp_new();
        s->type = WILDCARD;
        s->starexp = exp;
        return s;
    }
    return exp;
}

static Expression* andex(){
    Expression *exp = wildcard();
    if(match(DOT)){
        Expression *s = exp_new();
        s->type = AND;
        s->binexp.left = exp;
        s->binexp.right = expression();
        return s;
    }
    return exp;
}

static Expression* orex(){
    Expression *exp = andex();
    if(match(PLUS)){
        Expression *s = exp_new();
        s->type = OR;
        s->binexp.left = exp;
        s->binexp.right = expression();
        return s;
    }
    return exp;
}

static Expression* expression(){
    start();
    return orex();
}

/*
 * Expression Printing
 * =======================================
 */

static void printexp(Expression *exp);

static void printgroup(Expression *exp){
    printf("(");
    printexp(exp->starexp);
    printf(")");
}

static void printlit(Token literal){
    sym_print_single(literal.value);
}

static void printor(Expression *exp){
    printexp(exp->binexp.left);
    printf("+");
    printexp(exp->binexp.right);
}

static void printand(Expression *exp){
    printexp(exp->binexp.left);
    printf(".");
    printexp(exp->binexp.right);
}

static void printstar(Expression *exp){
    printexp(exp->starexp);
    printf("*");
}

static void printexp(Expression *exp){
    if(exp == NULL){
        printf("Null!");
    }
    else{
        switch(exp->type){
            case AND:
                printand(exp);
                break;
            case OR:
                printor(exp);
                break;
            case LITREAL:
                printlit(exp->litexp);
                break;
            case WILDCARD:
                printstar(exp);
                break;
            case GROUPING:
                printgroup(exp);
                break;
        }
    }
}

int main(){
    const char* exp = "((a+a.b.c).a*.(b* + a.b*)).a*";
    init_source(exp);
    printf("\nGiven expression : %s\n", exp);
    Expression *tree = expression();
    printf("\nParsed expression : ");
    printexp(tree);
    /*while((t = nextToken()).type != END){
      switch(t.type){
      case IDENTIFIER:
      printf("Identifer");
      break;
      case LEFT_PARENTHESIS:
      printf("Left_Parenthesis");
      break;
      case PLUS:
      printf("Plus");
      break;
      case RIGHT_PARENTHESIS:
      printf("Right_Parenthesis");
      break;
      case STAR:
      printf("Star");
      break;
      case DOT:
      printf("Dot");
      break;
      case END:
      printf("End");
      break;
      }
      printf("(");
      sym_print_single(t.value);
      printf(") ");
      }*/

    return 0;
}
