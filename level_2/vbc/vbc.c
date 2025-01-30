#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

typedef struct node {
    enum {
        ADD,
        MULTI,
        VAL
    } type;
    int val;
    struct node *l;
    struct node *r;
} node;

node *new_node(node n) {
    node *ret = calloc(1, sizeof(n));
    if (!ret)
        return (NULL);
    *ret = n; 
    return (ret);
}

void destroy_tree(node *n) {
    if (!n)
        return;
    if (n->type != VAL) {
        destroy_tree(n->l);
        destroy_tree(n->r);
    }
    free(n);
}
// exit a ajouter
void unexpected(char c) {
    if (c)
        printf("Unexpected token '%c'\n", c);
    else
        printf("Unexpected end of file\n");
    exit(1);
}

int accept(char **s, char c)
{
    if (**s == c) 
    {
        (*s)++;
        return (1);
    }
    return (0);
}

int expect(char **s, char c) {
    if (accept(s, c))
        return (1);
    unexpected(**s ? **s : '\0');
    return (0);
}

// Forward declarations
node *parse_expr(char **s);
node *parse_term(char **s);
node *parse_factor(char **s);

node *parse_expr(char **s) {
    node *left = parse_term(s);
    while (**s == '+') {
        (*s)++;
        node *right = parse_term(s);
        left = new_node((node){ADD, 0, left, right});
    }
    return left;
}

node *parse_term(char **s) {
    node *left = parse_factor(s);
    while (**s == '*') {
        (*s)++;
        node *right = parse_factor(s);
        left = new_node((node){MULTI, 0, left, right});
    }
    return left;
}

node *parse_factor(char **s) {
    if (isdigit(**s)) {
        int value = **s - '0';
        (*s)++;
        return new_node((node){VAL, value, NULL, NULL});
    } 
    else if (accept(s, '(')) 
    {
        node *expr = parse_expr(s);
        expect(s, ')');
        return expr;
    } else 
    {
        unexpected(**s);
    }
    return NULL; // Unreachable, for safety
}

int eval_tree(node *tree) {
    switch (tree->type) {
        case ADD:
            return eval_tree(tree->l) + eval_tree(tree->r);
        case MULTI:
            return eval_tree(tree->l) * eval_tree(tree->r);
        case VAL:
            return tree->val;
    }
    return 0; // Unreachable
}

int main(int argc, char **argv) {
    if (argc != 2)
        return (1);
    char *s = argv[1];
    node *tree = parse_expr(&s);
    if (*s != '\0') { // Ensure the entire input is consumed
        destroy_tree(tree);
        unexpected(*s);
    }
    printf("%d\n", eval_tree(tree));
    destroy_tree(tree);
    return 0;
}