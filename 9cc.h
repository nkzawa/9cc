#ifndef _9CC_H_
#define _9CC_H_

enum {
    ND_NUM = 256,
    ND_IDENT,
    ND_EQ,
    ND_NE,
};

typedef struct {
    void **data;
    int capacity;
    int len;
} Vector;

typedef struct {
    Vector *keys;
    Vector *vals;
} Map;

typedef struct Node {
    int ty;
    struct Node *lhs;
    struct Node *rhs;
    int val;
    char name;
} Node;

Vector *new_vector();
void vec_push(Vector*, void*);

Map *new_map();
void map_put(Map*, char*, void*);
void *map_get(Map*, char*);

void tokenize(char*);
void program();
void gen(Node*);
void runtest();

extern Node *code[100];

#endif // _9CC_H_
