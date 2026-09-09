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

// Symbol table
typedef struct {
    char name[32];
    int stack_offset;
} Symbol;

Symbol symbol_table[100];
int symbol_count = 0;

// Add variable to symbol table
int add_symbol(const char *name) {

    for (int i = 0; i < symbol_count; i++) {
        if (strcmp(symbol_table[i].name, name) == 0) {
            return symbol_table[i].stack_offset;
        }
    }

    if (symbol_count >= 100) {
        printf("Error: Too many variables!\n");
        exit(1);
    }

    symbol_count++;

    int offset = symbol_count * 8;

    strcpy(symbol_table[symbol_count - 1].name, name);
    symbol_table[symbol_count - 1].stack_offset = offset;

    return offset;
}

// Get variable offset
int get_symbol_offset(const char *name) {

    for (int i = 0; i < symbol_count; i++) {
        if (strcmp(symbol_table[i].name, name) == 0) {
            return symbol_table[i].stack_offset;
        }
    }

    printf("Error: Variable '%s' undeclared!\n", name);
    exit(1);
}

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

// AST node types
typedef enum {
    AST_INT,
    AST_VAR,
    AST_BINARY_EXPR,
    AST_VAR_DECL,
    AST_RETURN_STMT,
    AST_IF_STMT
} ASTNodeType;

// AST node structure
typedef struct ASTNode {
    ASTNodeType type;

    int int_val;

    char op;

    char var_name[32];

    struct ASTNode *left;
    struct ASTNode *right;

    struct ASTNode *expr;

    struct ASTNode *return_value;

    struct ASTNode *cond;
    struct ASTNode *then_block;
    struct ASTNode *else_block;

    struct ASTNode *next;
} ASTNode;

// Create a new AST node
ASTNode* create_node(ASTNodeType type) {

    ASTNode *node = (ASTNode*)calloc(1, sizeof(ASTNode));

    if (!node) {
        printf("Error: Memory allocation failed!\n");
        exit(1);
    }

    node->type = type;

    return node;
}

// Parser current token
void free_ast(ASTNode *node);

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

    if (current_token.type == TOKEN_IDENTIFIER) {

        ASTNode *node = create_node(AST_VAR);

        strcpy(node->var_name, current_token.text);

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

        if (!right) {
            printf("Error: Expected expression after operator!\n");
            free_ast(left);
            exit(1);
        }

        ASTNode *binary_node = create_node(AST_BINARY_EXPR);

        binary_node->op = op;
        binary_node->left = left;
        binary_node->right = right;

        left = binary_node;
    }

    return left;
}

// Parse variable declaration
ASTNode* parse_variable_declaration(const char **src) {

    if (current_token.type != TOKEN_KW_INT) {
        return NULL;
    }

    advance_token(src);

    if (current_token.type != TOKEN_IDENTIFIER) {
        printf("Error: Expected variable name after int!\n");
        exit(1);
    }

    ASTNode *node = create_node(AST_VAR_DECL);

    strcpy(node->var_name, current_token.text);

    advance_token(src);

    if (current_token.type != TOKEN_ASSIGN) {
        printf("Error: Expected '=' after variable name!\n");
        free(node);
        exit(1);
    }

    advance_token(src);

    node->expr = parse_expression(src);

    if (!node->expr) {
        printf("Error: Expected value for variable!\n");
        free(node);
        exit(1);
    }

    if (current_token.type != TOKEN_SEMICOLON) {
        printf("Error: Expected ';' after variable declaration!\n");
        free(node);
        exit(1);
    }

    advance_token(src);

    return node;
}

// Parse return statement
ASTNode* parse_return_statement(const char **src) {

    if (current_token.type != TOKEN_KW_RETURN) {
        return NULL;
    }

    advance_token(src);

    ASTNode *node = create_node(AST_RETURN_STMT);

    node->return_value = parse_expression(src);

    if (!node->return_value) {
        printf("Error: Expected return value!\n");
        free(node);
        exit(1);
    }

    if (current_token.type != TOKEN_SEMICOLON) {
        printf("Error: Expected ';' after return statement!\n");
        free(node);
        exit(1);
    }

    advance_token(src);

    return node;
}

// Parse block
ASTNode* parse_block(const char **src) {

    if (current_token.type != TOKEN_LBRACE) {
        printf("Error: Expected '{'!\n");
        exit(1);
    }

    advance_token(src);

    ASTNode *head = NULL;
    ASTNode *tail = NULL;

    while (current_token.type != TOKEN_RBRACE &&
           current_token.type != TOKEN_EOF) {

        ASTNode *statement = NULL;

        if (current_token.type == TOKEN_KW_INT) {
            statement = parse_variable_declaration(src);
        } else if (current_token.type == TOKEN_KW_RETURN) {
            statement = parse_return_statement(src);
        } else {
            printf("Error: Unexpected token inside block!\n");
            exit(1);
        }

        if (!head) {
            head = statement;
            tail = statement;
        } else {
            tail->next = statement;
            tail = statement;
        }
    }

    if (current_token.type != TOKEN_RBRACE) {
        printf("Error: Expected '}'!\n");
        exit(1);
    }

    advance_token(src);

    return head;
}

// Parse if statement
ASTNode* parse_if_statement(const char **src) {

    if (current_token.type != TOKEN_KW_IF) {
        return NULL;
    }

    advance_token(src);

    if (current_token.type != TOKEN_LPAREN) {
        printf("Error: Expected '(' after if!\n");
        exit(1);
    }

    advance_token(src);

    ASTNode *node = create_node(AST_IF_STMT);

    node->cond = parse_expression(src);

    if (!node->cond) {
        printf("Error: Expected condition inside if!\n");
        free(node);
        exit(1);
    }

    if (current_token.type != TOKEN_RPAREN) {
        printf("Error: Expected ')' after condition!\n");
        free(node);
        exit(1);
    }

    advance_token(src);

    node->then_block = parse_block(src);

    if (current_token.type == TOKEN_KW_ELSE) {

        advance_token(src);

        node->else_block = parse_block(src);
    }

    return node;
}

// Parse all statements
ASTNode* parse_program(const char **src) {

    ASTNode *head = NULL;
    ASTNode *tail = NULL;

    while (current_token.type != TOKEN_EOF) {

        ASTNode *statement = NULL;

        if (current_token.type == TOKEN_KW_INT) {
            statement = parse_variable_declaration(src);
        } else if (current_token.type == TOKEN_KW_RETURN) {
            statement = parse_return_statement(src);
        } else if (current_token.type == TOKEN_KW_IF) {
            statement = parse_if_statement(src);
        } else {
            printf("Error: Unexpected token!\n");
            exit(1);
        }

        if (!head) {
            head = statement;
            tail = statement;
        } else {
            tail->next = statement;
            tail = statement;
        }
    }

    return head;
}

// Print AST
void print_ast(ASTNode *node, int indent) {

    while (node) {

        for (int i = 0; i < indent; i++) {
            printf("  ");
        }

        switch (node->type) {

            case AST_VAR_DECL:
                printf("VariableDeclaration(%s)\n", node->var_name);
                print_ast(node->expr, indent + 1);
                break;

            case AST_VAR:
                printf("Variable(%s)\n", node->var_name);
                break;

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

            case AST_IF_STMT:
                printf("IfStatement\n");

                for (int i = 0; i < indent + 1; i++) {
                    printf("  ");
                }

                printf("Condition\n");
                print_ast(node->cond, indent + 2);

                for (int i = 0; i < indent + 1; i++) {
                    printf("  ");
                }

                printf("ThenBlock\n");
                print_ast(node->then_block, indent + 2);

                if (node->else_block) {

                    for (int i = 0; i < indent + 1; i++) {
                        printf("  ");
                    }

                    printf("ElseBlock\n");
                    print_ast(node->else_block, indent + 2);
                }

                break;
        }

        node = node->next;
    }
}

// Label counter
int label_sequence = 0;

// Generate x86_64 assembly code
void generate_code(ASTNode *node, FILE *output_file) {

    while (node) {

        switch (node->type) {

            case AST_INT:
                fprintf(output_file,
                        "    mov rax, %d\n",
                        node->int_val);
                break;

            case AST_VAR: {
                int offset = get_symbol_offset(node->var_name);

                fprintf(output_file,
                        "    mov rax, [rbp - %d]\n",
                        offset);

                break;
            }

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

            case AST_VAR_DECL: {
                // Generate value of variable
                generate_code(node->expr, output_file);

                // Add variable to symbol table
                int offset = add_symbol(node->var_name);

                // Store value in stack
                fprintf(output_file,
                        "    mov [rbp - %d], rax\n",
                        offset);

                break;
            }

            case AST_IF_STMT: {
                int label_id = label_sequence++;

                // Generate condition
                generate_code(node->cond, output_file);

                fprintf(output_file,
                        "    cmp rax, 0\n");

                if (node->else_block) {
                    fprintf(output_file,
                            "    je .Lelse_%d\n",
                            label_id);
                } else {
                    fprintf(output_file,
                            "    je .Lend_%d\n",
                            label_id);
                }

                // Generate if block
                generate_code(node->then_block, output_file);

                if (node->else_block) {

                    fprintf(output_file,
                            "    jmp .Lend_%d\n",
                            label_id);

                    fprintf(output_file,
                            ".Lelse_%d:\n",
                            label_id);

                    // Generate else block
                    generate_code(node->else_block, output_file);
                }

                fprintf(output_file,
                        ".Lend_%d:\n",
                        label_id);

                break;
            }

            case AST_RETURN_STMT:

                // Generate return value
                generate_code(node->return_value, output_file);

                // Function epilogue
                fprintf(output_file,
                        "    mov rsp, rbp\n");

                fprintf(output_file,
                        "    pop rbp\n");

                fprintf(output_file,
                        "    ret\n");

                break;
        }

        node = node->next;
    }
}

// Free AST memory
void free_ast(ASTNode *node) {

    if (!node) {
        return;
    }

    free_ast(node->left);
    free_ast(node->right);
    free_ast(node->expr);
    free_ast(node->return_value);
    free_ast(node->cond);
    free_ast(node->then_block);
    free_ast(node->else_block);
    free_ast(node->next);

    free(node);
}

// Read entire file
char* read_file(const char *file_path) {

    FILE *file = fopen(file_path, "rb");

    if (!file) {
        printf("Error: Could not open file %s\n",
               file_path);
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

int main(int argc, char *argv[]) {

    // Check command-line arguments
    if (argc < 2) {
        printf("Usage: %s <source_file.c> [-o output_file.s]\n",
               argv[0]);
        return 1;
    }

    // Set input and output file names
    const char *input_file_path = argv[1];
    const char *output_file_path = "out.s";

    // Check for custom output file
    if (argc >= 4 && strcmp(argv[2], "-o") == 0) {
        output_file_path = argv[3];
    }

    // Read source file
    char *source_code = read_file(input_file_path);

    if (!source_code) {
        return 1;
    }

    printf("Compiling %s -> %s...\n",
           input_file_path,
           output_file_path);

    // Start lexer
    const char *ptr = source_code;

    advance_token(&ptr);

    // Parse source code
    ASTNode *ast_root = parse_program(&ptr);

    if (!ast_root) {
        printf("Error: Could not parse source file!\n");
        free(source_code);
        return 1;
    }

    printf("Generating AST\n");
    print_ast(ast_root, 0);

    // Open output assembly file
    FILE *output_file = fopen(output_file_path, "w");

    if (!output_file) {
        printf("Error: Could not open output file %s\n",
               output_file_path);

        free_ast(ast_root);
        free(source_code);

        return 1;
    }

    // Write assembly header
    fprintf(output_file,
            ".intel_syntax noprefix\n");

    fprintf(output_file,
            ".globl main\n");

    fprintf(output_file,
            "main:\n");

    // Function prologue
    fprintf(output_file,
            "    push rbp\n");

    fprintf(output_file,
            "    mov rbp, rsp\n");

    fprintf(output_file,
            "    sub rsp, 800\n");

    // Generate assembly code
    generate_code(ast_root, output_file);

    fclose(output_file);

    printf("Assembly code successfully written to %s\n",
           output_file_path);

    printf("Compilation finished successfully!\n");

    // Free memory
    free_ast(ast_root);
    free(source_code);

    return 0;
}