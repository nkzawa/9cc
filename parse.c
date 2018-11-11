#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include "9cc.h"

enum {
    TK_NUM = 256,
    TK_IDENT,
    TK_EQ,
    TK_NE,
    TK_EOF,
};

typedef struct {
    int ty;
    int val;
    char *input;
} Token;

static Vector *tokens;
static int pos;

static Node *expr();

Vector *tokenize(char *p) {
    Vector *tokens = new_vector();

    int i = 0;
    while (*p) {
        if (isspace(*p)) {
            p++;
            continue;
        }

        Token *t = malloc(sizeof(Token));
        vec_push(tokens, t);

        if (p[0] == '=' && p[1] == '=') {
            t->ty = TK_EQ;
            t->input = p;
            i++;
            p += 2;
            continue;
        }

        if (p[0] == '!' && p[1] == '=') {
            t->ty = TK_NE;
            t->input = p;
            i++;
            p += 2;
            continue;
        }

        if (*p == '+' || *p == '-' || *p == '*' || *p == '/' || *p == '(' || *p == ')' || *p == '=' || *p == ';') {
            t->ty = *p;
            t->input = p;
            i++;
            p++;
            continue;
        }

        if (isdigit(*p)) {
            t->ty = TK_NUM;
            t->input = p;
            t->val = strtol(p, &p, 10);
            i++;
            continue;
        }

        if ('a' <= *p && *p <= 'z') {
            t->ty = TK_IDENT;
            t->input = p;
            i++;
            p++;
            continue;
        }

        fprintf(stderr, "Could not tokenize: '%s'\n", p);
        exit(1);
    }

    Token *t = malloc(sizeof(Token));
    vec_push(tokens, t);
    t->ty = TK_EOF;
    t->input = p;

    return tokens;
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
    Token *t = tokens->data[pos];
    if (t->ty == TK_NUM) {
        pos++;
        return new_node_num(t->val);
    }
    if (t->ty == TK_IDENT) {
        pos++;
        return new_node_ident(*t->input);
    }
    if (t->ty == '(') {
        pos++;
        Node *node = expr();
        t = tokens->data[pos];
        if (t->ty != ')') {
            fprintf(stderr, "Unexpected token: %s\n", t->input);
            exit(1);
        }
        pos++;
        return node;
    }
    fprintf(stderr, "Unexpected token: %s\n", t->input);
    exit(1);
}

static Node *mul() {
    Node *lhs = term();
    Token *t = tokens->data[pos];
    if (t->ty == '*') {
        pos++;
        return new_node('*', lhs, mul());
    }
    if (t->ty == '/') {
        pos++;
        return new_node('/', lhs, mul());
    }
    return lhs;
}

static Node *add() {
    Node *lhs = mul();
    Token *t = tokens->data[pos];
    if (t->ty == '+') {
        pos++;
        return new_node('+', lhs, add());
    }
    if (t->ty == '-') {
        pos++;
        return new_node('-', lhs, add());
    }
    return lhs;
}

static Node *expr() {
    Node *lhs = add();
    Token *t = tokens->data[pos];
    if (t->ty == TK_EQ) {
        pos++;
        return new_node(ND_EQ, lhs, expr());
    }
    if (t->ty == TK_NE) {
        pos++;
        return new_node(ND_NE, lhs, expr());
    }
    return lhs;
}

static Node *assign() {
    Node *lhs = expr();
    Token *t = tokens->data[pos];
    if (t->ty == '=') {
        pos++;
        return new_node('=', lhs, assign());
    }
    if (t->ty == ';') {
        pos++;
    }
    return lhs;
}

Vector *program(Vector *tokens_) {
    tokens = tokens_;
    pos = 0;

    Vector *code = new_vector();
    while (((Token *)tokens->data[pos])->ty != TK_EOF) {
        vec_push(code, assign());
    }
    return code;
}
