#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>
#include <math.h>

typedef struct Node {
    int bombCount;
    bool isBomb;
    struct Node *left, *right, *up, *down;
} Node;

Node* createNode();
Node* createMinefield();
void placeBombs();
void updateBombNumber();
void debugMinefield();
void printMinefield();
void freeMinefield();

int main() {
    int rows = 0, cols = 0, bombs = 0;
    
    
    while (rows < 5 || rows > 26 || cols < 5 || cols > 40) {
        printf("Digite as linhas (5-26): ");
        scanf("%d", &rows);
        printf("Digite as colunas (5-40): ");
        scanf("%d", &cols);
        if (rows < 5 || rows > 26 || cols < 5 || cols > 40) {
            printf("Valores inválidos!\n");
        }
    }

    int maxBombs = floor((rows * cols) /5);
    while (bombs < 7 || bombs > maxBombs) {
        printf("Digite o número de bombas (7-%d): ", maxBombs);
        scanf("%d", &bombs);
        if (bombs < 7 || bombs > maxBombs) {
            printf("Número de bombas inválido!\n");
        }
    }

    Node* head = createMinefield(rows, cols); // é sempre bom saber onde a lista começa
    placeBombs(head, rows, cols, bombs);
    updateBombNumber(head, rows, cols);
    debugMinefield(head, rows, cols);
    printMinefield(head, rows, cols);
    freeMinefield(head);

    return EXIT_SUCCESS;
}

Node* createNode() {

    // esse aqui não tem segredo, a gente só aloca memória mesmo e cria um nódulo

    Node* newNode = (Node*)malloc(sizeof(Node));
    newNode->bombCount = 0;
    newNode->isBomb = false;
    newNode->left = newNode->right = newNode->up = newNode->down = NULL;
    return newNode;
}

void placeBombs(Node* head, int rows, int cols, int bombCount) {
    srand(time(NULL));
    int placed = 0;
    int r, c;
    
    // aqui é o seguinte, escolhe coluna e linha aleatória, percorre a lista bidimensional até a posição aleatória da matriz A r,c
    // bota uma bomba lá, se a contagem de bombas for igual ao quanto de bomba a gente quer, o código said o laço

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

                // esse aqui é meio óbvio, mas ele verificar se tem o ponteiro e se o nó para onde o ponteiro aponta tem uma bomba
                // talvez tenha uma forma melhor de fazer isso, mas não se mexe no q tá funcionando kkkkkkk
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

// isso aqui foi feito pq é um saco entender como fazer os ponteiros up, down apontar para onde eu quero. É mais uma forma de
// ver em detalhes como cada nódulo se comporta

void debugMinefield(Node* head, int rows, int cols) {
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

void printMinefield(Node* head, int rows, int cols) {

    // aqui mostra o campo minado no console, não tá no padrão do miguel pq a gente decidiu fazer uma interface no GTK
    // se quiser, eu posso fazer no padrão dele mesmo assim

    Node* rowStart = head;
    while (rowStart != NULL) {
        Node* temp = rowStart;
        while (temp != NULL) {
            if (temp->isBomb) {
                printf(" * ");
            } else {
                printf(" %d ", temp->bombCount);
            }
            temp = temp->right;
        }
        printf("\n");
        rowStart = rowStart->down;
    }
}
