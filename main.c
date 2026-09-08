#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

// Token types
typedef enum {
    TOKEN_INT,
    TOKEN_IDENTIFIER,

    // Keywords
    TOKEN_KW_INT,
    TOKEN_KW_RETURN,
    TOKEN_KW_IF,
    TOKEN_KW_ELSE,

    TOKEN_ASSIGN,
    TOKEN_PLUS,
    TOKEN_MINUS,
    TOKEN_STAR,
    TOKEN_SLASH,
    TOKEN_LPAREN,
    TOKEN_RPAREN,
    TOKEN_LBRACE,
    TOKEN_RBRACE,
    TOKEN_SEMICOLON,
    TOKEN_EOF,
    TOKEN_UNKNOWN
} TokenType;

// Token structure
typedef struct {
    TokenType type;
    int value;
    char text[32];
} Token;

// Check if a word is a keyword or identifier
TokenType check_keyword(const char *text) {

    if (strcmp(text, "int") == 0) {
        return TOKEN_KW_INT;
    }

    if (strcmp(text, "return") == 0) {
        return TOKEN_KW_RETURN;
    }

    if (strcmp(text, "if") == 0) {
        return TOKEN_KW_IF;
    }

    if (strcmp(text, "else") == 0) {
        return TOKEN_KW_ELSE;
    }

    return TOKEN_IDENTIFIER;
}

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

    // Read identifier or keyword
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

        // Check if the word is a keyword
        t.type = check_keyword(buf);

        t.value = 0;

        strcpy(t.text, buf);

        return t;
    }

    // Read operators and symbols
    char current = **src;
    (*src)++;

    switch (current) {

        case '=':
            return (Token){TOKEN_ASSIGN, 0, "="};

        case '+':
            return (Token){TOKEN_PLUS, 0, "+"};

        case '-':
            return (Token){TOKEN_MINUS, 0, "-"};

        case '*':
            return (Token){TOKEN_STAR, 0, "*"};

        case '/':
            return (Token){TOKEN_SLASH, 0, "/"};

        case '(':
            return (Token){TOKEN_LPAREN, 0, "("};

        case ')':
            return (Token){TOKEN_RPAREN, 0, ")"};

        case '{':
            return (Token){TOKEN_LBRACE, 0, "{"};

        case '}':
            return (Token){TOKEN_RBRACE, 0, "}"};

        case ';':
            return (Token){TOKEN_SEMICOLON, 0, ";"};

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

        case TOKEN_KW_INT:
            printf("TOKEN_KEYWORD(int)\n");
            break;

        case TOKEN_KW_RETURN:
            printf("TOKEN_KEYWORD(return)\n");
            break;

        case TOKEN_KW_IF:
            printf("TOKEN_KEYWORD(if)\n");
            break;

        case TOKEN_KW_ELSE:
            printf("TOKEN_KEYWORD(else)\n");
            break;

        case TOKEN_ASSIGN:
            printf("TOKEN_ASSIGN (=)\n");
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

        case TOKEN_LPAREN:
            printf("TOKEN_LPAREN (()\n");
            break;

        case TOKEN_RPAREN:
            printf("TOKEN_RPAREN ())\n");
            break;

        case TOKEN_LBRACE:
            printf("TOKEN_LBRACE ({)\n");
            break;

        case TOKEN_RBRACE:
            printf("TOKEN_RBRACE (})\n");
            break;

        case TOKEN_SEMICOLON:
            printf("TOKEN_SEMICOLON (;)\n");
            break;

        case TOKEN_EOF:
            printf("TOKEN_EOF\n");
            break;

        default:
            printf("TOKEN_UNKNOWN\n");
            break;
    }
}

// Read entire file
char* read_file(const char *file_path) {

    FILE *file = fopen(file_path, "rb");

    if (!file) {
        printf("Error: Could not open file %s\n", file_path);
        return NULL;
    }

    // Get file size
    fseek(file, 0, SEEK_END);

    long size = ftell(file);

    fseek(file, 0, SEEK_SET);

    // Allocate memory
    char *buffer = (char*)malloc(size + 1);

    if (!buffer) {
        fclose(file);
        printf("Error: Memory allocation failed!\n");
        return NULL;
    }

    // Read file
    fread(buffer, 1, size, file);

    buffer[size] = '\0';

    fclose(file);

    return buffer;
}

int main() {

    // Read C code from test.c
    char *source_code = read_file("test.c");

    if (!source_code) {
        return 1;
    }

    const char *ptr = source_code;

    printf("--- Reading from test.c ---\n");

    printf("%s\n", source_code);

    printf("--- Lexer Output (Tokens) ---\n");

    Token token;

    do {

        token = get_next_token(&ptr);

        print_token(token);

    } while (token.type != TOKEN_EOF);

    // Free allocated memory
    free(source_code);

    return 0;
}