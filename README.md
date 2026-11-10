# 🛒 Cadastro de Loja em C

Sistema de cadastro de produtos em linha de comando, feito em C, com persistência em arquivo de texto.

## 📌 Funcionalidades
- Criar, listar, inserir, alterar e excluir produtos
- ID automático incremental
- Verificação de duplicidade por nome
- Busca por nome e categoria (case-insensitive)
- Ordenação por nome (A–Z / Z–A) e preço (crescente / decrescente) com Bubble Sort
- Exportação para CSV

## 🗂️ Estrutura dos dados
Cada produto é salvo em `produtos.txt` no formato:

`id;produto;categoria;preco;quantidade;fornecedor`

## 🧠 Conceitos aplicados
- `struct` e ponteiros
- Manipulação de arquivos (`fopen`, `fprintf`, `fscanf`, arquivo temporário)
- Menu com `switch`
- Algoritmo de ordenação (Bubble Sort)

## ▶️ Como executar
gcc main.c -o cadastro_loja
./cadastro_loja

## 👤 Autor
Arthur Ferreira Magalhães
[LinkedIn](https://linkedin.com/in/arthur-ferreira-magalhães-515743286) | [GitHub](https://github.com/arthur-fm123)
