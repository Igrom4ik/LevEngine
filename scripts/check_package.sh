#!/usr/bin/env bash
set -euo pipefail
# Candidate package directories to search (search order)
candidates=(
  "${1:-}"
  "$(pwd)/build/package"
  "$(pwd)/cmake-build-release/package"
  "$(pwd)/cmake-build-debug/package"
  "$(pwd)/build_ci/package"
  "$(pwd)/package"
)

pkgdir=""
for d in "${candidates[@]}"; do
  if [ -n "$d" ] && [ -d "$d" ]; then
    pkgdir="$d"
    break
  fi
done

if [ -z "$pkgdir" ]; then
  echo "No package directory found among candidates:" >&2
  printf '%s
' "${candidates[@]}" >&2
  exit 2
fi

echo "Using package directory: $pkgdir"
# pick newest archive (zip or tar.gz)
archive=$(find "$pkgdir" -maxdepth 1 -type f \( -iname "*.zip" -o -iname "*.tar.gz" -o -iname "*.tgz" \) -printf '%T@ %p\n' 2>/dev/null | sort -nr | head -n1 | awk '{print $2}')
if [ -z "$archive" ]; then
  echo "No package archive found in $pkgdir" >&2
  exit 2
fi
zip="$archive"

echo "Found package: $zip"

echo "Archive listing:"
if [[ "$zip" == *.zip ]]; then
  if command -v 7z >/dev/null 2>&1; then
    7z l -ba "$zip"
  else
    unzip -l "$zip" || true
  fi
else
  tar -tzf "$zip" || true
fi

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
