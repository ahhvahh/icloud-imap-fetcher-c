# Configuração

## Formato

O projeto usa um arquivo `.conf` de sintaxe simples:

- comentários com `#` ou `;`
- seções entre colchetes, como `[icloud]`
- pares `chave = valor`

O parser atual trata o nome da seção como organizacional. As chaves são lidas pelo nome.

## Arquivo de exemplo

O arquivo base está em:

```text
config/icloud-imap-fetcher.example.conf
```

## Parâmetros suportados

- `username`: conta iCloud Mail usada para autenticação.
- `app_password`: senha específica de app.
- `imap_url`: URL IMAP. Para iCloud, o padrão é `imaps://imap.mail.me.com`.
- `mailbox`: nome da pasta IMAP alvo.
- `search_filter`: filtro IMAP base. Exemplo comum: `UNSEEN`.
- `mark_as_read`: aceita `true`, `false`, `1`, `0`, `yes`, `no`.
- `from_contains`: trecho esperado no remetente.
- `subject_contains`: trecho esperado no assunto.
- `attachment_name_prefix`: prefixo aplicado ao nome final do arquivo salvo.
- `max_messages_per_run`: limita quantas mensagens serão processadas por execução.
- `dry_run`: quando `true`, o serviço simula a operação sem persistir artefatos.
- `download_dir`: pasta de saída para anexos baixados.
- `processed_dir`: pasta auxiliar para rastreamento.
- `log_dir`: diretório onde os arquivos `.log` serão gravados.
- `retention_days`: define a retenção dos logs por idade de arquivo.
- `level`: nível de log textual.
- `interval_seconds`: intervalo alvo entre execuções.
