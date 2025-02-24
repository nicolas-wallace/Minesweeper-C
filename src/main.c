#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>
#include <math.h>
#include "../include/dataStructure.h"
#include "../include/minefield.h"
#include "../include/colors.h"

/*
—————————————————————————————————————————————————————————————————————————————————————————————————————————————————————————————
Nicolas Wallace
Contribuição: Criação e elaboração da estrutura de dados (lista encadeada bidimensional), algoritmo de recursão 
para revelar os zeros, contador de bombas adjacentes, modularização do código, criação do Makefile, debugging do 
código e manuseamento de memória.

Responsável pelas funções:
Node* getNode();
Node* createNode();
Node* createMinefield();
void freeMinefield();
void revealNodes();
void debugMinefield();
void debugPrintMinefield();
void updateBombNumber();
parte de int main();
—————————————————————————————————————————————————————————————————————————————————————————————————————————————————————————————
Yasmin Amorim
Contribuição: Revisão e contribuição para a estrutura de dados, eficiência do código, interface gráfica, estética, 
otimização do código, macros para as cores, título MINESWEEPER em ASCII, funções de leitura de dados, verificações
de entrada de usuário e UX

Responsável pelas funções:
void userPrintMinefield();
void revealMinefield();
void toggleFlag();
void limparTela();
int getBoardSize();
int getIntChosenRow();
parte de void revealNodes();
parte de int main();
—————————————————————————————————————————————————————————————————————————————————————————————————————————————————————————————
*/

int main() {
    
    int start;
    bool gameWon = false, gameLost = false;

    printf("\033[1;92m");

    // cabeçalho estilizado em ASCII
    printf("  __  __ _                                                   \n");
    printf(" |  \\/  (_)                                                  \n");
    printf(" | \\  / |_ _ __   ___  _____      _____  ___ _ __   ___ _ __ \n");
    printf(" | |\\/| | | '_ \\ / _ \\/ __\\ \\ /\\ / / _ \\/ _ \\ '_ \\ / _ \\ '__|\n");
    printf(" | |  | | | | | |  __/\\__ \\\\ V  V /  __/  __/ |_) |  __/ |   \n");
    printf(" |_|  |_|_|_| |_|\\___||___/ \\_/\\_/ \\___|\\___| .__/ \\___|_|   \n");
    printf("                                            | |               \n");
    printf("                                            |_|               \n");

    // reseta a cor para o padrão do terminal
    printf("\033[0m");

    do {
        printf(BLUE"\nDigite 1 para iniciar o jogo: "RESET);
        scanf("%d", &start);
        if (start != 1) {
            printf(RED"Entrada invalida. Tente novamente.\n"RESET);
        }
    } while (start != 1);

    limparTela();
    printf(GREEN"O jogo comecou!\n"RESET);

    int rows, cols, bombs;
    int flagsLeft, correctlyFlagged = 0;

    // configurar tamanho do tabuleiro
    do {
        printf(CYAN"\nDigite as linhas (5-26): \n"RESET);
        scanf("%d", &rows);
        printf(CYAN"\nDigite as colunas (5-40): \n"RESET);
        scanf("%d", &cols);
    } while (rows < 5 || rows > 26 || cols < 5 || cols > 40);

    // configurar número de bombas
    int maxBombs = (rows*cols)/5;
    
    if (maxBombs < 7) {
        maxBombs = 7;
    }

    do {
        printf(RED"\nDigite o numero de bombas (7-%d): \n"RESET, maxBombs);
        scanf("%d", &bombs);
    } while (bombs < 7 || bombs > maxBombs);

    flagsLeft = bombs;

    // cria e configura o campo minado
    Node* head = createMinefield(rows, cols);
    placeBombs(head, rows, cols, bombs);
    updateBombNumber(head, rows, cols);

    int chosenRow, chosenCol;
    char charChosenRow, op;

    while (true) {
        // solicita input do jogador
        userPrintMinefield(head, rows, cols);
        printf("Escreva tudo junto, sem espacos.\n");
        printf("Digite # para marcar ou desmarcar uma bomba, e ! para desmarcar uma bandeira.");
        printf("\nEscolha uma celula: ");
        printf("\n(ex. #A1 para marcar/desmarcar bandeira, !A1 para desmarcar bandeira, A1 para explorar)\n");
        
        char input[10];  // buffer para entrada do usuário
        scanf("%s", input);
        
        // determina a operação e a coordenada
        char op = '\0'; // padrão: sem operação explícita (explorar)
        char charChosenRow;
        int chosenCol;
        
        if (input[0] == '#' || input[0] == '!') {
            op = input[0]; // guarda a operação
            // Extraímos a letra e o número a partir do segundo caractere
            charChosenRow = input[1];
            chosenCol = atoi(&input[2]); // Converte o resto da string para inteiro
        } else {
            // Nenhuma operação especial, só a coordenada (para explorar)
            charChosenRow = input[0];
            chosenCol = atoi(&input[1]); // Converte o resto da string para inteiro
        }
        
        // Ajustando índice para lógica interna
        chosenCol--;
        chosenRow = getIntChosenRow(charChosenRow);
        
        if (chosenRow < 0 || chosenRow >= rows || chosenCol < 0 || chosenCol >= cols) {
            printf("Posição invalida, escolha dentro da grade.\n");
            continue;
        }
        
        Node* selected = getNode(head, chosenRow, chosenCol);
        
        if (op == '#') {
            // pra ver se já tava marcado
            bool wasFlagged = selected->isFlagged;
            
            toggleFlag(head, chosenRow, chosenCol);
            
            // atualiza contadores após mudar o estado da flag
            if (!wasFlagged && selected->isBomb) {
                // uma bomba foi marcada corretamente
                flagsLeft--;
                correctlyFlagged++;
            } 
            else if (wasFlagged && selected->isBomb) {
                // uma bomba foi desmarcada
                flagsLeft++;
                correctlyFlagged--;
            }
            
            // verifica condição de vitória e encerra o laço
            if (correctlyFlagged == bombs && flagsLeft == 0) {
                userPrintMinefield(head, rows, cols);
                printf("\nVoce venceu!\n");
                gameWon = true;
                break;
            }
        } else if (op == '!') {
            // desmarcar bandeira
            if (selected->isFlagged) {
                toggleFlag(head, chosenRow, chosenCol);
                // se for uma bomba, atualiza contadores
                if (selected->isBomb) {
                    flagsLeft++;
                    correctlyFlagged--;
                }
            } else {
                printf("Essa celula não tem bandeira para remover.\n");
            }
        } else {
            // explorar (quando não tem operação específica)
            exploreNode(head, chosenRow, chosenCol);
            
            if (selected->isBomb) {
                // usuário perde o jogo e revela o tabuleiro completo
                userPrintMinefield(head, rows, cols);
                printf("\nVoce perdeu!\n");
                revealMinefield(head, rows, cols);
                gameLost = true;
                break;
            }
        }
    }

    // tela de saída

    if (gameLost || gameWon) {
        while (true) {
            int esc;
            printf("Digite 1 para sair.");
            scanf("%d", &esc);
            if (esc == 1) {
                break;
            }
        }
    }

    // limpa a memória, só recebe o primeiro termo da lista e vai limpando de um por um
    freeMinefield(head);
    return EXIT_SUCCESS;
}
