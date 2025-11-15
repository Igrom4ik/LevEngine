"""
Test Unicode output in automation system
"""

import io
import os
import sys

# Fix Unicode output for Windows console
if sys.platform == "win32":
    try:
        # Set UTF-8 encoding for stdout and stderr
        sys.stdout = io.TextIOWrapper(sys.stdout.buffer, encoding='utf-8', errors='replace')
        sys.stderr = io.TextIOWrapper(sys.stderr.buffer, encoding='utf-8', errors='replace')
    except Exception:
        pass

# Add Automation directory to path
sys.path.insert(0, os.path.dirname(__file__))


def test_unicode():
    """Test Unicode output"""
    print("=" * 70)
    print("ТЕСТ ВЫВОДА UNICODE")
    print("=" * 70)
    print()
    print("✓ Проверка русских символов")
    print("✓ Проверка специальных символов: ☺ ★ ♥ →")
    print("✓ Проверка эмодзи (если поддерживается): 🚀 ⚙️ ✨")
    print()
    print("Если вы видите этот текст корректно, значит Unicode работает!")
    print("=" * 70)


if __name__ == "__main__":
    test_unicode()
