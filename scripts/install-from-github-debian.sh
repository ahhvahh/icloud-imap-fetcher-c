#!/usr/bin/env bash
set -euo pipefail

DEFAULT_REPO_URL="https://github.com/icloud-imap-fetcher-c/icloud-imap-fetcher-c.git"
REPO_URL="$DEFAULT_REPO_URL"
BRANCH=""
KEEP_TEMP_DIR=false
INSTALL_ARGS=()

usage() {
  cat <<USAGE
Uso: $0 [opções] [-- <argumentos para scripts/install.sh>]

Este script (Debian/Ubuntu):
1) Instala dependências de build
2) Clona o projeto em uma pasta temporária
3) Compila com make
4) Executa a instalação com scripts/install.sh

Opções:
  --repo-url <url>    URL do repositório Git (padrão: $DEFAULT_REPO_URL)
  --branch <nome>     Branch, tag ou commit para checkout
  --keep-temp-dir     Não remove a pasta temporária ao final
  --help              Exibe esta ajuda

Exemplo:
  sudo $0 --repo-url https://github.com/SEU_USUARIO/icloud-imap-fetcher-c.git --branch main -- --prefix /usr/local
USAGE
}

while [[ $# -gt 0 ]]; do
  case "$1" in
    --repo-url)
      REPO_URL="$2"
      shift 2
      ;;
    --branch)
      BRANCH="$2"
      shift 2
      ;;
    --keep-temp-dir)
      KEEP_TEMP_DIR=true
      shift
      ;;
    --help)
      usage
      exit 0
      ;;
    --)
      shift
      INSTALL_ARGS+=("$@")
      break
      ;;
    *)
      echo "Opção desconhecida: $1" >&2
      usage >&2
      exit 1
      ;;
  esac
done

if [[ ${EUID:-$(id -u)} -ne 0 ]]; then
  echo "Este script precisa ser executado como root (use sudo)." >&2
  exit 1
fi

if ! command -v apt-get >/dev/null 2>&1; then
  echo "apt-get não encontrado. Este instalador é específico para Debian/Ubuntu." >&2
  exit 1
fi

echo "[1/4] Instalando dependências de build..."
apt-get update
DEBIAN_FRONTEND=noninteractive apt-get install -y --no-install-recommends \
  ca-certificates git make gcc libc6-dev libcurl4-openssl-dev

temp_dir=$(mktemp -d -t icloud-imap-fetcher-c.XXXXXX)
cleanup() {
  if [[ "$KEEP_TEMP_DIR" == true ]]; then
    echo "Diretório temporário preservado em: $temp_dir"
  else
    rm -rf "$temp_dir"
  fi
}
trap cleanup EXIT

echo "[2/4] Clonando repositório em: $temp_dir"
git clone "$REPO_URL" "$temp_dir/repo"

if [[ -n "$BRANCH" ]]; then
  echo "Fazendo checkout de: $BRANCH"
  git -C "$temp_dir/repo" checkout "$BRANCH"
fi

echo "[3/4] Compilando projeto..."
make -C "$temp_dir/repo" clean
make -C "$temp_dir/repo"

echo "[4/4] Instalando no sistema..."
"$temp_dir/repo/scripts/install.sh" "${INSTALL_ARGS[@]}"

echo "Instalação concluída com sucesso."
