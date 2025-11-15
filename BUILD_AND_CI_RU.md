BUILD, VERSIONING и CI (GitHub Actions)
=======================================

Цель
----
Этот документ описывает как:

- хранится и распространяется версия проекта;
- как настроена и запускается сборка и упаковка (CPack/CMake);
- как CI (GitHub Actions) собирает, проверяет и упаковывает артефакты;
- краткие инструкции для разработчиков: как менять версию, чистить временные файлы, создавать CI-теги и запускать сборку
  локально.

1. Где хранится версия

----------------------

- В корневом `CMakeLists.txt` обычно указана строка вида:

  project(HuyEngine VERSION 1.0.0 LANGUAGES CXX)

  Это "источник правды" для CMake на этапе конфигурации. Однако полезно сделать версию более гибкой — хранить в
  переменной, поддерживать переопределение через командную строку (-D) и/или получать из git-тега на CI.

- В проекте встречаются и другие файлы, которые получают информацию о версии через CMake, например `Version.h.in`,
  `version.txt.in` и т.д. Убедитесь, что они генерируются через `configure_file()` и берут значение переменной CMake.

Рекомендуемая надёжная схема в `CMakeLists.txt`:

- Определять значение версии через кэшируемую переменную, которую можно переопределить при вызове CMake (локально или из
  CI).

Пример (рекомендуется вставить в начало корневого CMakeLists.txt перед вызовом project()):

# Установить значение по умолчанию, можно переопределить при вызове cmake -DPROJECT_VERSION=...

set(PROJECT_VERSION "1.0.0" CACHE STRING "Project version (override with -DPROJECT_VERSION)")
project(HuyEngine VERSION ${PROJECT_VERSION} LANGUAGES CXX)

# Генерация заголовка с версией

configure_file(${CMAKE_SOURCE_DIR}/Version.h.in ${CMAKE_BINARY_DIR}/App/Version.h @ONLY)

Дополнительный вариант: на CI читать git tag и прокидывать в CMake через -DPROJECT_VERSION (лучший вариант при
использовании git-тэгов как источника правды).

2. Предложенная политика версий: git-теги как источник правды

----------------------------------------------------------

- Для релизов используем аннотированные теги формата `vX.Y.Z` (например, `v1.2.3`).
- Для CI-прогонов можно автоматически помечать билд тэгом `v{версия}-ci-{shortsha}` — см. `scripts/create_ci_tag.ps1`.
- На CI (GitHub Actions) при сборке релиза лучше определять версию так:
    - если сборка запущена по тэгу — взять версию из имени тега (строка после 'v');
    - иначе — можно использовать `PROJECT_VERSION` из CMakeLists (или сформировать CI-версию с суффиксом -ci-<sha>).

Пример в CI (псевдо-логика):

- если event == push && тэг присутствует: VERSION = tag (без ведущего v)
- иначе: VERSION = содержимое CMakeLists (или default) и дополнить short SHA для артефакта

3. Скрипт `scripts/create_ci_tag.ps1`

------------------------------------

- Этот скрипт читает `CMakeLists.txt`, извлекает тройную версию вида 0.0.0 и создаёт тег `v{версия}-ci-{shortsha}`.
- Скрипт затем пушит тег в origin.

Замечания:

- Скрипт полезен как вспомогательный при локальном создании CI-тега, но в CI обычно теги создаются/управляются в
  workflow (или вручную админом).
- Также важно, чтобы версия в `CMakeLists.txt` соответствовала ожидаемой «основной» версии, иначе теги будут иметь
  неожиданный номер.

4. Упаковка и CPack — почему README.md должен быть в архиве

-----------------------------------------------------------

- CI-верификация ожидает, что в итоговом ZIP-архиве присутствует `README.md` в корне архива.
- Это достигается добавлением install(FILES ...) в CMake, чтобы README оказался в каталоге установки, откуда CPack
  делает архив.

Добавьте в корневой `CMakeLists.txt` (секция инсталляции):

install(FILES ${CMAKE_SOURCE_DIR}/README.md DESTINATION .)

После этого, при выполнении `cmake --install` и `cpack -G ZIP` README попадёт в корень архива.

Не забудьте подключить `include(CPack)` и задать генератор в CPack-конфиге, если требуется:

set(CPACK_GENERATOR "ZIP")
include(CPack)

5. Проблемы с shell в GitHub Actions (Windows runners)

----------------------------------------------------

- На Windows runner по умолчанию шаги с `run:` выполняются в PowerShell, а не в Bash. Если в `run:` используется
  Bash-стиль (`if [ -n "..." ]`), то шаг упадёт с ошибкой "Missing '(' after 'if'".
- Решение: для шагов, где используется Unix-стиль синтаксис, явно указывать `shell: bash` в workflow.

Пример исправленного шага в `.github/workflows/package-and-verify.yml`:

- name: Configure
  run: |
  if [ -n "${{ steps.set_version.outputs.version }}" ]; then
  echo "Configuring with version ${{ steps.set_version.outputs.version }}"
  cmake -S . -B build -DPROJECT_VERSION=${{ steps.set_version.outputs.version }}
  else
  echo "Configuring without explicit version"
  cmake -S . -B build
  fi
  shell: bash

Повторите для всех шагов, использующих Bash-специфику на Windows-раннерах.

6. Полный пример последовательности сборки (локально/CI)

------------------------------------------------------

1) Настроить конфигурацию и указать версию (если нужно переопределить):

cmake -S . -B build -DPROJECT_VERSION=1.2.3

2) Сборка:

cmake --build build --config Release -- -j

3) Установка в staging/папку инсталляции (по умолчанию prefix):

cmake --install build --prefix "$PWD/staging"

4) Убедиться, что `README.md` и другие нужные файлы присутствуют в `staging` (или `install`):

ls -R staging

5) Запустить упаковку:

cd staging
cpack -G ZIP

6) Проверка содержимого архива (примерный check в workflow):

unzip -l artifact.zip > contents.txt
grep -q -E '(^|/)(README.md)$' contents.txt || (echo "README.md not found in archive" && exit 1)

7. Команды для разработчиков

----------------------------

- Обновить версию в `CMakeLists.txt` (локально):

  # bash
  git checkout main
  git pull origin main
  # отредактировать CMakeLists.txt и закоммитить
  git add CMakeLists.txt
  git commit -m "Bump version to X.Y.Z"
  git push origin main

- Создать тег релиза вручную (если нужна точная версия):

  git tag -a vX.Y.Z -m "Release vX.Y.Z"
  git push origin vX.Y.Z

- Запустить автогенерацию CI-тега (Windows PowerShell):

  pwsh scripts\create_ci_tag.ps1

  (скрипт прочитает CMakeLists.txt, возьмёт версию и short sha и создаст тег вида v{версия}-ci-{sha})

8. Как аккуратно удалять временные/staging каталоги (правильный PowerShell)

------------------------------------------------------------------------
Ниже один из безопасных способов найти и удалить каталоги/файлы по шаблону из корня репозитория (работает в PowerShell):

$patterns = @('build_validate*','build_version_test*','build_temp','pkg_extract','staging*')
foreach ($pattern in $patterns) {
Get-ChildItem -Path $PWD -Filter $pattern -Force -ErrorAction SilentlyContinue | ForEach-Object {
Write-Host "Removing $($_.FullName)"
Remove-Item -Recurse -Force -ErrorAction SilentlyContinue $_.FullName
}
}

Пример команды для *nix:

rm -rf build_validate* build_version_test* build_temp pkg_extract* staging*

9. Небольшие рекомендации и checklist для Pull Request / CI

-----------------------------------------------------------

- Убедиться, что `install(FILES ${CMAKE_SOURCE_DIR}/README.md DESTINATION .)` добавлен и `cmake --install` помещает
  README в staging.
- Проверить, что workflow шаги, где используется Bash-стиль, содержат `shell: bash` (особенно при матрице с
  `windows-latest`).
- Для релизов назначать аннотированный тег `vX.Y.Z`. CI должен детектировать тэг и собрать релиз, либо принять
  `-DPROJECT_VERSION` как вход.
- Прописать создание CPack-конфига (CPackReleaseConfig.cmake) для единообразной упаковки.

10. Часто встречающиеся вопросы (FAQ)

-------------------------------------
Q: Я меняю версию в строке `project(...)` — хватит ли этого, чтобы все файлы обновились?
A: Если проект настроен правильно (все версии создаются через `configure_file()` и берут значение из CMake-переменной),
то да. Если в каких-то местах версия захардкожена вручную — нужно заменить это на использование `configure_file()` и
шаблонного файла (`Version.h.in`, `version.txt.in`).

Q: Хочу, чтобы в репозитории "источником версий" были git-теги. Как это внедрить?
A: В CI — читать текущий тэг (если сборка запущена по тэгу) и прокидывать его в CMake через `-DPROJECT_VERSION`.
Локально можно либо вручную изменить CMakeLists.txt, либо настроить локальные скрипты, которые обновляют
CMakeLists/требуемые файлы и коммитят изменения.

11. Что можно сделать дальше (улучшения)
---------------------------------------
- Переключиться на автоматическое извлечение версии из git tags в CMake при сборке в CI (например, `git describe --tags --abbrev=0`).
- Добавить тест workflow, который собирает проект и проверяет содержимое архива на каждый PR.
- Автоматизировать bump версии через pull request template или специальную action.

12. Адаптация под MSVC (Visual Studio / Windows)
------------------------------------------------
Ниже собраны практические рекомендации и примеры команд для сборки проекта с помощью MSVC / Visual Studio (локально и в CI), а также особенности настройки CMake/CPack для Windows.

12.1. Выбор генератора и архитектуры
- Для сборки под MSVC используйте генератор Visual Studio: например, "Visual Studio 17 2022" для VS2022.
- Явно указывайте архитектуру: `-A x64` или `-A Win32`.

Пример конфигурации (PowerShell / cmd):

```powershell
cmake -S . -B build -G "Visual Studio 17 2022" -A x64 -DPROJECT_VERSION=1.2.3
```

Для новых версий CMake можно не указывать `-G`, а только `-A`, если вызываете из VS Command Prompt / developer prompt.

12.2. Multi-config сборка
- MSVC использует multi-config генераторы: сборки разделяются по конфигурации (Debug/Release).
- При сборке и установке обязательно указывать конфигурацию:

```powershell
cmake --build build --config Release -- /m
cmake --install build --config Release --prefix "%CD%\staging"
```

Флаг `/m` (MSBuild) включает параллельную сборку.

12.3. Настройка runtime и флагов MSVC
- В современных CMake (3.15+) предпочтительно контролировать runtime через `CMAKE_MSVC_RUNTIME_LIBRARY` (например `MultiThreadedDLL` => `/MD`).

В `CMakeLists.txt` можно задать:

```cmake
if(MSVC)
  # Для всех конфигураций: использовать динамическую runtime библиотеку
  set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreadedDLL" CACHE STRING "MSVC runtime")
  # Примеры дополнительных опций
  add_compile_options($<$<CONFIG:Release>:/O2>)
  add_compile_options(/bigobj)
endif()
```

- Если проект требует `/MT` (статическая CRT) — задайте `MultiThreaded` в `CMAKE_MSVC_RUNTIME_LIBRARY`.

12.4. Учет различий в коде и препроцессор
- Для несовместимого с MSVC кода (inline asm, GCC-специфичные расширения) используйте препроцессорные макросы:

```cmake
if(MSVC)
  target_compile_definitions(${PROJECT_NAME} PRIVATE PLATFORM_MSVC=1)
else()
  target_compile_definitions(${PROJECT_NAME} PRIVATE PLATFORM_POSIX=1)
endif()
```

- В исходниках оборачивайте платформозависимый код:

```cpp
#ifdef PLATFORM_MSVC
  // MSVC-специфичная реализация
#else
  // POSIX / GCC
#endif
```

12.5. CPack и упаковка на Windows
- CPack поддерживает ZIP и NSIS/ZIP с настройками. На Windows укажите генератор и конфигурацию:

```powershell
# из папки проекта (после cmake --install)
cd staging
cpack -G ZIP -C Release
```

- Убедитесь, что `install(FILES ${CMAKE_SOURCE_DIR}/README.md DESTINATION .)` присутствует (см. раздел 4). Также полезно задать имя пакета:

```cmake
set(CPACK_PACKAGE_FILE_NAME "${PROJECT_NAME}-${PROJECT_VERSION}-${CMAKE_SYSTEM_NAME}")
set(CPACK_GENERATOR "ZIP")
include(CPack)
```

12.6. Пример GitHub Actions шага для MSVC (Windows)
- На `windows-latest` runner по умолчанию shell — PowerShell. Ниже пример шага, который использует MSVC, билдит и упаковывает (исполняется в PowerShell):

```yaml
- name: Configure, build and package (Windows)
  shell: pwsh
  run: |
    # Настроить конфигурацию
    cmake -S . -B build -G "Visual Studio 17 2022" -A x64 -DPROJECT_VERSION=${{ steps.set_version.outputs.version }}
    # Собрать Release
    cmake --build build --config Release -- /m
    # Установить в staging
    cmake --install build --config Release --prefix "$PWD\staging"
    # Перейти в staging и запаковать
    Push-Location staging
    cpack -G ZIP -C Release
    Pop-Location
```

- Если в шаге используется Bash-стиль синтаксиса, явно указывайте `shell: bash`. Но для MSVC-команд на Windows удобнее оставить `shell: pwsh`.

12.7. Частые проблемы и их решения на MSVC
- "Linker LNK1181 / cannot open file" — проверьте пути установки и исполняемых библиотек, убедитесь, что `cmake --install` положил все требуемые .lib/.dll.
- Отсутствие README в архиве — проверьте `install(FILES ...)` и `cmake --install` с нужной конфигурацией.
- Конфликт runtime (DLL vs static) — согласуйте `CMAKE_MSVC_RUNTIME_LIBRARY` для всех зависимостей.
- Пара строк в workflow используют Bash-стиль на Windows — либо перевести их в PowerShell синтаксис, либо добавить `shell: bash`.

12.8. Локальная отладка в Visual Studio
- После `cmake -S . -B build -G "Visual Studio 17 2022" -A x64` можно открыть сгенерированный `.sln` в Visual Studio и использовать GUI для билдов/отладки.

12.9. Примеры команд для разработчика (PowerShell)

```powershell
# configure
cmake -S . -B build -G "Visual Studio 17 2022" -A x64 -DPROJECT_VERSION=1.2.3
# build Release
cmake --build build --config Release -- /m
# install in staging
cmake --install build --config Release --prefix "$PWD\staging"
# pack
cd staging
cpack -G ZIP -C Release
```

12.10. CI-заметки
- На CI для Windows используйте `actions/setup-msbuild@v1` или просто `runs-on: windows-latest` — у runner'а уже есть Visual Studio.
- Для консистентности с Linux/ macOS используйте шаги, которые для каждой платформы задают корректный генератор и конфигурацию (см. matrix в workflow).

---

Если нужно, могу:

- Сделать коммит этого файла и запушить в ветку (с вашего разрешения и авторизацией),
- Внести пример правок в `CMakeLists.txt` (добавить install(FILES ...) и пример механизма чтения версии из переменной),
- Обновить `.github/workflows/package-and-verify.yml` — добавить `shell: bash` в проблемные шаги и пример чтения версии
  из тега.

Напишите, какие из вышеуказанных действий выполнить автоматически (создать коммит, правки в CMake или workflow).
