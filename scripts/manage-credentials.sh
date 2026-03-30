#!/usr/bin/env bash
set -eu

umask 077

APP_NAME="icloud-imap-fetcher-c"
CREDSTORE_BASE="/etc/credstore/$APP_NAME"

usage() {
  cat <<USAGE
Usage:
  $0 --set <credential_name>
  $0 --set <credential_name> --from-file <path>
  $0 --set-many <name1,name2,...>

Examples:
  $0 --set mail.icloud.user
  $0 --set mail.icloud.pswrd
  $0 --set mail.icloud.pswrd --from-file /root/secure/mail.icloud.pswrd
USAGE
}

require_root() {
  if [ "$(id -u)" -ne 0 ]; then
    echo "this script must run as root" >&2
    exit 1
  fi
}

validate_name() {
  case "$1" in
    ""|*/*)
      return 1
      ;;
    *[!A-Za-z0-9._-]*)
      return 1
      ;;
    *)
      return 0
      ;;
  esac
}

write_credential_atomically() {
  name="$1"
  target="$CREDSTORE_BASE/$name"
  tmp="$(mktemp "$CREDSTORE_BASE/.${name}.tmp.XXXXXX")"

  trap 'rm -f "$tmp"' EXIT HUP INT TERM

  if [ -n "${FROM_FILE:-}" ]; then
    if [ ! -f "$FROM_FILE" ]; then
      echo "source file not found for $name" >&2
      exit 1
    fi
    cat "$FROM_FILE" > "$tmp"
  else
    printf "enter value for %s: " "$name" >&2
    stty -echo
    IFS= read -r secret || true
    stty echo
    printf "\n" >&2

    if [ -z "${secret:-}" ]; then
      echo "credential value cannot be empty for $name" >&2
      exit 1
    fi

    printf '%s' "$secret" > "$tmp"
    unset secret
  fi

  chown root:root "$tmp"
  chmod 0600 "$tmp"
  mv -f "$tmp" "$target"
  trap - EXIT HUP INT TERM

  echo "credential updated: $name"
}

require_root
mkdir -p "$CREDSTORE_BASE"
chown root:root "$CREDSTORE_BASE"
chmod 0700 "$CREDSTORE_BASE"

MODE=""
NAMES=""
FROM_FILE=""

while [ "$#" -gt 0 ]; do
  case "$1" in
    --set)
      MODE="set"
      NAMES="$2"
      shift 2
      ;;
    --set-many)
      MODE="set-many"
      NAMES="$2"
      shift 2
      ;;
    --from-file)
      FROM_FILE="$2"
      shift 2
      ;;
    --help)
      usage
      exit 0
      ;;
    *)
      echo "unknown option: $1" >&2
      usage >&2
      exit 1
      ;;
  esac
done

if [ -z "$MODE" ] || [ -z "$NAMES" ]; then
  usage >&2
  exit 1
fi

IFS=','
for name in $NAMES; do
  if ! validate_name "$name"; then
    echo "invalid credential name: $name" >&2
    exit 1
  fi
  write_credential_atomically "$name"
done
unset IFS
