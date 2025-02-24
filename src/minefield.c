#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>
#include <math.h>
#include "../include/dataStructure.h"
#include "../include/colors.h"

void limparTela() {
    #ifdef _WIN32
        system("cls");  // Para Windows
    #else
        system("clear"); // Para Linux/macOS
    #endif
}

void placeBombs(Node* head, int rows, int cols, int bombCount) {
    srand(time(NULL));
    int placed = 0;
    int r, c;
    
    /*aqui é o seguinte, escolhe coluna e linha aleatória, 
    percorre a lista bidimensional até a posição aleatória da matriz A r,c
    bota uma bomba lá, se a contagem de bombas for igual ao quanto de bomba a gente quer, 
    então o código sai do laço */ 

    while (placed < bombCount) {
        r = rand() % rows;
        c = rand() % cols;
        
        Node* temp = head;

        for (int i = 0; i < r; i++) {
            temp = temp->down;
        }

        for (int j = 0; j < c; j++) { 
            temp = temp->right;
        }
        
        if (!temp->isBomb) {
            temp->isBomb = true;
            placed++;
        }
    }
}

void updateBombNumber(Node* head, int rows, int cols) {

    // mesma ladainha de sempre, percorre a lista encadeada bidimensional e verifica cada quadradinho do tabuleiro individualmente

    Node* rowStart = head;
    while (rowStart != NULL) {
        Node* temp = rowStart;
        while (temp != NULL) {
            if (temp->isBomb == false) {

                /* esse aqui é meio óbvio, mas ele verifica se tem o ponteiro e se o nó para onde o ponteiro aponta tem uma bomba
                talvez tenha uma forma melhor de fazer isso, mas não se mexe no q tá funcionando kkkkkkk */

                int count = 0;
                if (temp->up && temp->up->isBomb) {
                    count++;
                }
                if (temp->down && temp->down->isBomb) {
                    count++;
                }
                if (temp->left && temp->left->isBomb) {
                    count++;
                }
                if (temp->right && temp->right->isBomb) {
                    count++;
                }
                if (temp->up && temp->up->left && temp->up->left->isBomb) {
                    count++;
                }
                if (temp->up && temp->up->right && temp->up->right->isBomb) {
                    count++;
                }
                if (temp->down && temp->down->left && temp->down->left->isBomb) {
                    count++;
                }
                if (temp->down && temp->down->right && temp->down->right->isBomb) {
                    count++;
                }
                temp->bombCount = count;
            }
            temp = temp->right;
        }
        rowStart = rowStart->down;
    }
}

/* isso aqui foi feito pq é um saco entender como fazer os ponteiros up, down apontar para onde eu quero. É mais uma forma de
ver em detalhes como cada nódulo se comporta */ 

void debugMinefield(Node* head) {
    Node* rowStart = head;
    while (rowStart != NULL) {
        Node* temp = rowStart;
        while (temp != NULL) {
            printf("[%p] bomb count: %d | isbomb: %d | up: %p | down: %p | right: %p | left: %p\n",
                   (void*)temp, temp->bombCount, temp->isBomb, 
                   (void*)temp->up, (void*)temp->down, 
                   (void*)temp->right, (void*)temp->left);
            temp = temp->right;
        }
        printf("\n");
        rowStart = rowStart->down;
    }
}

void freeMinefield(Node* head) {
    Node* rowStart = head;
    while (rowStart != NULL) {
        Node* temp = rowStart;
        while (temp != NULL) {
            Node* next = temp->right;
            free(temp);
            temp = next;
        }
        Node* nextRow = rowStart->down;
        rowStart = nextRow;
    }
}

void debugPrintMinefield(Node* head) {

    // aqui mostra o campo minado no console, não tá no padrão do miguel pq a gente decidiu fazer uma interface com o GTK
    // se quiser, eu posso fazer no padrão dele mesmo assim

    Node* rowStart = head;
    while (rowStart != NULL) {

        // yasmin, aqui ele escolhe a primeira linha e vai verificar até o final dela, pq o ponteiro à direita do último nó
        // da linha é NULL, então ele para e sai do laço

        Node* temp = rowStart;
        while (temp != NULL) {
            if (temp->isBomb) {
                printf(" * ");
            } else {
                printf(" %d ", temp->bombCount);
            }
            temp = temp->right;
        }

        // aí aqui ele vai para a próxima linha e pega o nó que o ponteiro inferior de rowStart aponta, que seria a linha inferior,
        // e então define rowStart como o novo começo da próxima linha

        printf("\n");
        rowStart = rowStart->down;
    }
}

void userPrintMinefield(Node* head, int r, int c) {
    int i, j = 0;
    char str[26] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";

    // printa as dezenas das colunas
    printf("   ");
    for (i = 1; i <= c; i++) {
        if (i >= 10) {
            printf("%d ", i / 10);
        } else {
            printf("  ");
        }
    }
    printf("\n");

    // printa as unidades das colunas
    printf("   ");
    for (i = 1; i <= c; i++) {
        printf("%d ", i % 10);
    }
    printf("\n");

    // printa a borda superior
    printf(YELLOW"  +"RESET);
    for (i = 0; i < c; i++) {
        printf(YELLOW"=+"RESET);
    }
    printf("\n");

    Node* rowStart = head;
    while (rowStart != NULL) {
        Node* temp = rowStart;
        
        // print as letras de linha
        printf(MAGENTA"%c |"RESET, str[j]);
        
        // printa o conteúdo dos quadrados
        while (temp != NULL) {
            if (temp->isRevealed) {
                if (temp->bombCount == 0) {
                    printf(GREEN" |"RESET);
                } else {
                    printf("%d|", temp->bombCount);
                }
            } else if (temp->isFlagged) {
                printf(YELLOW"F|"RESET);
            } else {
                printf(CYAN"■|"RESET);
            }
            temp = temp->right;
        }
        printf("\n");
        
        // printa a linha horizontal que separa as linhas (rows)
        printf(YELLOW"  +"RESET);
        for (i = 0; i < c; i++) {
            printf(YELLOW"=+"RESET);
        }
        printf("\n");
        j++;
        rowStart = rowStart->down;
    }
}
void revealNodes(Node* node) {
    if (!node || node->isRevealed) return;

    node->isRevealed = true;

    /*
    Essa é a lógica recursiva por trás das células com zero bombas, tem que ir verificando cada nódulo adjacente recursivamente
    até encontrar um nódulo com bombCount > 0. Isso aqui é bizarro, só conseguimos entender e fazer em C depois que vimos 
    um vídeo online explicando algo parecido em python, só que sem os ponteiros. pfv não mexer no que está funcionando.
    */ 

    if (node->bombCount == 0) {
        revealNodes(node->up);
        revealNodes(node->down);
        revealNodes(node->left);
        revealNodes(node->right);

        if (node->up) {
            revealNodes(node->up->left);
            revealNodes(node->up->right);
        }
        
        if (node->down) {
            revealNodes(node->down->left);
            revealNodes(node->down->right);
        }
    }
}

void exploreNode(Node* head, int chosenRow, int chosenCol) {
    Node* selected = getNode(head, chosenRow, chosenCol);
    if (!selected || selected->isRevealed) {
        return;
    }

    // se for uma bomba, não faz nada. O break do laço fica fora da função
    if (selected->isBomb) {
        selected->isRevealed = true;  // Revela a bomba para mostrar ao jogador
        return;
    }
    // revela a célula e expande se for zero, não vou explicar, tá explicado já na função
    revealNodes(selected);
}

void toggleFlag(Node* head, int row, int col) {
    Node* selected = getNode(head, row, col);
    if (!selected || selected->isRevealed) return;

    selected->isFlagged = !selected->isFlagged;
}

int getIntChosenRow(char c) {
    char upper[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
    char lower[] = "abcdefghijklmnopqrstuvwxyz";
    int i;
    
    for (i = 0; i < 26; i++) {
        if (c == upper[i] || c == lower[i]) {
            return i;
        }
    }

    return -1;
}

void revealMinefield(Node* head, int r, int c) {
    int i, j = 0;
    char str[26] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";

    // printa as dezenas das colunas
    printf("   ");
    for (i = 1; i <= c; i++) {
        if (i >= 10) {
            printf("%d ", i / 10);
        } else {
            printf("  ");
        }
    }
    printf("\n");

    // printa as unidades das colunas
    printf("   ");
    for (i = 1; i <= c; i++) {
        printf("%d ", i % 10);
    }
    printf("\n");

    // printa a borda superior
    printf(YELLOW"  +"RESET);
    for (i = 0; i < c; i++) {
        printf(YELLOW"=+"RESET);
    }
    printf("\n");

    Node* rowStart = head;
    while (rowStart != NULL) {
        Node* temp = rowStart;
        
        // printa as letras de linha
        printf(MAGENTA"%c |"RESET, str[j]);
        
        // printa o conteúdo dos quadrados
        while (temp != NULL) {
            if (temp->bombCount == 0 && !temp->isBomb) {
                printf(GREEN" |"RESET);
            } else if (temp->bombCount > 0) {
                printf("%d|", temp->bombCount);
            } else if (temp->isBomb) {
                printf(RED"@|"RESET);  // Representa uma bomba com '@'
            } else {
                printf(CYAN"■|"RESET);
            }
            temp = temp->right;
        }
        printf("\n");
        
        // printa a linha horizontal que separa as linhas (rows)
        printf(YELLOW"  +"RESET);
        for (i = 0; i < c; i++) {
            printf(YELLOW"=+"RESET);
        }
        printf("\n");
        j++;
        rowStart = rowStart->down;
    }
}