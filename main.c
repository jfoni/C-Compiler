#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

typedef enum {
    TOKEN_INT,
    TOKEN_PLUS,
    TOKEN_MINUS,
    TOKEN_STAR,
    TOKEN_SLASH,
    TOKEN_EOF,
    TOKEN_UNKNOWN
} TokenType;

typedef struct {
    TokenType type;
    int value;
} Token;

Token get_next_token(const char **src) {
    while (**src && isspace(**src)) {
        (*src)++;
    }

    if (**src == '\0') {
        return (Token){TOKEN_EOF, 0};
    }

    if (isdigit(**src)) {
        int val = 0;

        while (isdigit(**src)) {
            val = val * 10 + (**src - '0');
            (*src)++;
        }

        return (Token){TOKEN_INT, val};
    }

    char current = **src;
    (*src)++;

    switch (current) {
        case '+': return (Token){TOKEN_PLUS, 0};
        case '-': return (Token){TOKEN_MINUS, 0};
        case '*': return (Token){TOKEN_STAR, 0};
        case '/': return (Token){TOKEN_SLASH, 0};
        default: return (Token){TOKEN_UNKNOWN, 0};
    }
}

void print_token(Token token) {
    switch (token.type) {
        case TOKEN_INT:
            printf("TOKEN_INT(%d)\n", token.value);
            break;

        case TOKEN_PLUS:
            printf("TOKEN_PLUS (+)\n");
            break;

        case TOKEN_MINUS:
            printf("TOKEN_MINUS (-)\n");
            break;

        case TOKEN_STAR:
            printf("TOKEN_STAR (*)\n");
            break;

        case TOKEN_SLASH:
            printf("TOKEN_SLASH (/)\n");
            break;

        case TOKEN_EOF:
            printf("TOKEN_EOF\n");
            break;

        default:
            printf("TOKEN_UNKNOWN\n");
            break;
    }
}

int main() {
    char source_code[1000];

    printf("Enter expression: ");
    fgets(source_code, sizeof(source_code), stdin);

    const char *ptr = source_code;

    printf("\n--- Lexer Output (Tokens) ---\n");

    Token token;

    do {
        token = get_next_token(&ptr);
        print_token(token);
    } while (token.type != TOKEN_EOF);

    return 0;
}