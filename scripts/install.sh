#!/usr/bin/env bash
set -euo pipefail

ROOT_DIR=$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)
PREFIX=/usr/local
SYSCONFDIR=/etc
LOCALSTATEDIR=/var
UNITDIR=/etc/systemd/system
CONFIG_SOURCE="$ROOT_DIR/config/icloud-imap-fetcher.example.conf"

usage() {
  cat <<USAGE
Usage: $0 [options]

Options:
  --prefix <dir>          Installation prefix for binary and docs
  --sysconfdir <dir>      Base directory for configuration files
  --localstatedir <dir>   Base directory for data and logs
  --unitdir <dir>         systemd unit destination directory
  --config-source <file>  Configuration template to install
  --help                  Show this help
USAGE
}

while [[ $# -gt 0 ]]; do
  case "$1" in
    --prefix)
      PREFIX="$2"
      shift 2
      ;;
    --sysconfdir)
      SYSCONFDIR="$2"
      shift 2
      ;;
    --localstatedir)
      LOCALSTATEDIR="$2"
      shift 2
      ;;
    --unitdir)
      UNITDIR="$2"
      shift 2
      ;;
    --config-source)
      CONFIG_SOURCE="$2"
      shift 2
      ;;
    --help)
      usage
      exit 0
      ;;
    *)
      echo "Unknown option: $1" >&2
      usage >&2
      exit 1
      ;;
  esac
done

BIN_DIR="$PREFIX/bin"
DOC_DIR="$PREFIX/share/doc/icloud-imap-fetcher-c"
CONFIG_DIR="$SYSCONFDIR/icloud-imap-fetcher-c"
CONFIG_TARGET="$CONFIG_DIR/config.conf"
DATA_DIR="$LOCALSTATEDIR/lib/icloud-imap-fetcher-c"
DOWNLOAD_DIR="$DATA_DIR/downloads"
PROCESSED_DIR="$DATA_DIR/processed"
LOG_DIR="$LOCALSTATEDIR/log/icloud-imap-fetcher-c"
SERVICE_TARGET="$UNITDIR/icloud-imap-fetcher-c.service"
TIMER_TARGET="$UNITDIR/icloud-imap-fetcher-c.timer"

cd "$ROOT_DIR"
make clean
make

install -d "$BIN_DIR"
install -d "$DOC_DIR"
install -d "$CONFIG_DIR"
install -d "$DATA_DIR"
install -d "$DOWNLOAD_DIR"
install -d "$PROCESSED_DIR"
install -d "$LOG_DIR"
install -d "$UNITDIR"

install -m 0755 "$ROOT_DIR/bin/icloud-imap-fetcher-c" "$BIN_DIR/icloud-imap-fetcher-c"
install -m 0644 "$ROOT_DIR/README.md" "$DOC_DIR/README.md"
install -m 0644 "$ROOT_DIR/docs/architecture.md" "$DOC_DIR/architecture.md"
install -m 0644 "$ROOT_DIR/docs/configuration.md" "$DOC_DIR/configuration.md"
install -m 0644 "$ROOT_DIR/docs/deployment.md" "$DOC_DIR/deployment.md"
install -m 0644 "$ROOT_DIR/docs/technical-specification.md" "$DOC_DIR/technical-specification.md"
sed -e "s#__PREFIX__#$PREFIX#g" -e "s#__SYSCONFDIR__#$SYSCONFDIR#g" \
  "$ROOT_DIR/packaging/systemd/icloud-imap-fetcher-c.service" > "$SERVICE_TARGET"
install -m 0644 "$ROOT_DIR/packaging/systemd/icloud-imap-fetcher-c.timer" "$TIMER_TARGET"
chmod 0644 "$SERVICE_TARGET" "$TIMER_TARGET"

if [[ ! -f "$CONFIG_TARGET" ]]; then
  install -m 0600 "$CONFIG_SOURCE" "$CONFIG_TARGET"
fi

echo "Installation complete"
echo "Binary:        $BIN_DIR/icloud-imap-fetcher-c"
echo "Config:        $CONFIG_TARGET"
echo "Data:          $DATA_DIR"
echo "Downloads:     $DOWNLOAD_DIR"
echo "Processed:     $PROCESSED_DIR"
echo "Logs:          $LOG_DIR"
echo "Docs:          $DOC_DIR"
echo "Service unit:  $SERVICE_TARGET"
echo "Timer unit:    $TIMER_TARGET"
