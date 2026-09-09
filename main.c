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

// AST node types
typedef enum {
    AST_INT,
    AST_BINARY_EXPR,
    AST_RETURN_STMT
} ASTNodeType;

// AST node structure
typedef struct ASTNode {
    ASTNodeType type;
    int int_val;
    char op;
    struct ASTNode *left;
    struct ASTNode *right;
    struct ASTNode *return_value;
} ASTNode;

// Create a new AST node
ASTNode* create_node(ASTNodeType type) {

    ASTNode *node = (ASTNode*)malloc(sizeof(ASTNode));

    node->type = type;
    node->int_val = 0;
    node->op = '\0';
    node->left = NULL;
    node->right = NULL;
    node->return_value = NULL;

    return node;
}

// Parser current token
Token current_token;

// Get next token from lexer
void advance_token(const char **src) {
    current_token = get_next_token(src);
}

// Parse primary expression
ASTNode* parse_primary(const char **src) {

    if (current_token.type == TOKEN_INT) {

        ASTNode *node = create_node(AST_INT);

        node->int_val = current_token.value;

        advance_token(src);

        return node;
    }

    return NULL;
}

// Parse binary expression
ASTNode* parse_expression(const char **src) {

    ASTNode *left = parse_primary(src);

    while (current_token.type == TOKEN_PLUS ||
           current_token.type == TOKEN_MINUS) {

        char op;

        if (current_token.type == TOKEN_PLUS) {
            op = '+';
        } else {
            op = '-';
        }

        advance_token(src);

        ASTNode *right = parse_primary(src);

        ASTNode *binary_node = create_node(AST_BINARY_EXPR);

        binary_node->op = op;
        binary_node->left = left;
        binary_node->right = right;

        left = binary_node;
    }

    return left;
}

// Parse return statement
ASTNode* parse_statement(const char **src) {

    if (current_token.type == TOKEN_KW_RETURN) {

        advance_token(src);

        ASTNode *stmt = create_node(AST_RETURN_STMT);

        stmt->return_value = parse_expression(src);

        if (current_token.type == TOKEN_SEMICOLON) {
            advance_token(src);
        }

        return stmt;
    }

    return NULL;
}

// Print AST
void print_ast(ASTNode *node, int indent) {

    if (!node) {
        return;
    }

    for (int i = 0; i < indent; i++) {
        printf("  ");
    }

    switch (node->type) {

        case AST_RETURN_STMT:
            printf("ReturnStatement\n");
            print_ast(node->return_value, indent + 1);
            break;

        case AST_BINARY_EXPR:
            printf("BinaryExpr (%c)\n", node->op);
            print_ast(node->left, indent + 1);
            print_ast(node->right, indent + 1);
            break;

        case AST_INT:
            printf("IntegerLiteral(%d)\n", node->int_val);
            break;
    }
}

// Generate x86_64 assembly code
void generate_code(ASTNode *node, FILE *output_file) {

    if (!node) {
        return;
    }

    switch (node->type) {

        case AST_INT:
            fprintf(output_file, "    mov rax, %d\n", node->int_val);
            break;

        case AST_BINARY_EXPR:

            // Evaluate right side and save it on the stack
            generate_code(node->right, output_file);

            fprintf(output_file, "    push rax\n");

            // Evaluate left side
            generate_code(node->left, output_file);

            // Get right side value back
            fprintf(output_file, "    pop rbx\n");

            // Perform the operation
            if (node->op == '+') {
                fprintf(output_file, "    add rax, rbx\n");
            } else if (node->op == '-') {
                fprintf(output_file, "    sub rax, rbx\n");
            }

            break;

        case AST_RETURN_STMT:

            // Generate code for return value
            generate_code(node->return_value, output_file);

            // Return from function
            fprintf(output_file, "    ret\n");

            break;
    }
}

// Free AST memory
void free_ast(ASTNode *node) {

    if (!node) {
        return;
    }

    free_ast(node->left);
    free_ast(node->right);
    free_ast(node->return_value);

    free(node);
}

int main() {

    // Read C code from test.c
    char *source_code = read_file("test.c");

    if (!source_code) {
        return 1;
    }

    const char *ptr = source_code;

    printf("Reading from test.c\n");
    printf("%s\n", source_code);

    // Load first token
    advance_token(&ptr);

    // Generate AST
    ASTNode *ast_root = parse_statement(&ptr);

    printf("Generating AST\n");
    print_ast(ast_root, 0);

    // Open output assembly file
    FILE *output_file = fopen("out.s", "w");

    if (!output_file) {
        printf("Error: Could not create output file!\n");
        free_ast(ast_root);
        free(source_code);
        return 1;
    }

    // Write assembly header
    fprintf(output_file, ".intel_syntax noprefix\n");
    fprintf(output_file, ".globl main\n");
    fprintf(output_file, "main:\n");

    // Generate assembly code
    generate_code(ast_root, output_file);

    // Close output file
    fclose(output_file);

    printf("Assembly code successfully written to out.s\n");

    // Free AST memory
    free_ast(ast_root);

    // Free source code memory
    free(source_code);

    return 0;
}