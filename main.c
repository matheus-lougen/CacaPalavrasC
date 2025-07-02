#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define TAM_MAX_PALAVRA 10
#define ARQUIVO_PALAVRAS "palavras.bin"

// Struct para palavra
typedef struct {
    char texto[TAM_MAX_PALAVRA];
} Palavra;

// Struct para coordenada
typedef struct {
    int x; // linhas
    int y; // colunas
} Coordenada;

// Struct para a matriz do jogo
typedef struct {
    char **dados; // Matriz de caracteres
    int linhas;
    int colunas;
} Matriz;

void cadastrar_palavra();
void atualizar_palavra();
void apagar_palavra();
void mostrar_palavras();
int contar_palavras_no_arquivo();
Palavra* carregar_palavras(int* total);
Matriz* criar_matriz(int linhas, int colunas);
void liberar_matriz(Matriz* matriz);
void preencher_matriz_com_letras_aleatorias(Matriz* matriz);
void posicionar_palavras_na_matriz(Matriz* matriz, Palavra* palavras, int total_palavras);
void exibir_matriz(Matriz* matriz);
void jogar();
int verificar_sequencia(Matriz* matriz, Coordenada inicio, Coordenada fim, Palavra* palavras, int total_palavras, int* indice_palavra_encontrada, char* palavra_selecionada);
void substituir_palavra_na_matriz(Matriz* matriz, Coordenada inicio, Coordenada fim, int ordem_encontrada);
void menu_inicial();

int main() {
    srand(time(NULL)); // Inicializa gerador de números aleatórios
    menu_inicial();
    return 0;
}

void menu_inicial() {
    int opcao;
    do {
        printf("\nMENU INICIAL\n");
        printf("1. Novo Jogo\n");
        printf("2. Cadastrar Palavra\n");
        printf("3. Atualizar Palavra\n");
        printf("4. Apagar Palavra\n");
        printf("5. Mostrar Palavras\n");
        printf("6. Sair\n");
        printf("Escolha uma opcao: ");
        scanf("%d", &opcao);

        switch (opcao) {
            case 1:
                jogar();
                break;
            case 2:
                cadastrar_palavra();
                break;
            case 3:
                atualizar_palavra();
                break;
            case 4:
                apagar_palavra();
                break;
            case 5:
                mostrar_palavras();
                break;
            case 6:
                printf("Saindo...\n");
                break;
            default:
                printf("Opcao invalida!\n");
        }
    } while (opcao != 6);
}

void cadastrar_palavra() {
    char nova_palavra[TAM_MAX_PALAVRA];
    int total_palavras = 0;
    Palavra* palavras = carregar_palavras(&total_palavras);

    printf("\nDigite a nova palavra (mínimo 5 letras, máximo 9 letras): ");
    scanf("%s", nova_palavra);

    int tamanho = strlen(nova_palavra);

    // Validação de tamanho
    if (tamanho < 5 || tamanho > 9) {
        printf("Tamanho inválido! A palavra deve ter entre 5 e 9 letras.\n");
        free(palavras);
        return;
    }

    // Validação de duplicata
    for (int i = 0; i < total_palavras; i++) {
        if (strcasecmp(palavras[i].texto, nova_palavra) == 0) {
            printf("A palavra já existe no arquivo!\n");
            free(palavras);
            return;
        }
    }

    // Gravar no arquivo
    FILE* arquivo = fopen(ARQUIVO_PALAVRAS, "ab");
    if (arquivo == NULL) {
        printf("Erro ao abrir o arquivo!\n");
        free(palavras);
        return;
    }

    Palavra p;
    strcpy(p.texto, nova_palavra);
    fwrite(&p, sizeof(Palavra), 1, arquivo);
    fclose(arquivo);

    printf("Palavra '%s' cadastrada com sucesso!\n", nova_palavra);

    free(palavras);
}


void atualizar_palavra() {
    int total_palavras = 0;
    Palavra* palavras = carregar_palavras(&total_palavras);

    if (total_palavras == 0) {
        printf("\nNenhuma palavra cadastrada para atualizar.\n");
        return;
    }

    printf("\n===== Palavras Cadastradas =====\n");
    for (int i = 0; i < total_palavras; i++) {
        printf("%d. %s\n", i + 1, palavras[i].texto);
    }

    int escolha;
    printf("\nDigite o número da palavra que deseja atualizar: ");
    scanf("%d", &escolha);

    if (escolha < 1 || escolha > total_palavras) {
        printf("Opção inválida!\n");
        free(palavras);
        return;
    }

    char nova_palavra[TAM_MAX_PALAVRA];
    printf("Digite a nova palavra (mínimo 5 letras, máximo 9 letras): ");
    scanf("%9s", nova_palavra);

    int tamanho = strlen(nova_palavra);

    // Validar tamanho
    if (tamanho < 5 || tamanho > 9) {
        printf("Tamanho inválido! A palavra deve ter entre 5 e 9 letras.\n");
        free(palavras);
        return;
    }

    // Verificar se a nova palavra já existe (exceto na posição que o usuário está tentando atualizar)
    for (int i = 0; i < total_palavras; i++) {
        if (i != (escolha - 1) && strcasecmp(palavras[i].texto, nova_palavra) == 0) {
            printf("Essa palavra já existe no arquivo!\n");
            free(palavras);
            return;
        }
    }

    // Atualizar palavra na lista
    strcpy(palavras[escolha - 1].texto, nova_palavra);

    // Regravar todo o arquivo
    FILE* arquivo = fopen(ARQUIVO_PALAVRAS, "wb");
    if (arquivo == NULL) {
        printf("Erro ao abrir o arquivo para gravação!\n");
        free(palavras);
        return;
    }

    fwrite(palavras, sizeof(Palavra), total_palavras, arquivo);
    fclose(arquivo);

    printf("Palavra atualizada com sucesso!\n");

    free(palavras);
}


void apagar_palavra() {
    int total_palavras = 0;
    Palavra* palavras = carregar_palavras(&total_palavras);

    if (total_palavras == 0) {
        printf("\nNenhuma palavra cadastrada para apagar.\n");
        return;
    }

    printf("\n===== Palavras Cadastradas =====\n");
    for (int i = 0; i < total_palavras; i++) {
        printf("%d. %s\n", i + 1, palavras[i].texto);
    }

    int escolha;
    printf("\nDigite o número da palavra que deseja apagar: ");
    scanf("%d", &escolha);
    getchar();  // Limpa o '\n' do buffer

    if (escolha < 1 || escolha > total_palavras) {
        printf("Opção inválida!\n");
        free(palavras);
        return;
    }

    // Remover a palavra escolhida
    for (int i = escolha - 1; i < total_palavras - 1; i++) {
        palavras[i] = palavras[i + 1];
    }
    total_palavras--;

    // Regravar o arquivo binário
    FILE* arquivo = fopen(ARQUIVO_PALAVRAS, "wb");
    if (arquivo == NULL) {
        printf("Erro ao abrir o arquivo para gravação!\n");
        free(palavras);
        return;
    }

    if (total_palavras > 0) {
        fwrite(palavras, sizeof(Palavra), total_palavras, arquivo);
    }
    fclose(arquivo);

    printf("Palavra apagada com sucesso!\n");
    free(palavras);
}


void mostrar_palavras() {
    int total_palavras = 0;
    Palavra* palavras = carregar_palavras(&total_palavras);

    if (total_palavras == 0) {
        printf("\nNenhuma palavra cadastrada.\n");
        return;
    }

    printf("\n===== Palavras Cadastradas =====\n");
    for (int i = 0; i < total_palavras; i++) {
        printf("%d. %s\n", i + 1, palavras[i].texto);
    }

    free(palavras);
}


int contar_palavras_no_arquivo() {
    // Retornar quantidade de palavras gravadas no arquivo
    return 0; // Substitua depois
}

Palavra* carregar_palavras(int* total) {
    FILE* arquivo = fopen(ARQUIVO_PALAVRAS, "rb");
    if (arquivo == NULL) {
        *total = 0;
        return NULL;
    }

    fseek(arquivo, 0, SEEK_END);
    long tamanho_arquivo = ftell(arquivo);
    rewind(arquivo);

    int quantidade = tamanho_arquivo / sizeof(Palavra);
    Palavra* palavras = (Palavra*)malloc(quantidade * sizeof(Palavra));

    fread(palavras, sizeof(Palavra), quantidade, arquivo);
    fclose(arquivo);

    *total = quantidade;
    return palavras;
}


// ==== Funções da Matriz ==== //

Matriz* criar_matriz(int linhas, int colunas) {
    Matriz* matriz = (Matriz*)malloc(sizeof(Matriz));
    matriz->linhas = linhas;
    matriz->colunas = colunas;

    matriz->dados = (char**)malloc(linhas * sizeof(char*));
    for (int i = 0; i < linhas; i++) {
        matriz->dados[i] = (char*)malloc(colunas * sizeof(char));
    }
    for (int i = 0; i < linhas; i++) {
        for (int j = 0; j < colunas; j++) {
            matriz->dados[i][j] = '\0';
        }
    }

    return matriz;  
}

void liberar_matriz(Matriz* matriz) {
    for (int i = 0; i < matriz->linhas; i++) {
        free(matriz->dados[i]);
    }
    free(matriz->dados);
    free(matriz);
}


void preencher_matriz_com_letras_aleatorias(Matriz* matriz) {
    for (int i = 0; i < matriz->linhas; i++) {
        for (int j = 0; j < matriz->colunas; j++) {
            if (matriz->dados[i][j] == '\0') {
                matriz->dados[i][j] = 'A' + (rand() % 26);
            }
        }
    }
}

void posicionar_palavras_na_matriz(Matriz* matriz, Palavra* palavras, int total_palavras) {
    for (int p = 0; p < total_palavras; p++) {
        int palavra_colocada = 0;
        int tentativas = 0;
        int max_tentativas = 1000;  // Limite para evitar loop infinito

        while (!palavra_colocada && tentativas < max_tentativas) {
            tentativas++;

            int direcao = rand() % 3;  // 0 = horizontal, 1 = vertical, 2 = diagonal
            int x_inicial, y_inicial;
            int dx = 0, dy = 0;

            int tamanho = strlen(palavras[p].texto);

            // Definir direção (delta x, delta y) e posição inicial válida
            const char* direcao_str;
            if (direcao == 0) {  // Horizontal
                dx = 0;
                dy = 1;
                x_inicial = rand() % matriz->linhas;
                y_inicial = rand() % (matriz->colunas - tamanho + 1);
                direcao_str = "Horizontal";
            } else if (direcao == 1) {  // Vertical
                dx = 1;
                dy = 0;
                x_inicial = rand() % (matriz->linhas - tamanho + 1);
                y_inicial = rand() % matriz->colunas;
                direcao_str = "Vertical";
            } else {  // Diagonal
                dx = 1;
                dy = 1;
                x_inicial = rand() % (matriz->linhas - tamanho + 1);
                y_inicial = rand() % (matriz->colunas - tamanho + 1);
                direcao_str = "Diagonal";
            }

            // Verificar se a palavra cabe sem conflito (ou se o conflito é com a mesma letra)
            int pode_colocar = 1;
            int x = x_inicial;
            int y = y_inicial;
            for (int c = 0; c < tamanho; c++) {
                char atual = matriz->dados[x][y];
                if (atual != '\0' && atual != palavras[p].texto[c]) {
                    pode_colocar = 0;
                    break;
                }
                x += dx;
                y += dy;
            }

            // Se puder, grava a palavra na matriz e imprime a posição
            if (pode_colocar) {
                x = x_inicial;
                y = y_inicial;
                for (int c = 0; c < tamanho; c++) {
                    matriz->dados[x][y] = palavras[p].texto[c];
                    x += dx;
                    y += dy;
                }
                palavra_colocada = 1;
                printf("Palavra '%s' posicionada em (%d, %d) na direção %s.\n",
                       palavras[p].texto, x_inicial, y_inicial, direcao_str);
            }
        }

        if (!palavra_colocada) {
            printf("Não foi possível posicionar a palavra '%s' na matriz.\n", palavras[p].texto);
        }
    }
}



void exibir_matriz(Matriz* matriz) {
    printf("\n===== MATRIZ DO JOGO =====\n\n");

    // Exibir cabeçalho das colunas
    printf("    ");
    for (int j = 0; j < matriz->colunas; j++) {
        printf("%2d ", j);
    }
    printf("\n");

    // Exibir separador
    printf("    ");
    for (int j = 0; j < matriz->colunas; j++) {
        printf("---");
    }
    printf("\n");

    // Exibir conteúdo da matriz linha por linha
    for (int i = 0; i < matriz->linhas; i++) {
        printf("%2d | ", i);
        for (int j = 0; j < matriz->colunas; j++) {
            char c = matriz->dados[i][j];

            if ((c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z')) {
                printf(" %c ", c);  // Letras normais
            } else if (c >= '1' && c <= '9') {
                printf(" %c ", c);  // Números de acerto (1 a 7, no caso desse jogo)
            } else {
                printf(" . ");      // Espaços vazios (caso queira usar '.' como placeholder antes de preencher aleatoriamente)
            }
        }
        printf("\n");
    }
    printf("\n");
}


// ==== Funções do Jogo ==== //

void jogar() {
    int linhas, colunas;

    printf("\n===== NOVO JOGO =====\n");
    do {
        printf("Digite o número de linhas da matriz (7 a 9): ");
        scanf("%d", &linhas);
        getchar();
    } while (linhas < 7 || linhas > 9);

    do {
        printf("Digite o número de colunas da matriz (7 a 9): ");
        scanf("%d", &colunas);
        getchar();
    } while (colunas < 7 || colunas > 9);

    int total_arquivo = 0;
    Palavra* todas_palavras = carregar_palavras(&total_arquivo);

    if (total_arquivo < 10) {
        printf("\nNúmero insuficiente de palavras no arquivo. É preciso ter no mínimo 10.\n");
        free(todas_palavras);
        return;
    }

    // Sortear 7 palavras distintas
    Palavra palavras_jogo[7];
    int indices[total_arquivo];
    for (int i = 0; i < total_arquivo; i++) indices[i] = i;

    // Shuffle simples (Fisher-Yates)
    for (int i = total_arquivo - 1; i > 0; i--) {
        int j = rand() % (i + 1);
        int temp = indices[i];
        indices[i] = indices[j];
        indices[j] = temp;
    }

    for (int i = 0; i < 7; i++) {
        palavras_jogo[i] = todas_palavras[indices[i]];
    }
    free(todas_palavras);

    Matriz* matriz = criar_matriz(linhas, colunas);
    posicionar_palavras_na_matriz(matriz, palavras_jogo, 7);
    preencher_matriz_com_letras_aleatorias(matriz);

    int tentativas = 0;
    int palavras_restantes = 7;
    int ordem_encontradas = 1;
    int palavras_encontradas[7] = {0};  // Marca quais palavras já foram encontradas

    while (palavras_restantes > 0) {
        exibir_matriz(matriz);

        printf("\nPalavras restantes:\n");
        for (int i = 0; i < 7; i++) {
            if (!palavras_encontradas[i]) {
                printf("- %s\n", palavras_jogo[i].texto);
            }
        }
        printf("\nTentativas até agora: %d\n", tentativas);

        int linha_ini, col_ini, linha_fim, col_fim;
        printf("\nDigite a coordenada de início (linha coluna): ");
        scanf("%d %d", &linha_ini, &col_ini);
        getchar();

        printf("Digite a coordenada de fim (linha coluna): ");
        scanf("%d %d", &linha_fim, &col_fim);
        getchar();

        // Validação de coordenadas
        if (linha_ini < 0 || linha_ini >= linhas || col_ini < 0 || col_ini >= colunas ||
            linha_fim < 0 || linha_fim >= linhas || col_fim < 0 || col_fim >= colunas) {
            printf("\nCoordenadas inválidas! Tente novamente.\n");
            continue;
        }

        Coordenada inicio = {linha_ini, col_ini};
        Coordenada fim = {linha_fim, col_fim};
        int indice_palavra = -1;
        char palavra_selecionada[10];

        if (verificar_sequencia(matriz, inicio, fim, palavras_jogo, 7, &indice_palavra, palavra_selecionada) && !palavras_encontradas[indice_palavra]) {
            printf("\nParabéns! Você encontrou a palavra: %s\n", palavras_jogo[indice_palavra].texto);
            substituir_palavra_na_matriz(matriz, inicio, fim, ordem_encontradas);
            palavras_encontradas[indice_palavra] = 1;
            palavras_restantes--;
            ordem_encontradas++;
        } else {
            printf("\nSelecionado: %s\n", palavra_selecionada);
            printf("Não corresponde a nenhuma palavra restante.\n");
        }


        tentativas++;
    }

    exibir_matriz(matriz);
    printf("\n===== FIM DE JOGO =====\n");
    printf("Total de tentativas: %d\n", tentativas);

    // Menu pós-jogo
    int opcao;
    do {
        printf("\nO que deseja fazer agora?\n");
        printf("1. Novo Jogo\n");
        printf("2. Menu Inicial\n");
        printf("3. Sair\n");
        printf("Escolha uma opção: ");
        scanf("%d", &opcao);
        getchar();

        switch (opcao) {
            case 1:
                liberar_matriz(matriz);
                jogar();
                return;
            case 2:
                liberar_matriz(matriz);
                menu_inicial();
                return;
            case 3:
                printf("Encerrando o jogo...\n");
                liberar_matriz(matriz);
                exit(0);
            default:
                printf("Opção inválida!\n");
        }
    } while (opcao != 1 && opcao != 2 && opcao != 3);
}

int verificar_sequencia(Matriz* matriz, Coordenada inicio, Coordenada fim, Palavra* palavras, int total_palavras, int* indice_palavra_encontrada, char* palavra_selecionada) {
    int diff_x = fim.x - inicio.x;
    int diff_y = fim.y - inicio.y;

    int dx = 0, dy = 0;

    if (diff_x == 0 && diff_y != 0) {
        dx = 0;
        dy = (diff_y > 0) ? 1 : -1;
    } else if (diff_y == 0 && diff_x != 0) {
        dx = (diff_x > 0) ? 1 : -1;
        dy = 0;
    } else if (abs(diff_x) == abs(diff_y) && diff_x != 0) {
        dx = (diff_x > 0) ? 1 : -1;
        dy = (diff_y > 0) ? 1 : -1;
    } else {
        printf("\nMovimento inválido! Só pode ser horizontal, vertical ou diagonal.\n");
        strcpy(palavra_selecionada, "");  // Zera a string caso movimento inválido
        return 0;
    }

    int comprimento = (abs(diff_x) != 0) ? abs(diff_x) + 1 : abs(diff_y) + 1;

    int x = inicio.x;
    int y = inicio.y;

    for (int i = 0; i < comprimento; i++) {
        palavra_selecionada[i] = matriz->dados[x][y];
        x += dx;
        y += dy;
    }
    palavra_selecionada[comprimento] = '\0';

    for (int i = 0; i < total_palavras; i++) {
        if (strcasecmp(palavras[i].texto, palavra_selecionada) == 0) {
            *indice_palavra_encontrada = i;
            return 1;
        }
    }

    *indice_palavra_encontrada = -1;
    return 0;
}




void substituir_palavra_na_matriz(Matriz* matriz, Coordenada inicio, Coordenada fim, int ordem_encontrada) {
    int dx = 0, dy = 0;

    int diff_x = fim.x - inicio.x;
    int diff_y = fim.y - inicio.y;

    if (diff_x != 0) dx = (diff_x > 0) ? 1 : -1;
    if (diff_y != 0) dy = (diff_y > 0) ? 1 : -1;

    int comprimento = (diff_x != 0) ? abs(diff_x) + 1 : abs(diff_y) + 1;

    int x = inicio.x;
    int y = inicio.y;

    // Convertendo ordem_encontrada para caractere: '1' + (ordem_encontrada - 1)
    char c = '0' + ordem_encontrada;

    for (int i = 0; i < comprimento; i++) {
        matriz->dados[x][y] = c;
        x += dx;
        y += dy;
    }
}

