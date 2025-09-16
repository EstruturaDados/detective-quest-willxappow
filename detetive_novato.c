#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// NIVEL NOVATO

   /*    Hall de Entrada
              /              \
    Sala de Estar         Cozinha
     /        \               \
Biblioteca   Jardim          Adega */

// ----------------------
// Estrutura de uma sala
// ----------------------
typedef struct Sala {
    char nome[50];           // Nome do cômodo
    struct Sala *esquerda;   // Caminho à esquerda
    struct Sala *direita;    // Caminho à direita
} Sala;

// --------------------------------------------------
// Função: criarSala()
// Cria uma sala dinamicamente com o nome informado
// --------------------------------------------------
Sala* criarSala(const char *nome) {
    Sala *nova = (Sala*) malloc(sizeof(Sala));
    if (nova == NULL) {
        printf("\nErro ao alocar memória!\n\n");
        exit(1);
    }
    strncpy(nova->nome, nome, sizeof(nova->nome) - 1);
    nova->nome[sizeof(nova->nome) - 1] = '\0';  // Garante terminação
    nova->esquerda = NULL;
    nova->direita = NULL;
    return nova;
}

// --------------------------------------------------
// Função: explorarSalas()
// Permite que o jogador explore interativamente
// --------------------------------------------------
void explorarSalas(Sala *atual) {
    char escolha;

    while (atual != NULL) {
        printf("\n === Jogo Detetive Quest - [NIVEL NOVATO] === \n");
        printf("\nVocê está em: %s\n\n", atual->nome);

        // Caso seja uma sala sem caminhos (folha da árvore)
        if (atual->esquerda == NULL && atual->direita == NULL) {
            printf("Não há mais caminhos a seguir. Fim da exploração!\n");
            break;
        }

        printf("Escolha o caminho:\n");
        if (atual->esquerda != NULL) printf("  (e) [<-] esquerda - Ir para %s\n", atual->esquerda->nome);
        if (atual->direita != NULL) printf("  (d) [->] direita - Ir para %s\n", atual->direita->nome);
        printf("  (s) Sair do jogo\n");

        printf("Digite sua escolha: ");
        scanf(" %c", &escolha);

        if (escolha == 'e' && atual->esquerda != NULL) {
            atual = atual->esquerda;
        } else if (escolha == 'd' && atual->direita != NULL) {
            atual = atual->direita;
        } else if (escolha == 's') {
            printf("\nVocê saiu da exploração.\n\n");
            break;
        } else {
            printf("\nOpção inválida! Tente novamente.\n");
        }
    }
}
// Função: liberarSalas()
// Limpeza no final a alocação de memoria
void liberarSalas(Sala *atual) {
    if (atual == NULL) return;
    liberarSalas(atual->esquerda);
    liberarSalas(atual->direita);
    free(atual);
}

// --------------------------------------------------
// Função principal: main()
// Monta o mapa da mansão e inicia a exploração
// --------------------------------------------------
int main() {
    // Criando salas (árvore fixa, pré-definida)
    Sala *hall       = criarSala("Hall de Entrada");
    Sala *salaEstar  = criarSala("Sala de Estar");
    Sala *cozinha    = criarSala("Cozinha");
    Sala *biblioteca = criarSala("Biblioteca");
    Sala *jardim     = criarSala("Jardim");
    Sala *adega      = criarSala("Adega");

    // Montando o mapa (ligando os nós)
    hall->esquerda = salaEstar;
    hall->direita = cozinha;

    salaEstar->esquerda = biblioteca;
    salaEstar->direita = jardim;

    cozinha->direita = adega;

    // Iniciando a exploração
    explorarSalas(hall);

    liberarSalas(hall);

    return 0;
}
