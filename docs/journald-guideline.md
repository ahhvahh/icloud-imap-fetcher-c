# Diretriz de Logging com `journald` para Serviços Linux

## 1) Objetivo

Padronizar o uso do **systemd journal** (`journald`) como destino primário de logs para serviços Linux, priorizando:

- observabilidade em tempo real;
- centralização da coleta;
- filtragem por unidade (`.service`);
- redução de complexidade operacional.

## 2) Recomendação

Para serviços gerenciados por `systemd`, adotar o fluxo padrão:

1. a aplicação escreve logs em `stdout` (eventos) e `stderr` (erros);
2. o `systemd` captura automaticamente as saídas;
3. a operação consulta logs por `journalctl`.

> **ASSUNÇÃO:** o ambiente alvo executa Linux com `systemd` ativo.

## 3) Quando aplicar

Aplicar esta diretriz quando:

- a aplicação roda como serviço `systemd`;
- há necessidade de acompanhamento em tempo real;
- a operação já usa `systemctl`/`journalctl`.

## 4) Vantagens operacionais

- integração nativa com o gerenciador de serviços;
- consulta por serviço (`-u`) e por boot (`-b`);
- menor esforço com rotação/manual de arquivos para o caso básico.

## 5) Modelo recomendado

### 5.1 Aplicação

- registrar mensagens de negócio em `stdout`;
- registrar falhas em `stderr`;
- manter **uma linha por evento**;
- evitar dados sensíveis (senha, token, chave).

Exemplo:

```text
INFO aplicação iniciada
WARN fila com atraso
ERROR falha ao enviar arquivo
```

### 5.2 Unidade `systemd`

```ini
[Unit]
Description=Minha Aplicacao

[Service]
WorkingDirectory=/opt/minhaapp
ExecStart=/opt/minhaapp/minhaapp
Restart=always
RestartSec=5
SyslogIdentifier=minhaapp

[Install]
WantedBy=multi-user.target
```

## 6) Comandos operacionais essenciais

### Acompanhar em tempo real

```bash
journalctl -f -u minhaapp.service
```

### Últimas 100 linhas

```bash
journalctl -u minhaapp.service -n 100
```

### Apenas boot atual

```bash
journalctl -b -u minhaapp.service
```

### A partir de horário específico

```bash
journalctl -u minhaapp.service --since "2026-03-29 10:00:00"
```

### Apenas erros

```bash
journalctl -u minhaapp.service -p err
```

## 7) Persistência e retenção

Configuração em `/etc/systemd/journald.conf`:

```ini
[Journal]
Storage=persistent
SystemMaxUse=1G
RuntimeMaxUse=200M
```

Aplicar:

```bash
systemctl restart systemd-journald
```

Verificar uso:

```bash
journalctl --disk-usage
```

Limpar por idade:

```bash
journalctl --vacuum-time=7days
```

Limpar por tamanho:

```bash
journalctl --vacuum-size=500M
```

## 8) Limites e exceções

- Se houver exigência regulatória de exportação para SIEM/arquivo imutável, complementar com pipeline dedicado.
- Se o host não usar `systemd`, esta diretriz não se aplica.

> **PONTO EM ABERTO:** definir política institucional de retenção por ambiente (dev/hml/prd).

## 9) Conclusão

Para serviços Linux gerenciados por `systemd`, `journald` deve ser o mecanismo padrão de observabilidade local de logs, com aplicação escrevendo em `stdout`/`stderr` e operação via `journalctl`.
