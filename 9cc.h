#ifndef _9CC_H_
#define _9CC_H_

enum {
    ND_NUM = 256,
    ND_IDENT,
};

typedef struct Node {
    int ty;
    struct Node *lhs;
    struct Node *rhs;
    int val;
    char name;
} Node;

void tokenize(char*);
void program();
void gen(Node*);

extern Node *code[100];

#endif // _9CC_H_
