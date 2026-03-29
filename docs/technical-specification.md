# Especificação técnica

## Linguagem e build

- linguagem: **C11**
- compilação: `make`
- dependência principal: `libcurl`
- alvo inicial: Linux

## Binário

Nome do executável:

```text
icloud-imap-fetcher-c
```

## Interface de linha de comando

### Argumentos suportados

- `--config <arquivo>`: informa o caminho do arquivo de configuração.
- `--help`: exibe ajuda de uso.

## Requisitos funcionais atuais

1. carregar configuração de arquivo;
2. aplicar defaults para campos não informados;
3. inicializar log em diretório configurável;
4. realizar expurgo de logs antigos;
5. preparar diretórios de dados;
6. inicializar `libcurl` com credenciais e URL IMAP;
7. encerrar com código diferente de zero em falha crítica.
