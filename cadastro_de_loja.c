/*
 * ATIVIDADE AVALIATIVA 04 - Cadastro de loja (ordenacao + arquivos + struct + ponteiros)
 *
 * Arquivo de dados: produtos.txt
 * Formato de cada linha: id;produto;categoria;preco;quantidade;fornecedor
 * Exemplo:               1;Camiseta Basica;Vestuario;49.90;30;Moda Brasil
 *
 * Compilar:  gcc -Wall -Wextra cadastro_loja.c -o cadastro_loja
 */

#include <stdio.h>   // printf, scanf, FILE, fopen, fclose, fprintf, fscanf, remove, rename
#include <stdlib.h>  // exit
#include <string.h>  // strcmp, strstr, strcpy
#include <ctype.h>   // tolower

#define ARQUIVO     "produtos.txt"   // arquivo principal de dados
#define TEMPORARIO  "temp.txt"       // arquivo temporario (alterar/excluir)
#define ARQUIVO_CSV "produtos.csv"   // arquivo de exportacao
#define MAX 500                      // maximo de produtos carregados na memoria

// Estrutura que representa uma linha da tabela de produtos
typedef struct {
    int id;                 // identificador (gerado automaticamente)
    char produto[50];       // nome do produto
    char categoria[50];     // categoria
    float preco;            // preco em reais
    int quantidade;         // quantidade em estoque
    char fornecedor[50];    // nome do fornecedor
} Produto;

/* ====================================================================
 *  1. FUNCOES DE ENTRADA (leitura segura do teclado)
 * ==================================================================== */

// Descarta o que sobrou no buffer do teclado ate o Enter
void limparBuffer() {
    int c;                                        // guarda cada caractere lido
    while ((c = getchar()) != '\n' && c != EOF);  // le e joga fora ate achar o Enter
}

// Le um numero inteiro; repete enquanto o usuario digitar algo invalido (ex.: letra)
int lerInt(const char *mensagem) {
    int n, r;                                     // n = valor lido, r = retorno do scanf
    printf("%s", mensagem);                       // mostra a pergunta
    while ((r = scanf("%d", &n)) != 1) {          // scanf devolve 1 quando leu um inteiro
        if (r == EOF) exit(0);                    // acabou a entrada: encerra o programa
        limparBuffer();                           // tira o texto invalido do buffer
        printf("Valor invalido! %s", mensagem);   // pergunta de novo
    }
    return n;                                     // devolve o inteiro lido
}

// Le um numero decimal (use PONTO: 49.90); repete se for invalido
float lerFloat(const char *mensagem) {
    float n;                                      // valor lido
    int r;                                        // retorno do scanf
    printf("%s", mensagem);                       // mostra a pergunta
    while ((r = scanf("%f", &n)) != 1) {          // 1 = leu um float
        if (r == EOF) exit(0);                    // fim da entrada
        limparBuffer();                           // limpa a entrada invalida
        printf("Valor invalido! %s", mensagem);   // pergunta de novo
    }
    return n;                                     // devolve o float lido
}

// Le uma linha de texto (aceita espacos) e troca ';' por ',' para nao quebrar o arquivo
void lerTexto(const char *mensagem, char *destino) {
    int i;                                        // indice para percorrer o texto
    printf("%s", mensagem);                       // mostra a pergunta
    if (scanf(" %49[^\n]", destino) != 1) exit(0);// le ate o Enter (max 49); espaco inicial pula \n sobrando
    for (i = 0; destino[i] != '\0'; i++) {        // percorre cada caractere do texto
        if (destino[i] == ';') destino[i] = ',';  // ';' e o separador do arquivo, entao nao pode ficar no texto
    }
}

/* ====================================================================
 *  2. FUNCOES DE TEXTO (comparacoes case-insensitive, com ponteiros)
 * ==================================================================== */

// Copia 'origem' para 'destino' convertendo tudo para minusculo
void minusculo(const char *origem, char *destino) {
    while (*origem != '\0') {                     // enquanto nao chegou no fim do texto
        *destino = tolower((unsigned char)*origem); // converte o caractere apontado e grava no destino
        origem++;                                 // avanca o ponteiro de origem
        destino++;                                // avanca o ponteiro de destino
    }
    *destino = '\0';                              // fecha o texto de destino
}

// Compara dois textos ignorando maiusculas/minusculas (mesmo retorno do strcmp)
int comparar(const char *a, const char *b) {
    char x[50], y[50];                            // copias em minusculo
    minusculo(a, x);                              // converte o primeiro
    minusculo(b, y);                              // converte o segundo
    return strcmp(x, y);                          // 0 = iguais, >0 = a vem depois, <0 = a vem antes
}

// Devolve 1 se 'texto' contem 'trecho' (ignorando maiusculas), senao 0
int contem(const char *texto, const char *trecho) {
    char x[50], y[50];                            // copias em minusculo
    minusculo(texto, x);                          // converte o texto
    minusculo(trecho, y);                         // converte o trecho procurado
    return strstr(x, y) != NULL;                  // strstr devolve NULL se nao encontrar
}

/* ====================================================================
 *  3. FUNCOES DE ARQUIVO
 * ==================================================================== */

// Le UM produto do arquivo; devolve 1 se leu os 6 campos, 0 se acabou o arquivo
int lerProduto(FILE *f, Produto *p) {
    return fscanf(f, " %d;%49[^;];%49[^;];%f;%d;%49[^\n]",
                  &p->id, p->produto, p->categoria,   // p->campo e o mesmo que (*p).campo
                  &p->preco, &p->quantidade, p->fornecedor) == 6;
}

// Grava UM produto no arquivo, em uma linha
void gravarProduto(FILE *f, const Produto *p) {
    fprintf(f, "%d;%s;%s;%.2f;%d;%s\n",
            p->id, p->produto, p->categoria, p->preco, p->quantidade, p->fornecedor);
}

// Carrega todos os produtos do arquivo para o vetor; devolve quantos leu
int carregar(Produto lista[]) {
    FILE *f = fopen(ARQUIVO, "r");                // abre para leitura
    int n = 0;                                    // contador de produtos lidos
    if (f == NULL) return 0;                      // arquivo nao existe: lista vazia
    while (n < MAX && lerProduto(f, &lista[n])) { // le enquanto couber e houver linhas
        n++;                                      // conta o produto lido
    }
    fclose(f);                                    // fecha o arquivo
    return n;                                     // quantidade carregada
}

// Grava o vetor inteiro em arquivo TEMPORARIO e depois troca pelo original
// (tecnica que evita corromper os dados se algo falhar no meio da gravacao)
void salvarTudo(const Produto lista[], int n) {
    FILE *f = fopen(TEMPORARIO, "w");             // cria o arquivo temporario
    int i;                                        // indice do laco
    if (f == NULL) {                              // se nao conseguiu criar
        printf("Erro ao criar o arquivo temporario!\n");
        return;                                   // sai sem mexer no original
    }
    for (i = 0; i < n; i++) {                     // percorre todos os produtos
        gravarProduto(f, &lista[i]);              // grava cada um no temporario
    }
    fclose(f);                                    // fecha o temporario
    remove(ARQUIVO);                              // apaga o original
    rename(TEMPORARIO, ARQUIVO);                  // o temporario passa a ser o original
}

// Devolve o proximo ID: maior ID existente + 1 (ID automatico)
int proximoId() {
    FILE *f = fopen(ARQUIVO, "r");                // abre para leitura
    Produto p;                                    // guarda cada linha lida
    int maior = 0;                                // maior ID visto ate agora
    if (f == NULL) return 1;                      // sem arquivo: o primeiro ID e 1
    while (lerProduto(f, &p)) {                   // le linha por linha
        if (p.id > maior) {                       // se esse ID e maior que o maior visto
            maior = p.id;                         // ele passa a ser o maior
        }
    }
    fclose(f);                                    // fecha o arquivo
    return maior + 1;                             // proximo ID livre
}

// Verifica se um ID ja existe no arquivo (protecao extra contra ID duplicado)
int existeId(int id) {
    FILE *f = fopen(ARQUIVO, "r");                // abre para leitura
    Produto p;                                    // guarda cada linha lida
    if (f == NULL) return 0;                      // sem arquivo, nada existe
    while (lerProduto(f, &p)) {                   // percorre o arquivo
        if (p.id == id) {                         // achou o mesmo ID
            fclose(f);                            // fecha antes de sair
            return 1;                             // ja existe
        }
    }
    fclose(f);                                    // fecha o arquivo
    return 0;                                     // nao existe
}

// Verifica se ja existe produto com esse nome (ignora maiusculas).
// 'idIgnorar' permite ignorar o proprio produto ao alterar (use 0 ao inserir).
int existeNome(const char *nome, int idIgnorar) {
    FILE *f = fopen(ARQUIVO, "r");                // abre para leitura
    Produto p;                                    // guarda cada linha lida
    if (f == NULL) return 0;                      // sem arquivo, sem duplicata
    while (lerProduto(f, &p)) {                   // percorre o arquivo
        if (p.id != idIgnorar && comparar(p.produto, nome) == 0) { // mesmo nome em outro produto
            fclose(f);                            // fecha antes de sair
            return 1;                             // duplicado
        }
    }
    fclose(f);                                    // fecha o arquivo
    return 0;                                     // nome livre
}

// Procura um produto pelo ID e devolve um PONTEIRO para ele (ou NULL se nao achar)
Produto *buscarPorId(Produto lista[], int n, int id) {
    int i;                                        // indice do laco
    for (i = 0; i < n; i++) {                     // percorre o vetor
        if (lista[i].id == id) {                  // achou o ID
            return &lista[i];                     // devolve o endereco do elemento
        }
    }
    return NULL;                                  // nao encontrou
}

/* ====================================================================
 *  4. EXIBICAO EM TABELA
 * ==================================================================== */

// Imprime o cabecalho da tabela
void imprimirCabecalho() {
    printf("\n+------+----------------------+------------------+------------+------------+----------------------+\n");
    printf("| %-4s | %-20s | %-16s | %10s | %10s | %-20s |\n",
           "ID", "Produto", "Categoria", "Preco (R$)", "Quantidade", "Fornecedor");
    printf("+------+----------------------+------------------+------------+------------+----------------------+\n");
}

// Imprime UM produto (recebido por ponteiro) como linha da tabela
void imprimirLinha(const Produto *p) {
    printf("| %-4d | %-20s | %-16s | %10.2f | %10d | %-20s |\n",
           p->id, p->produto, p->categoria, p->preco, p->quantidade, p->fornecedor);
}

// Imprime a linha final da tabela
void imprimirRodape() {
    printf("+------+----------------------+------------------+------------+------------+----------------------+\n");
}

// Imprime a tabela completa de um vetor (nao altera o arquivo)
void mostrarTabela(const Produto lista[], int n) {
    int i;                                        // indice do laco
    imprimirCabecalho();                          // topo da tabela
    for (i = 0; i < n; i++) {                     // uma linha por produto
        imprimirLinha(&lista[i]);
    }
    imprimirRodape();                             // base da tabela
}

/* ====================================================================
 *  5. OPERACOES DO MENU
 * ==================================================================== */

// 1 - CRIAR: cria o arquivo de dados vazio
void criar() {
    FILE *f = fopen(ARQUIVO, "r");                // tenta abrir para ver se ja existe
    char resp;                                    // resposta do usuario (s/n)
    if (f != NULL) {                              // se abriu, o arquivo ja existe
        fclose(f);                                // fecha o teste
        printf("O arquivo ja existe. Apagar todos os produtos? (s/n): ");
        scanf(" %c", &resp);                      // le a resposta
        if (tolower((unsigned char)resp) != 's') {// qualquer coisa diferente de 's' cancela
            printf("Operacao cancelada.\n");
            return;
        }
    }
    f = fopen(ARQUIVO, "w");                      // "w" cria o arquivo vazio (ou zera o existente)
    if (f == NULL) {                              // falhou ao criar
        printf("Erro ao criar o arquivo!\n");
        return;
    }
    fclose(f);                                    // fecha o arquivo
    printf("Arquivo %s criado!\n", ARQUIVO);
}

// 2 - LISTAR: mostra todos os produtos em tabela
void listar() {
    Produto lista[MAX];                           // vetor de estruturas na memoria
    int n = carregar(lista);                      // le o arquivo para o vetor
    if (n == 0) {                                 // nada cadastrado
        printf("Nenhum produto cadastrado.\n");
        return;
    }
    mostrarTabela(lista, n);                      // exibe a tabela
}

// 3 - INSERIR: cadastra um produto novo (ID automatico, sem nome duplicado)
void inserir() {
    Produto p;                                    // produto novo
    FILE *f;                                      // arquivo de dados

    lerTexto("Produto: ", p.produto);             // le o nome
    if (existeNome(p.produto, 0)) {               // ja existe produto com esse nome?
        printf("Ja existe um produto com esse nome! Cadastro cancelado.\n");
        return;                                   // sai sem gravar
    }
    lerTexto("Categoria: ", p.categoria);         // le a categoria
    do {
        p.preco = lerFloat("Preco (use ponto, ex.: 49.90): "); // le o preco
    } while (p.preco < 0);                        // nao aceita preco negativo
    do {
        p.quantidade = lerInt("Quantidade: ");    // le a quantidade
    } while (p.quantidade < 0);                   // nao aceita quantidade negativa
    lerTexto("Fornecedor: ", p.fornecedor);       // le o fornecedor

    p.id = proximoId();                           // ID automatico (maior + 1)
    while (existeId(p.id)) {                      // verificacao extra de ID duplicado
        p.id++;                                   // se existir, tenta o proximo
    }

    f = fopen(ARQUIVO, "a");                      // "a" acrescenta no fim do arquivo
    if (f == NULL) {                              // falhou ao abrir
        printf("Erro ao abrir o arquivo!\n");
        return;
    }
    gravarProduto(f, &p);                         // grava a linha do produto
    fclose(f);                                    // fecha o arquivo
    printf("Produto inserido com ID %d!\n", p.id);
}

// 4 - ALTERAR: muda os dados de um produto (reescreve via arquivo temporario)
void alterar() {
    Produto lista[MAX];                           // vetor com todos os produtos
    int n = carregar(lista);                      // carrega o arquivo
    int id;                                       // ID que o usuario quer alterar
    Produto *p;                                   // ponteiro para o produto encontrado
    char nome[50];                                // novo nome digitado

    if (n == 0) {                                 // lista vazia
        printf("Nenhum produto cadastrado.\n");
        return;
    }
    id = lerInt("ID do produto a alterar: ");     // pergunta qual
    p = buscarPorId(lista, n, id);                // procura o produto
    if (p == NULL) {                              // nao achou
        printf("Produto nao encontrado!\n");
        return;
    }

    printf("Produto atual:");
    mostrarTabela(p, 1);                          // mostra o produto (ponteiro serve como vetor de 1)

    lerTexto("Novo nome: ", nome);                // le o novo nome
    if (existeNome(nome, p->id)) {                // outro produto ja usa esse nome?
        printf("Ja existe outro produto com esse nome! Alteracao cancelada.\n");
        return;
    }
    strcpy(p->produto, nome);                     // copia o novo nome para o produto
    lerTexto("Nova categoria: ", p->categoria);   // le a nova categoria direto no produto
    do {
        p->preco = lerFloat("Novo preco: ");      // le o novo preco
    } while (p->preco < 0);
    do {
        p->quantidade = lerInt("Nova quantidade: ");
    } while (p->quantidade < 0);
    lerTexto("Novo fornecedor: ", p->fornecedor); // le o novo fornecedor

    salvarTudo(lista, n);                         // grava tudo em temp.txt e substitui o original
    printf("Produto alterado!\n");
}

// 5 - EXCLUIR: remove um produto (reescreve via arquivo temporario)
void excluir() {
    Produto lista[MAX];                           // vetor com todos os produtos
    int n = carregar(lista);                      // carrega o arquivo
    int id, i, j;                                 // id procurado e indices
    int posicao = -1;                             // posicao do produto no vetor (-1 = nao achou)
    char resp;                                    // confirmacao s/n

    if (n == 0) {                                 // lista vazia
        printf("Nenhum produto cadastrado.\n");
        return;
    }
    id = lerInt("ID do produto a excluir: ");     // pergunta qual
    for (i = 0; i < n; i++) {                     // procura a posicao do ID
        if (lista[i].id == id) {
            posicao = i;                          // guarda onde esta
        }
    }
    if (posicao == -1) {                          // nao achou
        printf("Produto nao encontrado!\n");
        return;
    }

    mostrarTabela(&lista[posicao], 1);            // mostra o que sera excluido
    printf("Confirmar exclusao? (s/n): ");
    scanf(" %c", &resp);                          // le a confirmacao
    if (tolower((unsigned char)resp) != 's') {    // se nao confirmou
        printf("Exclusao cancelada.\n");
        return;
    }

    for (j = posicao; j < n - 1; j++) {           // puxa os proximos produtos uma posicao para tras
        lista[j] = lista[j + 1];                  // sobrescreve o excluido
    }
    n--;                                          // agora ha um produto a menos
    salvarTudo(lista, n);                         // grava tudo em temp.txt e substitui o original
    printf("Produto excluido!\n");
}

// 6 - BUSCAR POR NOME (parte do nome, ignorando maiusculas)
void buscarPorNome() {
    Produto lista[MAX];                           // vetor com todos os produtos
    int n = carregar(lista);                      // carrega o arquivo
    char termo[50];                               // texto procurado
    int i, achou = 0;                             // indice e contador de resultados

    lerTexto("Nome (ou parte): ", termo);         // le o que buscar
    for (i = 0; i < n; i++) {                     // percorre os produtos
        if (contem(lista[i].produto, termo)) {    // o nome contem o termo?
            if (achou == 0) imprimirCabecalho();  // imprime o topo so no primeiro resultado
            imprimirLinha(&lista[i]);             // imprime o produto
            achou++;                              // conta o resultado
        }
    }
    if (achou == 0) {
        printf("Nenhum produto encontrado.\n");
    } else {
        imprimirRodape();                         // fecha a tabela
    }
}

// 7 - BUSCAR POR CATEGORIA (parte da categoria, ignorando maiusculas)
void buscarPorCategoria() {
    Produto lista[MAX];                           // vetor com todos os produtos
    int n = carregar(lista);                      // carrega o arquivo
    char termo[50];                               // texto procurado
    int i, achou = 0;                             // indice e contador de resultados

    lerTexto("Categoria (ou parte): ", termo);    // le o que buscar
    for (i = 0; i < n; i++) {                     // percorre os produtos
        if (contem(lista[i].categoria, termo)) {  // a categoria contem o termo?
            if (achou == 0) imprimirCabecalho();  // topo so no primeiro resultado
            imprimirLinha(&lista[i]);             // imprime o produto
            achou++;                              // conta o resultado
        }
    }
    if (achou == 0) {
        printf("Nenhum produto encontrado.\n");
    } else {
        imprimirRodape();                         // fecha a tabela
    }
}

/* ====================================================================
 *  6. ORDENACAO (Bubble Sort)
 * ==================================================================== */

// Troca dois produtos de lugar usando ponteiros
void trocar(Produto *a, Produto *b) {
    Produto aux = *a;                             // guarda o conteudo de a
    *a = *b;                                      // a recebe o conteudo de b
    *b = aux;                                     // b recebe o conteudo antigo de a
}

// Bubble Sort por nome. crescente = 1 (A-Z) ou 0 (Z-A)
void ordenarPorNome(Produto *v, int n, int crescente) {
    int i, j, cmp;                                // indices e resultado da comparacao
    for (i = 0; i < n - 1; i++) {                 // passadas
        for (j = 0; j < n - i - 1; j++) {         // compara pares adjacentes
            cmp = comparar(v[j].produto, v[j + 1].produto); // compara os nomes sem diferenciar maiuscula
            if ((crescente && cmp > 0) || (!crescente && cmp < 0)) { // esta fora de ordem?
                trocar(&v[j], &v[j + 1]);         // troca de lugar
            }
        }
    }
}

// Bubble Sort por preco. crescente = 1 (menor->maior) ou 0 (maior->menor)
void ordenarPorPreco(Produto *v, int n, int crescente) {
    int i, j;                                     // indices
    for (i = 0; i < n - 1; i++) {                 // passadas
        for (j = 0; j < n - i - 1; j++) {         // compara pares adjacentes
            if ((crescente && v[j].preco > v[j + 1].preco) ||   // crescente: atual maior que o proximo
                (!crescente && v[j].preco < v[j + 1].preco)) {  // decrescente: atual menor que o proximo
                trocar(&v[j], &v[j + 1]);         // troca de lugar
            }
        }
    }
}

// 8 - ORDENAR: submenu; mostra o resultado em tabela SEM alterar o arquivo
void ordenar() {
    int op;                                       // opcao do submenu
    do {
        printf("\n--- Ordenar ---\n");
        printf("1 - Nome (A-Z)\n");
        printf("2 - Nome (Z-A)\n");
        printf("3 - Preco (menor -> maior)\n");
        printf("4 - Preco (maior -> menor)\n");
        printf("0 - Voltar\n");
        op = lerInt("Opcao: ");                   // le a opcao
        if (op >= 1 && op <= 4) {                 // so carrega se for uma ordenacao valida
            Produto lista[MAX];                   // vetor na memoria
            int n = carregar(lista);              // carrega o arquivo
            if (n == 0) {
                printf("Nenhum produto cadastrado.\n");
            } else {
                switch (op) {
                    case 1: ordenarPorNome(lista, n, 1);  break; // A-Z
                    case 2: ordenarPorNome(lista, n, 0);  break; // Z-A
                    case 3: ordenarPorPreco(lista, n, 1); break; // menor -> maior
                    case 4: ordenarPorPreco(lista, n, 0); break; // maior -> menor
                }
                mostrarTabela(lista, n);          // exibe o resultado (arquivo continua igual)
            }
        } else if (op != 0) {
            printf("Opcao invalida!\n");
        }
    } while (op != 0);                            // 0 volta ao menu principal
}

// 9 - EXPORTAR CSV (separado por virgula; textos entre aspas)
void exportarCsv() {
    Produto lista[MAX];                           // vetor com todos os produtos
    int n = carregar(lista);                      // carrega o arquivo
    FILE *f;                                      // arquivo CSV
    int i;                                        // indice do laco

    if (n == 0) {
        printf("Nenhum produto para exportar.\n");
        return;
    }
    f = fopen(ARQUIVO_CSV, "w");                  // cria o CSV
    if (f == NULL) {
        printf("Erro ao criar o CSV!\n");
        return;
    }
    fprintf(f, "ID,Produto,Categoria,Preco,Quantidade,Fornecedor\n"); // linha de cabecalho
    for (i = 0; i < n; i++) {                     // uma linha por produto
        fprintf(f, "%d,\"%s\",\"%s\",%.2f,%d,\"%s\"\n",   // aspas protegem textos com virgula
                lista[i].id, lista[i].produto, lista[i].categoria,
                lista[i].preco, lista[i].quantidade, lista[i].fornecedor);
    }
    fclose(f);                                    // fecha o CSV
    printf("Exportado para %s (%d produtos).\n", ARQUIVO_CSV, n);
}

/* ====================================================================
 *  7. PROGRAMA PRINCIPAL
 * ==================================================================== */

int main() {
    int op;                                       // opcao do menu principal
    do {
        printf("\n --- Escolha qual acao voce deseja realizar --- \n\n");
        printf(" 1 - Criar\n");
        printf(" 2 - Listar\n");
        printf(" 3 - Inserir\n");
        printf(" 4 - Alterar\n");
        printf(" 5 - Excluir\n");
        printf(" 6 - Buscar por nome\n");
        printf(" 7 - Buscar por categoria\n");
        printf(" 8 - Ordenar produtos\n");
        printf(" 9 - Exportar CSV\n");
        printf(" 0 - Sair\n\n");
        op = lerInt("Opcao: ");                   // le a opcao (repete se digitar letra)
        switch (op) {                             // chama a funcao da opcao escolhida
            case 1: criar();              break;
            case 2: listar();             break;
            case 3: inserir();            break;
            case 4: alterar();            break;
            case 5: excluir();            break;
            case 6: buscarPorNome();      break;
            case 7: buscarPorCategoria(); break;
            case 8: ordenar();            break;
            case 9: exportarCsv();        break;
            case 0: printf("Saindo...\n"); break;
            default: printf("Opcao invalida!\n");
        }
    } while (op != 0);                            // repete ate escolher 0

    return 0;                                     // fim do programa
} 