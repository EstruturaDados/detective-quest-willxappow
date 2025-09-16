#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
// NIVEL MESTRE

/*                      [Hall de Entrada]
                         /              \
              [Sala de Estar]           [Cozinha]
               /          \              /      \
      [Biblioteca]       [Jardim]  [Escritório] [Adega]
         /
   [Quarto]          */

/* --------------------------
   Tipos e estruturas
   -------------------------- */

/* Estrutura de um cômodo (Sala) da mansão */
typedef struct Sala {
    char nome[64];
    char pista[128];         
    int pista_coletada;      
    struct Sala *esquerda;
    struct Sala *direita;
} Sala;

/* Nó da BST de pistas */
typedef struct PistaNode {
    char pista[128];
    struct PistaNode *esquerda;
    struct PistaNode *direita;
} PistaNode;

/* Nó da lista encadeada da hash */
typedef struct HashNode {
    char pista[128];     
    char suspeito[64];   
    struct HashNode *proximo;
} HashNode;

/* Tabela hash */
typedef struct HashTable {
    int tamanho;
    HashNode **buckets;
} HashTable;

/* --------------------------
   Assinaturas
   -------------------------- */
Sala* criarSala(const char *nome, const char *pista);
void explorarSalas(Sala *raiz, PistaNode **raizPistas, HashTable *ht);

PistaNode* inserirPista(PistaNode *raiz, const char *pista);
PistaNode* adicionarPista(PistaNode *raiz, const char *pista);

void inserirNaHash(HashTable *ht, const char *pista, const char *suspeito);
const char* encontrarSuspeito(HashTable *ht, const char *pista);

void verificarSuspeitoFinal(PistaNode *raizPistas, HashTable *ht, const char *acusado);

HashTable* criarHashTable(int tamanho);
void liberarHashTable(HashTable *ht);
unsigned int hashString(const char *s, int modulo);
void exibirPistasInOrder(PistaNode *raiz, int *contador);
void liberarPistas(PistaNode *raiz);
void liberarSalas(Sala *atual);
char *trim_newline(char *s);
void listarSuspeitos(HashTable *ht);
void pause();


// --- Pausa para usuario ver informações ---
void pause() {
    printf("\nPressione ENTER para continuar...");
    getchar();
}

/* --------------------------
   Implementação
   -------------------------- */

Sala* criarSala(const char *nome, const char *pista) {
    Sala *s = (Sala*) malloc(sizeof(Sala));
    if (!s) { fprintf(stderr, "Falha alocar Sala\n"); exit(EXIT_FAILURE); }
    strncpy(s->nome, nome, sizeof(s->nome)-1);
    s->nome[sizeof(s->nome)-1] = '\0';
    if (pista && pista[0] != '\0') {
        strncpy(s->pista, pista, sizeof(s->pista)-1);
        s->pista[sizeof(s->pista)-1] = '\0';
    } else {
        s->pista[0] = '\0';
    }
    s->pista_coletada = 0;
    s->esquerda = s->direita = NULL;
    return s;
}

PistaNode* inserirPista(PistaNode *raiz, const char *pista) {
    if (pista == NULL || pista[0] == '\0') return raiz;
    if (raiz == NULL) {
        PistaNode *n = (PistaNode*) malloc(sizeof(PistaNode));
        if (!n) { fprintf(stderr,"Erro alocar PistaNode\n"); exit(EXIT_FAILURE); }
        strncpy(n->pista, pista, sizeof(n->pista)-1);
        n->pista[sizeof(n->pista)-1] = '\0';
        n->esquerda = n->direita = NULL;
        return n;
    }
    int cmp = strcmp(pista, raiz->pista);
    if (cmp < 0) raiz->esquerda = inserirPista(raiz->esquerda, pista);
    else if (cmp > 0) raiz->direita = inserirPista(raiz->direita, pista);
    return raiz;
}

PistaNode* adicionarPista(PistaNode *raiz, const char *pista) {
    return inserirPista(raiz, pista);
}

HashTable* criarHashTable(int tamanho) {
    HashTable *ht = (HashTable*) malloc(sizeof(HashTable));
    if (!ht) { fprintf(stderr,"Erro alocar HashTable\n"); exit(EXIT_FAILURE); }
    ht->tamanho = tamanho;
    ht->buckets = (HashNode**) calloc(tamanho, sizeof(HashNode*));
    if (!ht->buckets) { fprintf(stderr,"Erro alocar buckets\n"); exit(EXIT_FAILURE); }
    return ht;
}

unsigned int hashString(const char *s, int modulo) {
    unsigned long h = 5381;
    int c;
    while ((c = (unsigned char)*s++))
        h = ((h << 5) + h) + c;
    return (unsigned int)(h % modulo);
}

void inserirNaHash(HashTable *ht, const char *pista, const char *suspeito) {
    if (!ht || !pista) return;
    unsigned int idx = hashString(pista, ht->tamanho);
    HashNode *cur = ht->buckets[idx];
    while (cur) {
        if (strcmp(cur->pista, pista) == 0) {
            strncpy(cur->suspeito, suspeito, sizeof(cur->suspeito)-1);
            cur->suspeito[sizeof(cur->suspeito)-1] = '\0';
            return;
        }
        cur = cur->proximo;
    }
    HashNode *n = (HashNode*) malloc(sizeof(HashNode));
    strncpy(n->pista, pista, sizeof(n->pista)-1);
    n->pista[sizeof(n->pista)-1] = '\0';
    strncpy(n->suspeito, suspeito, sizeof(n->suspeito)-1);
    n->suspeito[sizeof(n->suspeito)-1] = '\0';
    n->proximo = ht->buckets[idx];
    ht->buckets[idx] = n;
}

const char* encontrarSuspeito(HashTable *ht, const char *pista) {
    if (!ht || !pista) return NULL;
    unsigned int idx = hashString(pista, ht->tamanho);
    HashNode *cur = ht->buckets[idx];
    while (cur) {
        if (strcmp(cur->pista, pista) == 0) return cur->suspeito;
        cur = cur->proximo;
    }
    return NULL;
}

void explorarSalas(Sala *raiz, PistaNode **raizPistas, HashTable *ht) {
    Sala *atual = raiz;
    char entrada[32];

    printf("\n=== Detective Quest - [NIVEL MESTRE] ===\n");
    while (atual != NULL) {
        printf("\nVocê entra em: %s\n", atual->nome);

        if (strlen(atual->pista) > 0) {
            if (!atual->pista_coletada) {
                printf(">>> Você encontrou uma pista: \"%s\"\n", atual->pista);
                const char *sus = encontrarSuspeito(ht, atual->pista);
                if(sus) printf(">>> Esta pista aponta para: %s\n", sus);
                *raizPistas = inserirPista(*raizPistas, atual->pista);
                atual->pista_coletada = 1;
            } else {
                printf("Esta pista já foi coletada anteriormente: \"%s\".\n", atual->pista);
                const char *sus = encontrarSuspeito(ht, atual->pista);
                if(sus) printf(">>> Esta pista aponta para: %s\n", sus);
            }
        } else {
            printf("Não há pistas visíveis nesta sala.\n");
        }

        printf("\nCaminhos disponíveis:\n");
        if (atual->esquerda) printf("  (e) esquerda -> %s\n", atual->esquerda->nome);
        if (atual->direita)  printf("  (d) direita  -> %s\n", atual->direita->nome);
        printf("  (l) listar suspeitos conhecidos\n");
        printf("  (s) sair da mansão\n");
        printf("Escolha: ");

        if (!fgets(entrada, sizeof(entrada), stdin)) return;
        char c = tolower((unsigned char)entrada[0]);
        if (c == 'e' && atual->esquerda) atual = atual->esquerda;
        else if (c == 'd' && atual->direita) atual = atual->direita;
        else if (c == 's') { printf("Você deixou a mansão.\n"); break; }
        else if (c == 'l') listarSuspeitos(ht);
        else printf("Opção inválida ou caminho inexistente.\n");
    }
}

void exibirPistasInOrder(PistaNode *raiz, int *contador) {
    if (!raiz) return;
    exibirPistasInOrder(raiz->esquerda, contador);
    printf("%d. %s\n", ++(*contador), raiz->pista);
    exibirPistasInOrder(raiz->direita, contador);
}

/* função auxiliar global para contagem */
int contarPistasSuspeito(PistaNode *raiz, HashTable *ht, const char *acusado) {
    if (!raiz) return 0;
    int count = 0;
    count += contarPistasSuspeito(raiz->esquerda, ht, acusado);
    const char *s = encontrarSuspeito(ht, raiz->pista);
    if (s != NULL && strcasecmp(s, acusado) == 0) count++;
    count += contarPistasSuspeito(raiz->direita, ht, acusado);
    return count;
}

void verificarSuspeitoFinal(PistaNode *raizPistas, HashTable *ht, const char *acusado) {
    if (!acusado || strlen(acusado) == 0) {
        printf("Nome inválido para acusado.\n");
        return;
    }
    int contar = contarPistasSuspeito(raizPistas, ht, acusado);

    printf("\n--- Veredicto ---\n");
    if (contar >= 2) {
        printf("Acusação válida! \"%s\" tem %d pista(s) que o ligam ao crime.\n", acusado, contar);
    } else if (contar == 1) {
        printf("Acusação fraca: apenas 1 pista aponta para \"%s\".\n", acusado);
    } else {
        printf("Nenhuma pista coletada aponta para \"%s\".\n", acusado);
    }
}

void liberarPistas(PistaNode *raiz) {
    if (!raiz) return;
    liberarPistas(raiz->esquerda);
    liberarPistas(raiz->direita);
    free(raiz);
}

void liberarSalas(Sala *atual) {
    if (!atual) return;
    liberarSalas(atual->esquerda);
    liberarSalas(atual->direita);
    free(atual);
}

void liberarHashTable(HashTable *ht) {
    if (!ht) return;
    for (int i = 0; i < ht->tamanho; ++i) {
        HashNode *cur = ht->buckets[i];
        while (cur) {
            HashNode *next = cur->proximo;
            free(cur);
            cur = next;
        }
    }
    free(ht->buckets);
    free(ht);
}

char *trim_newline(char *s) {
    if (!s) return s;
    size_t L = strlen(s);
    if (L > 0 && s[L-1] == '\n') s[L-1] = '\0';
    return s;
}

void listarSuspeitos(HashTable *ht) {
    if (!ht) return;
    char nomes[64][64];
    int count = 0;
    for (int i = 0; i < ht->tamanho; ++i) {
        HashNode *cur = ht->buckets[i];
        while (cur) {
            int found = 0;
            for (int j = 0; j < count; ++j)
                if (strcasecmp(nomes[j], cur->suspeito) == 0) { found = 1; break; }
            if (!found) {
                strncpy(nomes[count], cur->suspeito, sizeof(nomes[count])-1);
                nomes[count][sizeof(nomes[count])-1] = '\0';
                count++;
            }
            cur = cur->proximo;
        }
    }
    if (count == 0) printf("\nNenhum suspeito cadastrado.\n");
    else {
        printf("\nSuspeitos conhecidos (%d):\n\n", count);
        for (int i = 0; i < count; ++i) printf("  - %s\n", nomes[i]);
    }
    pause();
}

/* --------------------------
   main
   -------------------------- */
int main(void) {
    Sala *hall       = criarSala("Hall de Entrada", "A porta estava destrancada");
    Sala *salaEstar  = criarSala("Sala de Estar",    "Havia um copo quebrado no chão");
    Sala *cozinha    = criarSala("Cozinha",          "A faca sumiu do suporte");
    Sala *biblioteca = criarSala("Biblioteca",       "Um livro estava fora do lugar");
    Sala *jardim     = criarSala("Jardim",           "Pegadas molhadas perto da estufa");
    Sala *adega      = criarSala("Adega",            "Cheiro forte de perfume");
    Sala *escritorio = criarSala("Escritório",       "Bilhete rasgado encontrado");
    Sala *quarto     = criarSala("Quarto",           "Lençol com manchas de tinta");

    hall->esquerda = salaEstar;
    hall->direita  = cozinha;
    salaEstar->esquerda = biblioteca;
    salaEstar->direita  = jardim;
    cozinha->esquerda = escritorio;
    cozinha->direita = adega;
    biblioteca->esquerda = quarto;

    HashTable *ht = criarHashTable(31);
    inserirNaHash(ht, "A porta estava destrancada", "Carlos");
    inserirNaHash(ht, "Havia um copo quebrado no chão", "Mariana");
    inserirNaHash(ht, "A faca sumiu do suporte", "Carlos");
    inserirNaHash(ht, "Um livro estava fora do lugar", "Eduardo");
    inserirNaHash(ht, "Pegadas molhadas perto da estufa", "Mariana");
    inserirNaHash(ht, "Cheiro forte de perfume", "Isabela");
    inserirNaHash(ht, "Bilhete rasgado encontrado", "Eduardo");
    inserirNaHash(ht, "Lençol com manchas de tinta", "Isabela");

    PistaNode *pistasColetadas = NULL;
    explorarSalas(hall, &pistasColetadas, ht);

    printf("\n============================\n\n");
    printf("Pistas coletadas:\n");
    if (pistasColetadas == NULL) printf("Nenhuma pista foi coletada.\n");
    else {
        int contador = 0;
        exibirPistasInOrder(pistasColetadas, &contador);
        printf("\nTotal de pistas coletadas: [%d]\n", contador);
    }

    char acusado[64];
    printf("\nDigite o nome do suspeito a ser acusado: ");
    if (fgets(acusado, sizeof(acusado), stdin)) {
        trim_newline(acusado);
        verificarSuspeitoFinal(pistasColetadas, ht, acusado);
    }

    liberarSalas(hall);
    liberarPistas(pistasColetadas);
    liberarHashTable(ht);
    printf("\nFim do jogo. Obrigado por jogar!\n");
    return 0;
}
