#include "../symbols.h"
#include <stdint.h>

typedef enum{
    IDENTIFIER,
    PLUS,
    LEFT_PARENTHESIS,
    RIGHT_PARENTHESIS,
    STAR,
    DOT,
    END
} TokenType;

static const char *tokenNames[] = {"identifer", "plus", "left parenthesis", "right parenthesis",
                                    "star", "dot", "end"};

typedef struct{
    TokenType type;
    Symbol value;
    uint32_t position;
} Token;

typedef enum{
    LITREAL,
    OR,
    AND,
    WILDCARD,
    GROUPING
} ExpressionType;

static const char *expNames[] = {"literal", "or", "and", "wildcard", "grouping"};

typedef struct Ex Expression;

typedef struct{
    Expression *left;
    Expression *right;
} BinaryExpression;

struct Ex{
    ExpressionType type;
    union{
        BinaryExpression binexp;
        Expression *starexp;
        Token litexp;
    };
};
