#!/usr/bin/env bash
set -euo pipefail

ROOT_DIR=$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)
CONFIG_PATH=${1:-"$ROOT_DIR/config/icloud-imap-fetcher.example.conf"}

cd "$ROOT_DIR"
make clean
make
"$ROOT_DIR/bin/icloud-imap-fetcher-c" --config "$CONFIG_PATH"
