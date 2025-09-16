#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

// NIVEL INTERMEDIARIO
        /*    Hall de Entrada
                    /              \
            Sala de Estar         Cozinha
            /        \               \
        Biblioteca   Jardim          Adega */

// -------------------------
// Estrutura de uma sala
// -------------------------
typedef struct Sala {
    char nome[50];
    char pista[100];  // Pista opcional
    struct Sala *esquerda;
    struct Sala *direita;
} Sala;

// -------------------------
// Nó da árvore de pistas (BST)
// -------------------------
typedef struct PistaNode {
    char pista[100];
    struct PistaNode *esquerda;
    struct PistaNode *direita;
} PistaNode;

// --------------------------------------------------
// Função: criarSala()
// Cria uma sala com nome e pista associados
// --------------------------------------------------
Sala* criarSala(const char *nome, const char *pista) {
    Sala *nova = (Sala*) malloc(sizeof(Sala));
    if (!nova) {
        printf("Erro ao alocar memória para sala!\n");
        exit(1);
    }
    strncpy(nova->nome, nome, sizeof(nova->nome) - 1);
    nova->nome[sizeof(nova->nome) - 1] = '\0';

    if (pista != NULL)
        strncpy(nova->pista, pista, sizeof(nova->pista) - 1);
    else
        nova->pista[0] = '\0'; // Sem pista

    nova->pista[sizeof(nova->pista) - 1] = '\0';
    nova->esquerda = NULL;
    nova->direita = NULL;
    return nova;
}

// --------------------------------------------------
// Função: inserirPista()
// Insere uma pista na BST em ordem alfabética
// --------------------------------------------------
PistaNode* inserirPista(PistaNode *raiz, const char *pista) {
    if (raiz == NULL) {
        PistaNode *nova = (PistaNode*) malloc(sizeof(PistaNode));
        if (!nova) {
            printf("Erro ao alocar memória para pista!\n");
            exit(1);
        }
        strncpy(nova->pista, pista, sizeof(nova->pista) - 1);
        nova->pista[sizeof(nova->pista) - 1] = '\0';
        nova->esquerda = nova->direita = NULL;
        return nova;
    }

    if (strcmp(pista, raiz->pista) < 0)
        raiz->esquerda = inserirPista(raiz->esquerda, pista);
    else if (strcmp(pista, raiz->pista) > 0)
        raiz->direita = inserirPista(raiz->direita, pista);
    // Duplicadas são ignoradas
    return raiz;
}

// --------------------------------------------------
// Função: explorarSalasComPistas()
// Exploração da mansão + coleta de pistas
// --------------------------------------------------
void explorarSalasComPistas(Sala *atual, PistaNode **raizPistas) {
    char escolha;

    while (atual != NULL) {
        printf("\n=== Jogo Detective Quest [NÍVEL INTERMEDIÁRIO] ===\n");
        printf("\nVocê está em: %s\n", atual->nome);

        if (strlen(atual->pista) > 0) {
            printf("[OK] -> Pista encontrada: \"%s\"\n", atual->pista);
            *raizPistas = inserirPista(*raizPistas, atual->pista);
        } else {
            printf("Nenhuma pista nesta sala.\n");
        }

        printf("\nEscolha o caminho:\n");
        if (atual->esquerda != NULL) printf("  (e) [<-] esquerda - %s\n", atual->esquerda->nome);
        if (atual->direita != NULL) printf("  (d) [->] direita  - %s\n", atual->direita->nome);
        printf("  (s) Sair da mansão\n");

        printf("Digite sua escolha: ");
        scanf(" %c", &escolha);
        escolha = tolower(escolha);

        if (escolha == 'e' && atual->esquerda != NULL) {
            atual = atual->esquerda;
        } else if (escolha == 'd' && atual->direita != NULL) {
            atual = atual->direita;
        } else if (escolha == 's') {
            printf("\nVocê saiu da mansão!\n\n");
            break;
        } else {
            printf("\n[X] -> Opção inválida!\n");
        }
    }
}

// --------------------------------------------------
// Função: exibirPistas()
// Exibe as pistas coletadas em ordem alfabética
// --------------------------------------------------
void exibirPistas(PistaNode *raiz, int *contador) {
    if (raiz == NULL) return;
    exibirPistas(raiz->esquerda, contador);
    printf("%d. %s\n", ++(*contador), raiz->pista);
    exibirPistas(raiz->direita, contador);
}

// --------------------------------------------------
// Função: liberarSalas()
// Libera memória da árvore de salas
// --------------------------------------------------
void liberarSalas(Sala *atual) {
    if (atual == NULL) return;
    liberarSalas(atual->esquerda);
    liberarSalas(atual->direita);
    free(atual);
}

// --------------------------------------------------
// Função: liberarPistas()
// Libera memória da árvore de pistas
// --------------------------------------------------
void liberarPistas(PistaNode *raiz) {
    if (raiz == NULL) return;
    liberarPistas(raiz->esquerda);
    liberarPistas(raiz->direita);
    free(raiz);
}

// --------------------------------------------------
// Função principal: main()
// --------------------------------------------------
int main() {
    // Criação das salas da mansão
    Sala *hall       = criarSala("Hall de Entrada", "A porta estava destrancada.");
    Sala *salaEstar  = criarSala("Sala de Estar",    "Havia um copo quebrado no chão.");
    Sala *cozinha    = criarSala("Cozinha",          "A faca sumiu do suporte.");
    Sala *biblioteca = criarSala("Biblioteca",       "Um livro estava fora do lugar.");
    Sala *jardim     = criarSala("Jardim",           NULL);  // Sem pista
    Sala *adega      = criarSala("Adega",            "Cheiro forte de perfume.");

    // Conectando as salas (formato de árvore)
    hall->esquerda = salaEstar;
    hall->direita  = cozinha;

    salaEstar->esquerda = biblioteca;
    salaEstar->direita  = jardim;

    cozinha->direita = adega;

    // BST de pistas
    PistaNode *pistas = NULL;

    // Exploração
    explorarSalasComPistas(hall, &pistas);

    // Exibir pistas coletadas
    printf("============================\n");
    printf("\n=== Pistas Coletadas ===\n\n");
    if (pistas) {
        int contador = 0;
        exibirPistas(pistas, &contador);
        printf("\nTotal de pistas achadas: [%d]\n", contador);
    } else {
        printf("Nenhuma pista foi coletada.\n");
    }

    // Liberação de memória
    liberarSalas(hall);
    liberarPistas(pistas);

    return 0;
}
