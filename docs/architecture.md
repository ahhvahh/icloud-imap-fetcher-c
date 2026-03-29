# Arquitetura

## Visão geral

O `icloud-imap-fetcher-c` é um serviço orientado a execução periódica. O objetivo é conectar via IMAP, localizar mensagens específicas e persistir conteúdo relevante localmente com rastreabilidade por log.

## Fluxo principal

1. processo recebe `--config`;
2. o módulo de configuração carrega defaults e sobrescreve com o arquivo `.conf`;
3. o logger é inicializado e executa expurgo de logs antigos;
4. os diretórios operacionais são preparados;
5. a camada IMAP é inicializada com `libcurl`;
6. o serviço executa a rotina de busca/processamento;
7. o processo encerra de forma limpa.

## Módulos

### `app_config`

Responsável por carregar defaults, ler o arquivo de configuração, interpretar booleanos e inteiros e centralizar a estrutura `AppConfig`.

### `logger`

Responsável por garantir existência do diretório de log, abrir o arquivo ativo, registrar mensagens com timestamp e remover arquivos `.log` antigos.

### `attachment_saver`

Responsável por criar diretórios necessários e servir como ponto de evolução para persistência de anexos.

### `imap_client`

Responsável por inicializar `libcurl`, configurar credenciais e URL IMAP e concentrar a futura lógica de busca, leitura e marcação das mensagens.

### `main`

Responsável por parsing de argumentos, orquestração dos módulos e controle do fluxo principal.
