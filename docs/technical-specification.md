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
3. inicializar logging em `stdout`/`stderr` para coleta por `journalctl` em `systemd`;
4. manter log diário em diretório configurável e realizar expurgo de logs antigos;
5. preparar diretórios de dados;
6. inicializar `libcurl` com credenciais lidas de `CREDENTIALS_DIRECTORY` (systemd `LoadCredential=`) e URL IMAP;
7. encerrar com código diferente de zero em falha crítica.
