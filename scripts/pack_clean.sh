#!/usr/bin/env bash
set -euo pipefail
INSTALL_DIR="${1:-$(pwd)/cmake-build-release/install}"
OUT_DIR="${2:-$(pwd)/cmake-build-release/package}"
NAME="${3:-LevEngine-1.0.0-Linux.zip}"

echo "Packing clean archive from: $INSTALL_DIR"
if [ ! -d "$INSTALL_DIR" ]; then echo "Install dir not found: $INSTALL_DIR" >&2; exit 2; fi
mkdir -p "$OUT_DIR"
ZIPPATH="$OUT_DIR/$NAME"
rm -f "$ZIPPATH" || true

STAGING=$(mktemp -d)
trap 'rm -rf "$STAGING"' EXIT

cp -r "$INSTALL_DIR/bin" "$STAGING/" 2>/dev/null || true
cp -r "$INSTALL_DIR/lib" "$STAGING/" 2>/dev/null || true
if [ -f "$INSTALL_DIR/README.md" ]; then cp "$INSTALL_DIR/README.md" "$STAGING/"; fi

( cd "$STAGING" && zip -r "$ZIPPATH" . )

echo "Created: $ZIPPATH"
exit 0

