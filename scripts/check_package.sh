#!/usr/bin/env bash
set -euo pipefail
pkgdir="${1:-$(pwd)/build/package}"
if [ ! -d "$pkgdir" ]; then
  echo "Package directory not found: $pkgdir" >&2
  exit 2
fi
zip=$(ls "$pkgdir"/*.zip 2>/dev/null | head -n1 || true)
if [ -z "$zip" ]; then
  zip=$(ls "$pkgdir"/*.tar.gz 2>/dev/null | head -n1 || true)
fi
if [ -z "$zip" ]; then
  echo "No package archive found in $pkgdir" >&2
  exit 2
fi
echo "Found package: $zip"
# check for source files inside archive
if [[ "$zip" == *.zip ]]; then
  if command -v 7z >/dev/null 2>&1; then
    if 7z l -ba "$zip" | grep -E "(\.cpp$|\.c$|\.h$|\.hpp$)" >/dev/null; then
      echo "Source files (*.cpp/*.h) found in package" >&2
      exit 1
    fi
    if ! 7z l -ba "$zip" | grep -i -E "README(\.md)?$" >/dev/null; then
      echo "README.md not found in archive" >&2
      exit 1
    fi
  else
    if unzip -l "$zip" | awk '{print $4}' | grep -E "(\.cpp$|\.c$|\.h$|\.hpp$)" >/dev/null; then
      echo "Source files (*.cpp/*.h) found in package" >&2
      exit 1
    fi
    if ! unzip -l "$zip" | awk '{print $4}' | grep -i -E "README(\.md)?$" >/dev/null; then
      echo "README.md not found in archive" >&2
      exit 1
    fi
  fi
else
  if tar -tzf "$zip" | grep -E "(\.cpp$|\.c$|\.h$|\.hpp$)" >/dev/null; then
    echo "Source files (*.cpp/*.h) found in package" >&2
    exit 1
  fi
  if ! tar -tzf "$zip" | grep -i -E "README(\.md)?$" >/dev/null; then
    echo "README.md not found in archive" >&2
    exit 1
  fi
fi

echo "Package verification passed: no source files, README.md present"
exit 0

