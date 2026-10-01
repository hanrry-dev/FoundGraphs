#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_V 20
#define MAX_NOME 4

typedef struct Adj {
    int vizinho;
    int peso;
    struct Adj *prox;
} Adj;

typedef struct {
    int n;
    char nome[MAX_V][MAX_NOME];
    Adj *lista[MAX_V];
} Grafo;

int obter_indice(Grafo *g, const char *cod) {
    for (int i = 0; i < g->n; i++) {
        if (strcmp(g->nome[i], cod) == 0) {
            return i;
        }
    }

    strcpy(g->nome[g->n], cod);
    g->lista[g->n] = NULL;
    return g->n++;
}

void adicionar_aresta(Grafo *g, const char *a, const char *b, int p) {
    int u = obter_indice(g, a);
    int v = obter_indice(g, b);

    Adj *novo1 = malloc(sizeof(Adj));
    novo1->vizinho = v;
    novo1->peso = p;
    novo1->prox = g->lista[u];
    g->lista[u] = novo1;

    Adj *novo2 = malloc(sizeof(Adj));
    novo2->vizinho = u;
    novo2->peso = p;
    novo2->prox = g->lista[v];
    g->lista[v] = novo2;
}

int ler_grafo(Grafo *g, const char *arquivo) {
    FILE *f = fopen(arquivo, "r");
    if (!f) {
        return 0;
    }

    int qtd;
    if (fscanf(f, "%d", &qtd) != 1) {
        fclose(f);
        return 0;
    }

    for (int i = 0; i < qtd; i++) {
        char a[MAX_NOME], b[MAX_NOME];
        int p;
        if (fscanf(f, " %3[^;];%3[^;];%d", a, b, &p) != 3) {
            fclose(f);
            return 0;
        }
        adicionar_aresta(g, a, b, p);
    }

    fclose(f);
    return 1;
}

int bfs(Grafo *g, int origem, int dist[]) {
    int visitado[MAX_V] = {0};
    int fila[MAX_V];
    int ini = 0, fim = 0;
    int alcancados = 0;

    visitado[origem] = 1;
    dist[origem] = 0;
    fila[fim++] = origem;

    printf("BFS: ");
    while (ini < fim) {
        int u = fila[ini++];
        printf("%s%s", alcancados ? " -> " : "", g->nome[u]);
        alcancados++;

        for (Adj *a = g->lista[u]; a != NULL; a = a->prox) {
            int v = a->vizinho;
            if (!visitado[v]) {
                visitado[v] = 1;
                dist[v] = dist[u] + 1;
                fila[fim++] = v;
            }
        }
    }

    printf("\n");
    return alcancados;
}

void dfs_visita(Grafo *g, int u, int visitado[], int *cont) {
    visitado[u] = 1;
    printf("%s%s", (*cont)++ ? " -> " : "", g->nome[u]);

    for (Adj *a = g->lista[u]; a != NULL; a = a->prox) {
        int v = a->vizinho;
        if (!visitado[v]) {
            dfs_visita(g, v, visitado, cont);
        }
    }
}

void dfs(Grafo *g, int origem) {
    int visitado[MAX_V] = {0};
    int cont = 0;

    printf("DFS: ");
    dfs_visita(g, origem, visitado, &cont);
    printf("\n");
}

int ciclo_visita(Grafo *g, int u, int visitado[], int pai[], int ciclo[], int *tam) {
    visitado[u] = 1;

    for (Adj *a = g->lista[u]; a != NULL; a = a->prox) {
        int v = a->vizinho;

        if (!visitado[v]) {
            pai[v] = u;
            if (ciclo_visita(g, v, visitado, pai, ciclo, tam)) {
                return 1;
            }
        } else if (v != pai[u]) {
            int tmp[MAX_V];
            int k = 0;
            int x = u;

            tmp[k++] = u;
            while (x != v) {
                x = pai[x];
                tmp[k++] = x;
            }

            *tam = 0;
            for (int j = k - 1; j >= 0; j--) {
                ciclo[(*tam)++] = tmp[j];
            }
            ciclo[(*tam)++] = v;
            return 1;
        }
    }

    return 0;
}

void encontrar_ciclo(Grafo *g, int origem) {
    int visitado[MAX_V] = {0};
    int pai[MAX_V];
    int ciclo[MAX_V + 1];
    int tam = 0;

    pai[origem] = -1;
    printf("Ciclo (DFS): ");

    if (ciclo_visita(g, origem, visitado, pai, ciclo, &tam)) {
        for (int i = 0; i < tam; i++) {
            printf("%s%s", i ? " -> " : "", g->nome[ciclo[i]]);
        }
        printf("\n");
    } else {
        printf("nenhum\n");
    }
}

int main(void) {
    Grafo g;
    g.n = 0;

    if (!ler_grafo(&g, "grafo.txt")) {
        printf("Erro ao abrir grafo.txt\n");
        return 1;
    }

    int origem = obter_indice(&g, "AJU");
    int destino = obter_indice(&g, "THE");
    int dist[MAX_V];

    printf("Lista de adjacência:\n");
    for (int u = 0; u < g.n; u++) {
        printf("  %s:", g.nome[u]);
        for (Adj *a = g.lista[u]; a != NULL; a = a->prox) {
            printf(" %s", g.nome[a->vizinho]);
        }
        printf("\n");
    }
    printf("\n");

    int alcancados = bfs(&g, origem, dist);
    dfs(&g, origem);

    printf("\nO grafo é conexo? %s\n", alcancados == g.n ? "Sim" : "Não");
    printf("Conexões de AJU até THE: %d\n", dist[destino]);
    encontrar_ciclo(&g, origem);

    return 0;
}