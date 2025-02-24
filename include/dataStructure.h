#ifndef DATASTRUCTURE_H
#define DATASTRUCTURE_H

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

typedef struct Node {
    int bombCount;
    bool isBomb;
    bool isRevealed;
    bool isFlagged;
    struct Node *left, *right, *up, *down;
} Node;


Node* getNode(Node* head, int row, int col);
Node* createNode();
Node* createMinefield(int rows, int cols);

#endif