# LevEngine

![status](https://img.shields.io/badge/status-experimental-orange)
![language](https://img.shields.io/badge/language-C++-00599C)
![license](https://img.shields.io/badge/license-MIT-blue)
![third-party](https://img.shields.io/badge/third--party-GLEW%20(BSD)-lightgrey)

<!-- CI status badges -->
[![Windows Build](https://github.com/Igrom4ik/LevEngine/actions/workflows/windows-build.yml/badge.svg)](https://github.com/Igrom4ik/LevEngine/actions/workflows/windows-build.yml)
[![clang-format](https://github.com/Igrom4ik/LevEngine/actions/workflows/clang-format-checker.yml/badge.svg)](https://github.com/Igrom4ik/LevEngine/actions/workflows/clang-format-checker.yml)

Коротко: LevEngine — это небольшой кроссплатформенный игровой движок на C++ с архитектурой, ориентированной на простоту расширения и интеграцию с современными средствами сборки (CMake, Ninja, Visual Studio). Проект содержит исходный код движка, сборочные сценарии и поставляется с набором сторонних библиотек в папке `vendor/`.

Основные возможности (текущее состояние):
- Ядро на C++ (один главный исполняемый файл `main.cpp`).
- Система сборки на CMake (поддержка генерации проектов для MSVC и Ninja).
- Включённая поддержка OpenGL через GLEW (в `vendor/glew`).
- Примеры и тесты — базовые примеры запуска графики и тесты Unicode в `Automation/`.
- Поддержка мультиплатформенной организации исходников и поставки бинарников (директории `build_vendor/`, `bin/`, `Debug/`).

Архитектура (кратко):
- source/ — исходники основного приложения (например, `main.cpp`).
- vendor/ — сторонние библиотеки и их исходники/билды (например, `glew`, `glfw`).
- build/ / cmake-build-debug/ / build_vendor/ — директории сборки и артефактов.
- scripts/ и Automation/ — скрипты сборки, упаковки и автоматизации.

Быстрый старт (Windows, разработка):
1) Откройте папку в вашей IDE (Clion, Visual Studio) или в терминале.
2) Создайте директорию сборки и запустите CMake:  
   - Через PowerShell:
     mkdir build && cd build
     cmake -S .. -B . -G "Ninja"
     cmake --build .
3) Запустите сгенерированный исполняемый файл (в `build/` или `bin/`).

Примеры команд для разработки (PowerShell):
```powershell
# создание и сборка
mkdir cmake-build-debug; cd cmake-build-debug
cmake .. -G "Ninja" -DCMAKE_BUILD_TYPE=Debug
cmake --build .
# запуск (пример)
.\Debug\LevEngine.exe
```

Лицензии
- Лицензия проекта: В репозитории не найден файл LICENSE в корне; по умолчанию в этом README указана лицензия MIT как временная метка. Если вы хотите другую лицензию — измените/добавьте файл `LICENSE` в корне репозитория, и я обновлю README.

  Рекомендация: если вы принимаете MIT, добавьте в корень файл `LICENSE` с полным текстом MIT License.

- Сторонние библиотеки:
  - GLEW: исходники и лицензия находятся в `vendor/glew/LICENSE.txt` (BSD-подобная лицензия). Пожалуйста, ознакомьтесь с ним при распространении бинарных сборок.
  - GLFW и другие библиотеки: см. соответствующие папки в `vendor/` для деталей.

Бейджи
- Статус сборки: ![status](https://img.shields.io/badge/status-experimental-orange)
- Язык: ![language](https://img.shields.io/badge/language-C++-00599C)
- Лицензия (предположение): ![license](https://img.shields.io/badge/license-MIT-blue)
- Сторонние: ![third-party](https://img.shields.io/badge/third--party-GLEW%20(BSD)-lightgrey)

Где смотреть дальше
- Документация по конфигурации сборки: `Automation/` и `scripts/`.
- Сторонние зависимости и версии: `cmake/ThirdPartyVersons.cmake` и `cmake/Dependencies.cmake`.

Как я могу помочь далее
- Добавить/вставить реальную лицензию проекта (MIT/Apache/GPL) и автоматически создать `LICENSE`.
- Добавить секцию "API" и подробное описание модулей (рендерер, сцена, ресурсы, ввод и т.п.).
- Сгенерировать шаблон документации (Doxygen/markdown) для модулей и примеров.

Если хотите — укажите предпочитаемую лицензию для проекта (или подтвердите MIT), и я добавлю файл `LICENSE` в корень и обновлю README с корректным бейджем.
