#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

// Token types
typedef enum {
    TOKEN_INT,
    TOKEN_IDENTIFIER,
    TOKEN_PLUS,
    TOKEN_MINUS,
    TOKEN_STAR,
    TOKEN_SLASH,
    TOKEN_EOF,
    TOKEN_UNKNOWN
} TokenType;

// Token structure
typedef struct {
    TokenType type;
    int value;
    char text[32];
} Token;

// Lexer function
Token get_next_token(const char **src) {

    // Skip spaces
    while (**src && isspace(**src)) {
        (*src)++;
    }

    // End of input
    if (**src == '\0') {
        return (Token){TOKEN_EOF, 0, ""};
    }

    // Read number
    if (isdigit(**src)) {
        int val = 0;

        while (isdigit(**src)) {
            val = val * 10 + (**src - '0');
            (*src)++;
        }

        return (Token){TOKEN_INT, val, ""};
    }

    // Read identifier
    if (isalpha(**src) || **src == '_') {

        char buf[32];
        int len = 0;

        while (isalnum(**src) || **src == '_') {

            if (len < 31) {
                buf[len++] = **src;
            }

            (*src)++;
        }

        buf[len] = '\0';

        Token t;

        t.type = TOKEN_IDENTIFIER;
        t.value = 0;

        strcpy(t.text, buf);

        return t;
    }

    // Read operators
    char current = **src;
    (*src)++;

    switch (current) {

        case '+':
            return (Token){TOKEN_PLUS, 0, "+"};

        case '-':
            return (Token){TOKEN_MINUS, 0, "-"};

        case '*':
            return (Token){TOKEN_STAR, 0, "*"};

        case '/':
            return (Token){TOKEN_SLASH, 0, "/"};

        default:
            return (Token){TOKEN_UNKNOWN, 0, ""};
    }
}

// Print token
void print_token(Token token) {

    switch (token.type) {

        case TOKEN_INT:
            printf("TOKEN_INT(%d)\n", token.value);
            break;

        case TOKEN_IDENTIFIER:
            printf("TOKEN_IDENTIFIER(\"%s\")\n", token.text);
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

    const char *source_code = "total = x + 50";

    const char *ptr = source_code;

    printf("Input Code: %s\n\n", source_code);

    printf("--- Lexer Output (Tokens) ---\n");

    Token token;

    do {

        token = get_next_token(&ptr);

        print_token(token);

    } while (token.type != TOKEN_EOF);

    return 0;
}