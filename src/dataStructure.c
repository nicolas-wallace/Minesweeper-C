#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>
#include <math.h>
#include "../include/dataStructure.h"


Node* createNode() {

    /* esse aqui não tem segredo, a gente só aloca memória mesmo e cria um nódulo */

    Node* newNode = (Node*)malloc(sizeof(Node));
    newNode->bombCount = 0;
    newNode->isBomb = false;
    newNode->isRevealed = false;
    newNode->isFlagged = false;
    newNode->left = newNode->right = newNode->up = newNode->down = NULL;
    return newNode;
}

Node* createMinefield(int rows, int cols) {
    Node* head = createNode(); // nó inicial
    Node* temp = head; // ponteiro temporário para percorrer a linha atual
    Node* prevRowStart = head; // primeiro nó da linha anterior

    // cria a primeira linha do tabuleiro
    for (int j = 1; j < cols; j++) {
        Node* newNode = createNode();
        temp->right = newNode;
        newNode->left = temp;
        temp = newNode;
    }

    // cria todas as outras linhas do tabuleiro
    for (int i = 1; i < rows; i++) {
        // cria o primeiro nó da nova linha e conecta à linha anterior
        Node* newRowStart = createNode();
        prevRowStart->down = newRowStart;
        newRowStart->up = prevRowStart;

        temp = newRowStart;
        Node* prevRowTemp = prevRowStart->right;

        // cria os nós da nova linha os conecta
        for (int j = 1; j < cols; j++) {
            Node* newNode = createNode();
            temp->right = newNode;
            newNode->left = temp;

            // conecta com a linha de cima
            if (prevRowTemp != NULL) {
                prevRowTemp->down = newNode;
                newNode->up = prevRowTemp;
                prevRowTemp = prevRowTemp->right;
            }
            temp = newNode;
        }
        prevRowStart = newRowStart; // atualiza a linha anterior com a nova
    }

    return head;
}

Node* getNode(Node* head, int row, int col) {
    Node* temp = head;
    for (int i = 0; i < row && temp; i++) {
        temp = temp->down;
    }
    for (int j = 0; j < col && temp; j++) {
        temp = temp->right;
    }

    return temp;
}