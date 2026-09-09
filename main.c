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
    TOKEN_KW_WHILE,

    TOKEN_ASSIGN,

    TOKEN_PLUS,
    TOKEN_MINUS,
    TOKEN_STAR,
    TOKEN_SLASH,

    TOKEN_GREATER,
    TOKEN_LESS,
    TOKEN_GREATER_EQUAL,
    TOKEN_LESS_EQUAL,
    TOKEN_EQUAL,
    TOKEN_NOT_EQUAL,

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

// Symbol table entry
typedef struct {
    char name[32];
    int stack_offset;
} Symbol;

Symbol symbol_table[100];
int symbol_count = 0;

// AST node types
typedef enum {
    AST_INT,
    AST_VAR,
    AST_BINARY_EXPR,
    AST_VAR_DECL,
    AST_ASSIGN,
    AST_RETURN_STMT,
    AST_IF_STMT,
    AST_WHILE_STMT
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

    struct ASTNode *body;
    struct ASTNode *next;

} ASTNode;

// Parser function declarations
ASTNode* parse_if_statement(const char **src);
ASTNode* parse_while_statement(const char **src);

void free_ast(ASTNode *node);

// Current token
Token current_token;

// Label counter
int label_sequence = 0;

// Check keyword
TokenType check_keyword(const char *text) {

    if (strcmp(text, "int") == 0)
        return TOKEN_KW_INT;

    if (strcmp(text, "return") == 0)
        return TOKEN_KW_RETURN;

    if (strcmp(text, "if") == 0)
        return TOKEN_KW_IF;

    if (strcmp(text, "else") == 0)
        return TOKEN_KW_ELSE;

    if (strcmp(text, "while") == 0)
        return TOKEN_KW_WHILE;

    return TOKEN_IDENTIFIER;
}

// Lexer function
Token get_next_token(const char **src) {

    Token t;

    t.type = TOKEN_UNKNOWN;
    t.value = 0;
    t.text[0] = '\0';

    // Skip spaces
    while (**src == ' ' ||
           **src == '\n' ||
           **src == '\t' ||
           **src == '\r') {

        (*src)++;
    }

    // End of input
    if (**src == '\0') {
        t.type = TOKEN_EOF;
        return t;
    }

    // Read number
    if (isdigit(**src)) {

        int value = 0;

        while (isdigit(**src)) {

            value = value * 10 + (**src - '0');
            (*src)++;
        }

        t.type = TOKEN_INT;
        t.value = value;

        return t;
    }

    // Read identifier or keyword
    if (isalpha(**src) || **src == '_') {

        char buf[32];
        int i = 0;

        while (isalnum(**src) || **src == '_') {

            if (i < 31) {
                buf[i++] = **src;
            }

            (*src)++;
        }

        buf[i] = '\0';

        strcpy(t.text, buf);

        t.type = check_keyword(buf);

        return t;
    }

    // Read operators and symbols
    switch (**src) {

        case '=':

            (*src)++;

            if (**src == '=') {

                t.type = TOKEN_EQUAL;
                (*src)++;

            } else {

                t.type = TOKEN_ASSIGN;
            }

            break;

        case '!':

            (*src)++;

            if (**src == '=') {

                t.type = TOKEN_NOT_EQUAL;
                (*src)++;

            } else {

                t.type = TOKEN_UNKNOWN;
            }

            break;

        case '>':

            (*src)++;

            if (**src == '=') {

                t.type = TOKEN_GREATER_EQUAL;
                (*src)++;

            } else {

                t.type = TOKEN_GREATER;
            }

            break;

        case '<':

            (*src)++;

            if (**src == '=') {

                t.type = TOKEN_LESS_EQUAL;
                (*src)++;

            } else {

                t.type = TOKEN_LESS;
            }

            break;

        case '+':

            t.type = TOKEN_PLUS;
            (*src)++;

            break;

        case '-':

            t.type = TOKEN_MINUS;
            (*src)++;

            break;

        case '*':

            t.type = TOKEN_STAR;
            (*src)++;

            break;

        case '/':

            t.type = TOKEN_SLASH;
            (*src)++;

            break;

        case '(':

            t.type = TOKEN_LPAREN;
            (*src)++;

            break;

        case ')':

            t.type = TOKEN_RPAREN;
            (*src)++;

            break;

        case '{':

            t.type = TOKEN_LBRACE;
            (*src)++;

            break;

        case '}':

            t.type = TOKEN_RBRACE;
            (*src)++;

            break;

        case ';':

            t.type = TOKEN_SEMICOLON;
            (*src)++;

            break;

        default:

            t.type = TOKEN_UNKNOWN;
            (*src)++;

            break;
    }

    return t;
}

// Read source file
char* read_file(const char *filename) {

    FILE *file = fopen(filename, "rb");

    if (!file) {

        printf("Error: Could not open file %s\n",
               filename);

        return NULL;
    }

    fseek(file, 0, SEEK_END);

    long size = ftell(file);

    fseek(file, 0, SEEK_SET);

    char *buffer =
        malloc(size + 1);

    if (!buffer) {

        printf("Error: Memory allocation failed!\n");

        fclose(file);

        return NULL;
    }

    fread(buffer, 1, size, file);

    buffer[size] = '\0';

    fclose(file);

    return buffer;
}

// Create AST node
ASTNode* create_node(ASTNodeType type) {

    ASTNode *node =
        (ASTNode*)calloc(1, sizeof(ASTNode));

    if (!node) {

        printf("Error: Memory allocation failed!\n");

        exit(1);
    }

    node->type = type;

    return node;
}

// Advance token
void advance_token(const char **src) {

    current_token =
        get_next_token(src);
}

// Add symbol
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

    int offset =
        (symbol_count + 1) * 8;

    strcpy(symbol_table[symbol_count].name,
           name);

    symbol_table[symbol_count].stack_offset =
        offset;

    symbol_count++;

    return offset;
}

// Get symbol offset
int get_symbol_offset(const char *name) {

    for (int i = 0; i < symbol_count; i++) {

        if (strcmp(symbol_table[i].name, name) == 0) {

            return symbol_table[i].stack_offset;
        }
    }

    return -1;
}

// Parse primary expression
ASTNode* parse_primary(const char **src) {

    if (current_token.type == TOKEN_INT) {

        ASTNode *node =
            create_node(AST_INT);

        node->int_val =
            current_token.value;

        advance_token(src);

        return node;
    }

    if (current_token.type == TOKEN_IDENTIFIER) {

        ASTNode *node =
            create_node(AST_VAR);

        strcpy(node->var_name,
               current_token.text);

        advance_token(src);

        return node;
    }

    return NULL;
}

// Parse expression
ASTNode* parse_expression(const char **src) {

    ASTNode *left =
        parse_primary(src);

    if (!left) {
        return NULL;
    }

    while (current_token.type == TOKEN_PLUS ||
           current_token.type == TOKEN_MINUS ||
           current_token.type == TOKEN_GREATER ||
           current_token.type == TOKEN_LESS ||
           current_token.type == TOKEN_GREATER_EQUAL ||
           current_token.type == TOKEN_LESS_EQUAL ||
           current_token.type == TOKEN_EQUAL ||
           current_token.type == TOKEN_NOT_EQUAL) {

        char op[3];

        if (current_token.type == TOKEN_PLUS) {

            strcpy(op, "+");

        } else if (current_token.type == TOKEN_MINUS) {

            strcpy(op, "-");

        } else if (current_token.type == TOKEN_GREATER) {

            strcpy(op, ">");

        } else if (current_token.type == TOKEN_LESS) {

            strcpy(op, "<");

        } else if (current_token.type == TOKEN_GREATER_EQUAL) {

            strcpy(op, ">=");

        } else if (current_token.type == TOKEN_LESS_EQUAL) {

            strcpy(op, "<=");

        } else if (current_token.type == TOKEN_EQUAL) {

            strcpy(op, "==");

        } else {

            strcpy(op, "!=");
        }

        advance_token(src);

        ASTNode *right =
            parse_primary(src);

        if (!right) {

            printf("Error: Expected expression after operator!\n");

            free_ast(left);

            exit(1);
        }

        ASTNode *node =
            create_node(AST_BINARY_EXPR);

        node->op = op[0];

        node->left = left;
        node->right = right;

        if (strcmp(op, ">") == 0)
            node->op = '>';

        else if (strcmp(op, "<") == 0)
            node->op = '<';

        else if (strcmp(op, ">=") == 0)
            node->op = 'G';

        else if (strcmp(op, "<=") == 0)
            node->op = 'L';

        else if (strcmp(op, "==") == 0)
            node->op = 'E';

        else if (strcmp(op, "!=") == 0)
            node->op = 'N';

        left = node;
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

        printf("Error: Expected variable name!\n");

        exit(1);
    }

    ASTNode *node =
        create_node(AST_VAR_DECL);

    strcpy(node->var_name,
           current_token.text);

    advance_token(src);

    if (current_token.type != TOKEN_ASSIGN) {

        printf("Error: Expected '=' after variable name!\n");

        free_ast(node);

        exit(1);
    }

    advance_token(src);

    node->expr =
        parse_expression(src);

    if (!node->expr) {

        printf("Error: Expected expression after '='!\n");

        free_ast(node);

        exit(1);
    }

    if (current_token.type != TOKEN_SEMICOLON) {

        printf("Error: Expected ';' after variable declaration!\n");

        free_ast(node);

        exit(1);
    }

    advance_token(src);

    return node;
}

// Parse assignment
ASTNode* parse_assignment(const char **src) {

    if (current_token.type != TOKEN_IDENTIFIER) {
        return NULL;
    }

    char variable_name[32];

    strcpy(variable_name,
           current_token.text);

    advance_token(src);

    if (current_token.type != TOKEN_ASSIGN) {
        return NULL;
    }

    advance_token(src);

    ASTNode *node =
        create_node(AST_ASSIGN);

    strcpy(node->var_name,
           variable_name);

    node->expr =
        parse_expression(src);

    if (!node->expr) {

        printf("Error: Expected expression after '='!\n");

        free_ast(node);

        exit(1);
    }

    if (current_token.type != TOKEN_SEMICOLON) {

        printf("Error: Expected ';' after assignment!\n");

        free_ast(node);

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

    ASTNode *node =
        create_node(AST_RETURN_STMT);

    node->return_value =
        parse_expression(src);

    if (!node->return_value) {

        printf("Error: Expected return expression!\n");

        free_ast(node);

        exit(1);
    }

    if (current_token.type != TOKEN_SEMICOLON) {

        printf("Error: Expected ';' after return!\n");

        free_ast(node);

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

    ASTNode *first = NULL;
    ASTNode *last = NULL;

    while (current_token.type != TOKEN_RBRACE &&
           current_token.type != TOKEN_EOF) {

        ASTNode *statement = NULL;

        if (current_token.type == TOKEN_KW_INT) {

            statement =
                parse_variable_declaration(src);

        } else if (current_token.type == TOKEN_KW_RETURN) {

            statement =
                parse_return_statement(src);

        } else if (current_token.type == TOKEN_KW_IF) {

            statement =
                parse_if_statement(src);

        } else if (current_token.type == TOKEN_KW_WHILE) {

            statement =
                parse_while_statement(src);

        } else if (current_token.type == TOKEN_IDENTIFIER) {

            statement =
                parse_assignment(src);

        } else {

            printf("Error: Unexpected token inside block!\n");

            exit(1);
        }

        if (statement) {

            if (!first) {

                first = statement;

            } else {

                last->next = statement;
            }

            last = statement;
        }
    }

    if (current_token.type != TOKEN_RBRACE) {

        printf("Error: Expected '}'!\n");

        free_ast(first);

        exit(1);
    }

    advance_token(src);

    return first;
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

    ASTNode *node =
        create_node(AST_IF_STMT);

    node->cond =
        parse_expression(src);

    if (!node->cond) {

        printf("Error: Expected condition inside if!\n");

        free_ast(node);

        exit(1);
    }

    if (current_token.type != TOKEN_RPAREN) {

        printf("Error: Expected ')' after condition!\n");

        free_ast(node);

        exit(1);
    }

    advance_token(src);

    node->then_block =
        parse_block(src);

    if (current_token.type == TOKEN_KW_ELSE) {

        advance_token(src);

        node->else_block =
            parse_block(src);
    }

    return node;
}

// Parse while statement
ASTNode* parse_while_statement(const char **src) {

    if (current_token.type != TOKEN_KW_WHILE) {
        return NULL;
    }

    advance_token(src);

    if (current_token.type != TOKEN_LPAREN) {

        printf("Error: Expected '(' after while!\n");

        exit(1);
    }

    advance_token(src);

    ASTNode *node =
        create_node(AST_WHILE_STMT);

    node->cond =
        parse_expression(src);

    if (!node->cond) {

        printf("Error: Expected condition inside while!\n");

        free_ast(node);

        exit(1);
    }

    if (current_token.type != TOKEN_RPAREN) {

        printf("Error: Expected ')' after condition!\n");

        free_ast(node);

        exit(1);
    }

    advance_token(src);

    node->body =
        parse_block(src);

    return node;
}

// Parse program
ASTNode* parse_program(const char **src) {

    ASTNode *first = NULL;
    ASTNode *last = NULL;

    while (current_token.type != TOKEN_EOF) {

        ASTNode *statement = NULL;

        if (current_token.type == TOKEN_KW_INT) {

            statement =
                parse_variable_declaration(src);

        } else if (current_token.type == TOKEN_KW_RETURN) {

            statement =
                parse_return_statement(src);

        } else if (current_token.type == TOKEN_KW_IF) {

            statement =
                parse_if_statement(src);

        } else if (current_token.type == TOKEN_KW_WHILE) {

            statement =
                parse_while_statement(src);

        } else if (current_token.type == TOKEN_IDENTIFIER) {

            statement =
                parse_assignment(src);

        } else {

            printf("Error: Unexpected token in program!\n");

            exit(1);
        }

        if (statement) {

            if (!first) {

                first = statement;

            } else {

                last->next = statement;
            }

            last = statement;
        }
    }

    return first;
}

// Print AST
void print_ast(ASTNode *node, int level) {

    while (node) {

        for (int i = 0; i < level; i++)
            printf("  ");

        switch (node->type) {

            case AST_INT:

                printf("IntegerLiteral(%d)\n",
                       node->int_val);

                break;

            case AST_VAR:

                printf("Variable(%s)\n",
                       node->var_name);

                break;

            case AST_BINARY_EXPR:

                if (node->op == 'G')
                    printf("BinaryExpr (>=)\n");

                else if (node->op == 'L')
                    printf("BinaryExpr (<=)\n");

                else if (node->op == 'E')
                    printf("BinaryExpr (==)\n");

                else if (node->op == 'N')
                    printf("BinaryExpr (!=)\n");

                else
                    printf("BinaryExpr (%c)\n",
                           node->op);

                print_ast(node->left,
                          level + 1);

                print_ast(node->right,
                          level + 1);

                break;

            case AST_VAR_DECL:

                printf("VariableDeclaration(%s)\n",
                       node->var_name);

                print_ast(node->expr,
                          level + 1);

                break;

            case AST_ASSIGN:

                printf("Assignment(%s)\n",
                       node->var_name);

                print_ast(node->expr,
                          level + 1);

                break;

            case AST_RETURN_STMT:

                printf("ReturnStatement\n");

                print_ast(node->return_value,
                          level + 1);

                break;

            case AST_IF_STMT:

                printf("IfStatement\n");

                for (int i = 0; i < level + 1; i++)
                    printf("  ");

                printf("Condition\n");

                print_ast(node->cond,
                          level + 2);

                for (int i = 0; i < level + 1; i++)
                    printf("  ");

                printf("ThenBlock\n");

                print_ast(node->then_block,
                          level + 2);

                if (node->else_block) {

                    for (int i = 0; i < level + 1; i++)
                        printf("  ");

                    printf("ElseBlock\n");

                    print_ast(node->else_block,
                              level + 2);
                }

                break;

            case AST_WHILE_STMT:

                printf("WhileStatement\n");

                for (int i = 0; i < level + 1; i++)
                    printf("  ");

                printf("Condition\n");

                print_ast(node->cond,
                          level + 2);

                for (int i = 0; i < level + 1; i++)
                    printf("  ");

                printf("Body\n");

                print_ast(node->body,
                          level + 2);

                break;
        }

        node = node->next;
    }
}

// Generate assembly
void generate_code(ASTNode *node,
                   FILE *output_file) {

    while (node) {

        switch (node->type) {

            case AST_INT:

                fprintf(output_file,
                        "    mov rax, %d\n",
                        node->int_val);

                break;

            case AST_VAR: {

                int offset =
                    get_symbol_offset(node->var_name);

                if (offset == -1) {

                    printf("Error: Undefined variable '%s'!\n",
                           node->var_name);

                    exit(1);
                }

                fprintf(output_file,
                        "    mov rax, [rbp - %d]\n",
                        offset);

                break;
            }

            case AST_BINARY_EXPR:

                generate_code(node->right,
                              output_file);

                fprintf(output_file,
                        "    push rax\n");

                generate_code(node->left,
                              output_file);

                fprintf(output_file,
                        "    pop rbx\n");

                if (node->op == '+') {

                    fprintf(output_file,
                            "    add rax, rbx\n");

                } else if (node->op == '-') {

                    fprintf(output_file,
                            "    sub rax, rbx\n");

                } else {

                    fprintf(output_file,
                            "    cmp rax, rbx\n");

                    if (node->op == '>') {

                        fprintf(output_file,
                                "    setg al\n");

                    } else if (node->op == '<') {

                        fprintf(output_file,
                                "    setl al\n");

                    } else if (node->op == 'G') {

                        fprintf(output_file,
                                "    setge al\n");

                    } else if (node->op == 'L') {

                        fprintf(output_file,
                                "    setle al\n");

                    } else if (node->op == 'E') {

                        fprintf(output_file,
                                "    sete al\n");

                    } else if (node->op == 'N') {

                        fprintf(output_file,
                                "    setne al\n");
                    }

                    fprintf(output_file,
                            "    movzx rax, al\n");
                }

                break;

            case AST_VAR_DECL: {

                int offset =
                    add_symbol(node->var_name);

                generate_code(node->expr,
                              output_file);

                fprintf(output_file,
                        "    mov [rbp - %d], rax\n",
                        offset);

                break;
            }

            case AST_ASSIGN: {

                int offset =
                    get_symbol_offset(node->var_name);

                if (offset == -1) {

                    printf("Error: Undefined variable '%s'!\n",
                           node->var_name);

                    exit(1);
                }

                generate_code(node->expr,
                              output_file);

                fprintf(output_file,
                        "    mov [rbp - %d], rax\n",
                        offset);

                break;
            }

            case AST_RETURN_STMT:

                generate_code(node->return_value,
                              output_file);

                fprintf(output_file,
                        "    mov rsp, rbp\n");

                fprintf(output_file,
                        "    pop rbp\n");

                fprintf(output_file,
                        "    ret\n");

                break;

            case AST_IF_STMT: {

                int label_id =
                    label_sequence++;

                generate_code(node->cond,
                              output_file);

                fprintf(output_file,
                        "    cmp rax, 0\n");

                fprintf(output_file,
                        "    je .Lelse_%d\n",
                        label_id);

                generate_code(node->then_block,
                              output_file);

                fprintf(output_file,
                        "    jmp .Lend_%d\n",
                        label_id);

                fprintf(output_file,
                        ".Lelse_%d:\n",
                        label_id);

                if (node->else_block) {

                    generate_code(node->else_block,
                                  output_file);
                }

                fprintf(output_file,
                        ".Lend_%d:\n",
                        label_id);

                break;
            }

            case AST_WHILE_STMT: {

                int label_id =
                    label_sequence++;

                fprintf(output_file,
                        ".Lloop_start_%d:\n",
                        label_id);

                generate_code(node->cond,
                              output_file);

                fprintf(output_file,
                        "    cmp rax, 0\n");

                fprintf(output_file,
                        "    je .Lloop_end_%d\n",
                        label_id);

                generate_code(node->body,
                              output_file);

                fprintf(output_file,
                        "    jmp .Lloop_start_%d\n",
                        label_id);

                fprintf(output_file,
                        ".Lloop_end_%d:\n",
                        label_id);

                break;
            }
        }

        node = node->next;
    }
}

// Free AST
void free_ast(ASTNode *node) {

    while (node) {

        ASTNode *next =
            node->next;

        free_ast(node->left);
        free_ast(node->right);
        free_ast(node->expr);
        free_ast(node->return_value);
        free_ast(node->cond);
        free_ast(node->then_block);
        free_ast(node->else_block);
        free_ast(node->body);

        free(node);

        node = next;
    }
}

// Main
int main(int argc, char *argv[]) {

    if (argc < 2) {

        printf("Usage: %s <source_file.c> [-o output_file.s]\n",
               argv[0]);

        return 1;
    }

    const char *input_file_path =
        argv[1];

    const char *output_file_path =
        "out.s";

    if (argc >= 4 &&
        strcmp(argv[2], "-o") == 0) {

        output_file_path =
            argv[3];
    }

    printf("Compiling %s -> %s...\n",
           input_file_path,
           output_file_path);

    char *source_code =
        read_file(input_file_path);

    if (!source_code) {
        return 1;
    }

    const char *ptr =
        source_code;

    symbol_count = 0;
    label_sequence = 0;

    current_token =
        get_next_token(&ptr);

    ASTNode *root =
        parse_program(&ptr);

    printf("Generating AST\n");

    print_ast(root, 0);

    FILE *output_file =
        fopen(output_file_path, "w");

    if (!output_file) {

        printf("Error: Could not create output file!\n");

        free_ast(root);
        free(source_code);

        return 1;
    }

    fprintf(output_file,
            ".intel_syntax noprefix\n");

    fprintf(output_file,
            ".globl main\n");

    fprintf(output_file,
            "main:\n");

    fprintf(output_file,
            "    push rbp\n");

    fprintf(output_file,
            "    mov rbp, rsp\n");

    fprintf(output_file,
            "    sub rsp, 800\n");

    generate_code(root,
                  output_file);

    fclose(output_file);

    printf("Assembly code successfully written to %s\n",
           output_file_path);

    printf("Compilation finished successfully!\n");

    free_ast(root);
    free(source_code);

    return 0;
}