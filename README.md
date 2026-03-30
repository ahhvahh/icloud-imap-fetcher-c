# icloud-imap-fetcher-c

Serviço em **C11 + libcurl** para conectar ao iCloud Mail via **IMAP sobre TLS**, aplicar filtros básicos e executar rotina de coleta de mensagens com base em configuração externa.

> **Status atual:** base funcional para configuração, conexão IMAP inicial, preparação de diretórios, logging com retenção e execução via `systemd service + timer`.

## Objetivo

Automatizar um fluxo operacional de coleta de e-mails em Linux, sem interface gráfica, com execução periódica e auditável.

## Escopo atual

- Carregamento de configuração por arquivo `.conf`.
- Conexão IMAP inicial usando `libcurl`.
- Preparação de diretórios de download/processados.
- Mapeamento de múltiplas pastas IMAP com destinos independentes para anexos e conteúdo.
- Logging em arquivo por dia com retenção em dias.
- Rota HTTP de logs (`/logs`) para acompanhamento via terminal.
- Instalação local com script (`scripts/install.sh`).
- Unidades `systemd` de `service` e `timer`.

## Limitações conhecidas

- A rotina IMAP ainda está em nível inicial (sessão/conexão); o fluxo completo de parsing MIME e persistência de anexos ainda precisa ser evoluído.
- Filtros avançados por remetente/assunto estão previstos em configuração, mas ainda dependem de implementação completa no cliente IMAP.

## Requisitos

- Linux
- Compilador C com suporte a C11 (ex.: `gcc` ou `clang`)
- `make`
- `libcurl` (headers + biblioteca)
- `systemd` (para modo serviço/timer)

Exemplo de dependências (Debian/Ubuntu):

```bash
sudo apt-get update
sudo apt-get install -y build-essential libcurl4-openssl-dev
```

## Build

```bash
make clean
make
```

Binário gerado:

- `bin/icloud-imap-fetcher-c`

## Configuração

Use `config/icloud-imap-fetcher.example.conf` como base.

```ini
[icloud]
credential_username_name = mail.icloud.user
credential_password_name = mail.icloud.pswrd
imap_url = imaps://imap.mail.me.com
mailbox = INBOX/Processar
search_filter = UNSEEN
mark_as_read = false

[message_filter]
from_contains = extrato@banco.com.br
subject_contains = Extrato Consolidado
attachment_name_prefix = extrato_
max_messages_per_run = 10
dry_run = false

[storage]
download_dir = /var/lib/icloud-imap-fetcher-c/downloads
processed_dir = /var/lib/icloud-imap-fetcher-c/processed
mapping = INBOX/Processar|UNSEEN|/var/lib/icloud-imap-fetcher-c/downloads/processar|/var/lib/icloud-imap-fetcher-c/contents/processar
mapping = INBOX/Notas|UNSEEN|/var/lib/icloud-imap-fetcher-c/downloads/notas|/var/lib/icloud-imap-fetcher-c/contents/notas

[logging]
log_dir = /var/log/icloud-imap-fetcher-c
retention_days = 7
level = info
log_http_port = 8080

[schedule]
interval_seconds = 300
```

### Segurança da configuração e credenciais

- Use **app-specific password** da Apple (não use a senha principal da conta).
- Não armazene segredos no `.conf`.
- Cadastre segredos no credstore com script administrativo:

```bash
sudo ./scripts/manage-credentials.sh --set mail.icloud.user
sudo ./scripts/manage-credentials.sh --set mail.icloud.pswrd
```

- Proteja o arquivo de configuração com permissão restrita:

```bash
chmod 600 /etc/icloud-imap-fetcher-c/config.conf
```

## Execução local

```bash
./bin/icloud-imap-fetcher-c --config ./config/icloud-imap-fetcher.example.conf
```

Ou com script auxiliar:

```bash
./scripts/run-local.sh
```

Servidor HTTP de logs (rota `/logs`):

```bash
./bin/icloud-imap-fetcher-c --config ./config/icloud-imap-fetcher.example.conf --serve-logs
```

Exemplo de acompanhamento no terminal:

```bash
watch -n 2 'curl -s http://127.0.0.1:8080/logs | tail -n 30'
```

## Instalação

Script de instalação:

```bash
sudo ./scripts/install.sh
```

Parâmetros suportados:

- `--prefix <dir>`
- `--sysconfdir <dir>`
- `--localstatedir <dir>`
- `--unitdir <dir>`
- `--config-source <file>`

Exemplo:

```bash
sudo ./scripts/install.sh --prefix /usr/local --sysconfdir /etc --localstatedir /var
```

### Instalação direta do GitHub (Debian/Ubuntu)

Use o instalador automatizado para baixar o repositório em diretório temporário, compilar e instalar:

```bash
sudo ./scripts/install-from-github-debian.sh --repo-url https://github.com/SEU_USUARIO/icloud-imap-fetcher-c.git --branch main
```

Se quiser, passe opções adicionais para o `scripts/install.sh` após `--`:

```bash
sudo ./scripts/install-from-github-debian.sh -- --prefix /usr/local --sysconfdir /etc --localstatedir /var
```

## systemd

Após instalar:

```bash
sudo systemctl daemon-reload
sudo systemctl enable --now icloud-imap-fetcher-c.timer
sudo systemctl list-timers | grep icloud-imap-fetcher-c
```

Execução manual do serviço:

```bash
sudo systemctl start icloud-imap-fetcher-c.service
sudo systemctl status icloud-imap-fetcher-c.service
```

O unit file usa `LoadCredential=` para carregar:

- `mail.icloud.user`
- `mail.icloud.pswrd`

## Logging

- Logs operacionais emitidos em `stdout`/`stderr` (coletáveis via `journalctl` quando executando com `systemd`).
- Persistência opcional em arquivo no diretório `log_dir`.
- Um arquivo por dia no formato `YYYY-MM-DD.log`.
- Limpeza de arquivos `.log` mais antigos que `retention_days`.

Exemplos com `journalctl`:

```bash
sudo journalctl -u icloud-imap-fetcher-c.service -n 100
sudo journalctl -u icloud-imap-fetcher-c.service -f
```

## Estrutura do projeto

```text
.
├── config/
├── docs/
├── include/
├── packaging/systemd/
├── scripts/
├── src/
├── Makefile
└── README.md
```

## Roadmap sugerido

1. Finalizar busca IMAP por mailbox/filtros.
2. Implementar parsing MIME e download real de anexos.
3. Implementar marcação de mensagens processadas (ex.: `SEEN` ou UID tracking).
4. Ampliar testes e validar fluxo ponta a ponta com conta iCloud de homologação.

## Documentação complementar

- `docs/architecture.md`
- `docs/configuration.md`
- `docs/deployment.md`
- `docs/technical-specification.md`
- `SECURITY.md`
