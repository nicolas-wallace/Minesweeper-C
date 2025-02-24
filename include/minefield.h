#ifndef MINEFIELD_H
#define MINEFIELD_H

#include "dataStructure.h"

void toggleFlag(Node* head, int row, int col);
void exploreNode(Node* head, int chosenRow, int chosenCol);
void placeBombs(Node* head, int rows, int cols, int bombCount);
void updateBombNumber(Node* head, int rows, int cols);
void debugMinefield(Node* head);
void debugPrintMinefield(Node* head);
void userPrintMinefield(Node* head, int r, int c);
void freeMinefield(Node* head);
void revealNodes(Node* node);
void limparTela();
void revealMinefield(Node* head, int r, int c);

int getBoardSize();
int getIntChosenRow(char c);

#endif