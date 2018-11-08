#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include "9cc.h"

enum {
    TK_NUM = 256,
    TK_IDENT,
    TK_EOF,
};

typedef struct {
    int ty;
    int val;
    char *input;
} Token;

Node *code[100];

static Token tokens[100];
static int pos;

static Node *expr();

void tokenize(char *p) {
    int i = 0;
    while (*p) {
        if (isspace(*p)) {
            p++;
            continue;
        }

        if (*p == '+' || *p == '-' || *p == '*' || *p == '/' || *p == '(' || *p == ')' || *p == '=' || *p == ';') {
            tokens[i].ty = *p;
            tokens[i].input = p;
            i++;
            p++;
            continue;
        }

        if (isdigit(*p)) {
            tokens[i].ty = TK_NUM;
            tokens[i].input = p;
            tokens[i].val = strtol(p, &p, 10);
            i++;
            continue;
        }

        if ('a' <= *p && *p <= 'z') {
            tokens[i].ty = TK_IDENT;
            tokens[i].input = p;
            i++;
            p++;
            continue;
        }

        fprintf(stderr, "Could not tokenize: '%s'\n", p);
        exit(1);
    }

    tokens[i].ty = TK_EOF;
    tokens[i].input = p;
}

static Node *new_node(int ty, Node *lhs, Node *rhs) {
    Node *node = malloc(sizeof(Node));
    node->ty = ty;
    node->lhs = lhs;
    node->rhs = rhs;
    return node;
}

static Node *new_node_num(int val) {
    Node *node = malloc(sizeof(Node));
    node->ty = ND_NUM;
    node->val = val;
    return node;
}

static Node *new_node_ident(char name) {
    Node *node = malloc(sizeof(Node));
    node->ty = ND_IDENT;
    node->name = name;
    return node;
}

static Node *term() {
    if (tokens[pos].ty == TK_NUM) {
        return new_node_num(tokens[pos++].val);
    }
    if (tokens[pos].ty == TK_IDENT) {
        return new_node_ident(*tokens[pos++].input);
    }
    if (tokens[pos].ty == '(') {
        pos++;
        Node *node = expr();
        if (tokens[pos].ty != ')') {
            fprintf(stderr, "Unexpected token: %s\n", tokens[pos].input);
            exit(1);
        }
        pos++;
        return node;
    }
    fprintf(stderr, "Unexpected token: %s\n", tokens[pos].input);
    exit(1);
}

static Node *mul() {
    Node *lhs = term();
    if (tokens[pos].ty == '*') {
        pos++;
        return new_node('*', lhs, mul());
    }
    if (tokens[pos].ty == '/') {
        pos++;
        return new_node('/', lhs, mul());
    }
    return lhs;
}

static Node *expr() {
    Node *lhs = mul();
    if (tokens[pos].ty == '+') {
        pos++;
        return new_node('+', lhs, expr());
    }
    if (tokens[pos].ty == '-') {
        pos++;
        return new_node('-', lhs, expr());
    }
    return lhs;
}

static Node *assign() {
    Node *lhs = expr();
    if (tokens[pos].ty == '=') {
        pos++;
        return new_node('=', lhs, assign());
    }
    if (tokens[pos].ty == ';') {
        pos++;
    }
    return lhs;
}

void program() {
    int i = 0;
    while (tokens[pos].ty != TK_EOF) {
        code[i++] = assign();
    }
}
