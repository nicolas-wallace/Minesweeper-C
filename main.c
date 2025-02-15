#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>
#include <math.h>

typedef struct Node {
    int bombCount;
    bool isBomb;
    bool isRevealed;
    struct Node *left, *right, *up, *down;
} Node;

// definição prévia das funções só para a função main não ficar lá embaixo
// lembrar de retirar e colocar em outro arquivo depois

Node* createNode();
Node* createMinefield();
Node* getNode();

void exploreNode();
void placeBombs();
void updateBombNumber();
void debugMinefield();
void debugPrintMinefield();
void userPrintMinefield();
void freeMinefield();


int main() {
    int rows, cols, bombs;

    // configurar tamanho do tabuleiro
    do {
        printf("Digite as linhas (5-26): ");
        scanf("%d", &rows);
        printf("Digite as colunas (5-40): ");
        scanf("%d", &cols);
    } while (rows < 5 || rows > 26 || cols < 5 || cols > 40);

    // configurar número de bombas
    int maxBombs = (rows * cols) / 5;
    do {
        printf("Digite o numero de bombas (7-%d): ", maxBombs);
        scanf("%d", &bombs);
    } while (bombs < 7 || bombs > maxBombs);

    // cria e configura o campo minado
    Node* head = createMinefield(rows, cols);
    placeBombs(head, rows, cols, bombs);
    updateBombNumber(head, rows, cols);

    int chosenRow, chosenCol;
    while (true) {
        // solicita input do jogador
        printf("\nEscolha uma celula para explorar (linha coluna): ");
        scanf("%d %d", &chosenRow, &chosenCol);

        if (chosenRow < 0 || chosenRow >= rows || chosenCol < 0 || chosenCol >= cols) {
            printf("Posição invalida, escolha dentro da grade.\n");
            continue;
        }

        // explora cell escolhida
        exploreNode(head, chosenRow, chosenCol);

        // acertou uma bomba, finaliza o jogo
        Node* selected = getNode(head, chosenRow, chosenCol);
        if (selected->isBomb) {
            break;
        }
    }

    // limpa a memória, só recebe o primeiro termo da lista e vai limpando de um por um
    freeMinefield(head);
    return EXIT_SUCCESS;
}

Node* createNode() {

    /* esse aqui não tem segredo, a gente só aloca memória mesmo e cria um nódulo */

    Node* newNode = (Node*)malloc(sizeof(Node));
    newNode->bombCount = 0;
    newNode->isBomb = false;
    newNode->isRevealed = false;
    newNode->left = newNode->right = newNode->up = newNode->down = NULL;
    return newNode;
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

void updateBombNumber(Node* head) {

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

void userPrintMinefield(Node* head) {
    
    // isso aqui é para printar o campo minado escondido, ainda não está nem perto de ser terminado

    Node* rowStart = head;
    while (rowStart != NULL) {
        Node* temp = rowStart;
        while (temp != NULL) {
            printf(" # ");
            temp = temp->right;
        }
        printf("\n");
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

        if (node->up != NULL) {
            revealNodes(node->up->left);
        } else {
            revealNodes(NULL);
        }
        
        if (node->up != NULL) {
            revealNodes(node->up->right);
        } else {
            revealNodes(NULL);
        }
        
        if (node->down != NULL) {
            revealNodes(node->down->left);
        } else {
            revealNodes(NULL);
        }
        
        if (node->down != NULL) {
            revealNodes(node->down->right);
        } else {
            revealNodes(NULL);
        }
    }
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

void exploreNode(Node* head, int chosenRow, int chosenCol) {
    Node* selected = getNode(head, chosenRow, chosenCol);
    if (!selected || selected->isRevealed) return;

    // se for uma bomba, cabou o jogo
    if (selected->isBomb) {
        printf("\nVocê pisou numa bomba. Game over.\n");
        return;
    }
    // revela a célula e expande se for zero, não vou explicar, tá explicado já na função
    revealNodes(selected);

    // atualiza o tabuleiro
    Node* rowStart = head;
    printf("\nTabuleiro Atualizado:\n");

    for (int r = 0; rowStart; r++) {
        Node* temp = rowStart;
        for (int c = 0; temp; c++) {
            if (temp->isRevealed) {
                if (temp->bombCount == 0) printf(" 0 ");
                else printf(" %d ", temp->bombCount);
            } else {
                printf(" # ");
            }
            temp = temp->right;
        }
        printf("\n");
        rowStart = rowStart->down;
    }
}
