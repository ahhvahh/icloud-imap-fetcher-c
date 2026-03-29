# Publicação e instalação

## Objetivo

Padronizar a instalação do projeto em Linux com paths previsíveis e compatíveis com FHS.

## Build local

```bash
make clean
make
```

## Instalação assistida por script

O script oficial é `scripts/install.sh`.

### Parâmetros suportados

- `--prefix`: raiz de instalação do binário e documentação.
- `--sysconfdir`: raiz para arquivos de configuração.
- `--localstatedir`: raiz para dados persistidos e logs.
- `--unitdir`: diretório de units do `systemd`.
- `--config-source`: arquivo base de configuração a ser instalado.

### Exemplo padrão

```bash
su -c './scripts/install.sh'
```

### Exemplo customizado

```bash
su -c './scripts/install.sh \
  --prefix /opt/icloud-imap-fetcher \
  --sysconfdir /etc \
  --localstatedir /srv \
  --unitdir /etc/systemd/system'
```

## systemd

O projeto inclui um service e um timer prontos para instalação.
