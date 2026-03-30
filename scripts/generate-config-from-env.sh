#!/usr/bin/env bash
set -euo pipefail

PROJECT_ROOT=${PROJECT_ROOT:-$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)}
OUTPUT_PATH=${1:-"$PROJECT_ROOT/.runtime/generated.config.conf"}

IMAP_CREDENTIAL_USERNAME_NAME=${IMAP_CREDENTIAL_USERNAME_NAME:-mail.icloud.user}
IMAP_CREDENTIAL_PASSWORD_NAME=${IMAP_CREDENTIAL_PASSWORD_NAME:-mail.icloud.pswrd}
IMAP_URL=${IMAP_URL:-imaps://imap.mail.me.com}
IMAP_MAILBOX=${IMAP_MAILBOX:-INBOX}
IMAP_SEARCH_FILTER=${IMAP_SEARCH_FILTER:-UNSEEN}
IMAP_MARK_AS_READ=${IMAP_MARK_AS_READ:-false}
MAIL_FROM_CONTAINS=${MAIL_FROM_CONTAINS:-}
MAIL_SUBJECT_CONTAINS=${MAIL_SUBJECT_CONTAINS:-}
ATTACHMENT_NAME_PREFIX=${ATTACHMENT_NAME_PREFIX:-mail_}
MAX_MESSAGES_PER_RUN=${MAX_MESSAGES_PER_RUN:-5}
DRY_RUN=${DRY_RUN:-false}
MAIL_DEST_DIR=${MAIL_DEST_DIR:-$PROJECT_ROOT/.runtime/messages}
ATTACHMENT_DEST_DIR=${ATTACHMENT_DEST_DIR:-$PROJECT_ROOT/.runtime/attachments}
LOG_DIR=${LOG_DIR:-$PROJECT_ROOT/.runtime/logs}
LOG_RETENTION_DAYS=${LOG_RETENTION_DAYS:-7}
LOG_LEVEL=${LOG_LEVEL:-info}
INTERVAL_SECONDS=${INTERVAL_SECONDS:-300}

mkdir -p "$(dirname "$OUTPUT_PATH")"
mkdir -p "$MAIL_DEST_DIR"
mkdir -p "$ATTACHMENT_DEST_DIR"
mkdir -p "$LOG_DIR"

cat > "$OUTPUT_PATH" <<EOF
[icloud]
credential_username_name = $IMAP_CREDENTIAL_USERNAME_NAME
credential_password_name = $IMAP_CREDENTIAL_PASSWORD_NAME
imap_url = $IMAP_URL
mailbox = $IMAP_MAILBOX
search_filter = $IMAP_SEARCH_FILTER
mark_as_read = $IMAP_MARK_AS_READ

[message_filter]
from_contains = $MAIL_FROM_CONTAINS
subject_contains = $MAIL_SUBJECT_CONTAINS
attachment_name_prefix = $ATTACHMENT_NAME_PREFIX
max_messages_per_run = $MAX_MESSAGES_PER_RUN
dry_run = $DRY_RUN

[storage]
download_dir = $ATTACHMENT_DEST_DIR
processed_dir = $MAIL_DEST_DIR

[logging]
log_dir = $LOG_DIR
retention_days = $LOG_RETENTION_DAYS
level = $LOG_LEVEL

[schedule]
interval_seconds = $INTERVAL_SECONDS
EOF

echo "$OUTPUT_PATH"
