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
   // dbg("Token : %s", tokenNames[present.type]);
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
    //dbg("In primary");
    if(match(LEFT_PARENTHESIS)){
        //dbg("In lp");
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

/*
 * Input construction
 * ============================================
 */

typedef struct{
    Symbols **combos;
    uint32_t length;
} Input;

static void input_print(Input *i){
    uint32_t count = 0;
    while(count < i->length){
        printf("\nSet %d : ", (count + 1));
        sym_print(i->combos[count]);
        count++;
    }
}

static Input* input_new(){
    Input *i = (Input *)malloc(sizeof(Input));
    i->combos = NULL;
    i->length = 0;
    return i;
}

static void input_free(Input *in){
    uint32_t i = 0;
    while(i < in->length){
        sym_free(in->combos[i]);
        i++;
    }
    free(in->combos);
    free(in);
}

static Input* input_append(Input *root, Symbol s){
    Input *ret = root;
    if(ret == NULL){
        ret = input_new();
        ret->combos = (Symbols **)malloc(sizeof(Symbols *));
        ret->combos[0] = sym_newcol();
        ret->length = 1;
        sym_add(ret->combos[0], s);
    }
    else{
        uint32_t i = 0;
        while(i < ret->length){
            sym_add(ret->combos[i], s);
            i++;
        }
    }

    return ret;
}

static Input* input_exp(Expression *exp);

static Input* input_lit(Token t){
    return input_append(NULL, t.value);
}

static void sym_app_sym(Symbols *orig, Symbols *append){
    uint64_t i = 0;
    while(i < append->numSymbols){
        sym_add(orig, append->symbols[i]);
        i++;
    }
}

static Input* input_and(Expression *e){
    Input *le = input_exp(e->binexp.left);
    Input *re = input_exp(e->binexp.right);
    //dbg("ANDing\nLeft expression : ");
    //input_print(le);
    //dbg("Right expression : ");
    //input_print(re);
    uint32_t i = 0, j = 0, k = 0;
    Input *res = input_new();
    res->length = le->length * re->length;
    res->combos = (Symbols **)malloc(sizeof(Symbols *) * res->length);
    while(i < le->length){
        Symbols *p = le->combos[i];
        j = 0;
        while(j < re->length){
            Symbols *q = sym_newcol();
            q->numSymbols = p->numSymbols;
            q->symbols = (Symbol *)malloc(sizeof(Symbol) * q->numSymbols);
            memcpy(q->symbols, p->symbols, sizeof(Symbol) * p->numSymbols);
            //dbg("Original symbols : ");
            //sym_print(q);
            Symbols *app = re->combos[j];
            //dbg("Appending symbols : ");
            //sym_print(app);
            sym_app_sym(q, app);
            //dbg("After appending : ");
            //sym_print(q);
            res->combos[k] = q;
            k++; j++;
        }
        i++;
    }
    input_free(le);
    input_free(re);
    //dbg("Resulting AND expression : ");
    //input_print(res);
    return res;
}

static Input* input_or(Expression *e){
    Input *le = input_exp(e->binexp.left);
    Input *re = input_exp(e->binexp.right);
    //dbg("ORing\nLeft expression : ");
    //input_print(le);
    //dbg("Right expression : ");
    //input_print(re);
    uint32_t po = le->length, i = 0;
    le->length += re->length;
    le->combos = (Symbols **)realloc(le->combos, sizeof(Symbols *) * le->length);
    while(po < le->length){
        le->combos[po] = re->combos[i];
        po++; i++;
    }
    //dbg("Resulting OR expression : ");
    free(re->combos);
    free(re);
    //input_print(le);
    return le;
}

static Input* input_exp(Expression *e){
    switch(e->type){
        case AND:
            return input_and(e);
        case OR:
            return input_or(e);
        case GROUPING:
            return input_exp(e->starexp);
        case LITREAL:
            return input_lit(e->litexp);
        case WILDCARD:
            err("Wildcard expressions are not implemented yet!");
            return NULL;
    }
}
/*
 * Driver
 * =====================================
 */

int main(){
    const char* exp = "((a+b.c.d).e.(f + g.h)).i";
    init_source(exp);
    printf("\nGiven expression : %s\n", exp);
    Expression *tree = expression();
    printf("\nParsed expression : ");
    printexp(tree);
    printf("\nGenerating possible output combinations...");
    Input *in = input_exp(tree);
    printf("\nOutputs : ");
    input_print(in);
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
    printf("\n");
    return 0;
}
