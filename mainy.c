#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>
#include <math.h>

// Cores para o terminal
#define RESET "\033[0m"
#define CYAN "\033[1;36m"
#define GREEN "\033[1;32m"
#define RED "\033[1;31m"
#define YELLOW "\033[1;33m"
#define MAGENTA "\033[35m"
#define BLUE   "\033[34m"



typedef struct Node {
    int bombCount;
    bool isBomb;
    bool isRevealed;
    bool isFlagged;
    struct Node *left, *right, *up, *down;
} Node;

// definição prévia das funções só para a função main não ficar lá embaixo
// lembrar de retirar e colocar em outro arquivo depois

Node* createNode();
Node* createMinefield(int rows, int cols);
Node* getNode(Node* head, int row, int col);

void toggleFlag(Node* head, int row, int col);
void exploreNode(Node* head, int chosenRow, int chosenCol);
void placeBombs(Node* head, int rows, int cols, int bombCount);
void updateBombNumber(Node* head, int rows, int cols);
void debugMinefield(Node* head);
void debugPrintMinefield(Node* head);
void userPrintMinefield(Node* head, int r, int c);
void freeMinefield(Node* head);
void revealNodes(Node* node);
void limparTela() {
    #ifdef _WIN32
        system("cls");  // Para Windows
    #else
        system("clear"); // Para Linux/macOS
    #endif
}

int getBoardSize();
int getIntChosenRow(char c);


int main() {
    
    int start;

    printf("\033[1;92m");

    // Cabeçalho estilizado em ASCII
    printf("  __  __ _                                                   \n");
    printf(" |  \\/  (_)                                                  \n");
    printf(" | \\  / |_ _ __   ___  _____      _____  ___ _ __   ___ _ __ \n");
    printf(" | |\\/| | | '_ \\ / _ \\/ __\\ \\ /\\ / / _ \\/ _ \\ '_ \\ / _ \\ '__|\n");
    printf(" | |  | | | | | |  __/\\__ \\\\ V  V /  __/  __/ |_) |  __/ |   \n");
    printf(" |_|  |_|_|_| |_|\\___||___/ \\_/\\_/ \\___|\\___| .__/ \\___|_|   \n");
    printf("                                           | |               \n");
    printf("                                           |_|               \n");

    // Resetar a cor para o padrão do terminal
    printf("\033[0m");

    do {
        printf(BLUE"Digite um 1 para iniciar o jogo: "RESET);
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
        printf(CYAN" \n Digite as linhas (5-26): \n"RESET);
        scanf("%d", &rows);
        printf(CYAN"\n Digite as colunas (5-40): \n"RESET);
        scanf("%d", &cols);
    } while (rows < 5 || rows > 26 || cols < 5 || cols > 40);

    // configurar número de bombas
    int maxBombs = (rows*cols)/5;
    
    if (maxBombs < 7) {
        maxBombs = 7;
    }

    do {
        printf(RED"\n Digite o numero de bombas (7-%d): \n"RESET, maxBombs);
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
        printf("Digite # para marcar uma bomba e ! para explorar.");
        printf("\nEscolha uma celula e se deseja explorar 'e' ou marcar 'f' (formato opcao-linha-coluna -- ex. #A1): ");
        
        // acredita que eu não sabia que scanf retornava um int?
        int input = scanf(" %c%c%d", &op, &charChosenRow, &chosenCol);
        if (input != 3) {
            printf("Entrada invalida. por favor, siga o formato 'opcao-linha-coluna' -- ex. #A1.\n");
            // limpa o buffer de entrada
            while ((getchar()) != '\n');
            continue;
        }

        /* 
        esse decremental aqui é pq a lógica do programa funciona com índice inicial 0, mas o do miguel começa com 1 kkkkkkkkkkkkkkk 
        então, para o usuário, ele escolhe de 1 a 40, o programa entende de 0 a 39
        */
        chosenCol--;

        // isso aqui é a mesma coisa, pq mtas funções precisam saber qual linha o usuário escolheu e manipular um char é péssimo
        chosenRow = getIntChosenRow(charChosenRow);

        if (chosenRow < 0 || chosenRow >= rows || chosenCol < 0 || chosenCol >= cols) {
            printf("Posição invalida, escolha dentro da grade.\n");
            continue;
        }

        Node* selected = getNode(head, chosenRow, chosenCol);

        if (op == '#') {
            // verifica se já tava marcado
            bool wasFlagged = selected->isFlagged;
            
            toggleFlag(head, chosenRow, chosenCol);
            
            // Atualiza contadores após mudar o estado da flag
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
                break;
            }
            
            printf("\nBandeiras restantes: %d\n", flagsLeft);
        }

        if (op == '!') {
            exploreNode(head, chosenRow, chosenCol);

            if (selected->isBomb) {
                userPrintMinefield(head, rows, cols);
                printf("\nVoce perdeu!\n");
                break;
            }
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
    newNode->isFlagged = false;
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