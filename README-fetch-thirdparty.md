Этот репозиторий содержит скрипт для автоматического скачивания и распаковки релизов GLEW 2.2.0 и GLFW 3.4 из GitHub.

Использование (Windows PowerShell):

```powershell
# из корня репозитория
.
\scripts\fetch_thirdparty.ps1
# или принудительно перескачать
.
\scripts\fetch_thirdparty.ps1 -Force
```

После успешного скачивания выполните обычную конфигурацию и сборку:

```powershell
cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug
cmake --build build --config Debug
```

Примечание: скрипт распакует архива в папку `thirdparty/glew` и `thirdparty/glfw-3.4`. Если вы хотите использовать другой путь или версию, отредактируйте `scripts/fetch_thirdparty.ps1`.

