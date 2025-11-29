# Ассеты, парсинг JSON и отладка crash/abort в LevEngine

Этот документ — краткое и актуальное руководство для разработчиков по работе с ассетами, JSON, и диагностике аварий (
abort) при запуске `LENApp.exe`.

Содержание

- Обзор: где лежат ассеты и как движок их ищет
- Быстрая проверка и запуск приложения (PowerShell)
- Автоматическое копирование ассетов при сборке (CMake snippet)
- Работа с JSON (парсер nlohmann::json): accept, BOM, исключения
- Что делать при появлении диалога "abort() has been called" и как собрать стек
- Быстрые решения и рекомендации

---

## 1) Обзор: где лежат ассеты и как их искать

- Исходная папка ассетов в проекте: `App/assets/`
    - Примеры: `App/assets/materials/*.mat`, `App/assets/textures/*.png`, `App/assets/shaders/*.glsl`.
- При запуске бинаря движок разрешает путь к ассетам относительно исполняемого файла:
    - По умолчанию `FileSystem::GetAssetsFolder()` возвращает `{exe-folder}/assets`.
    - При разработке удобно копировать `App/assets` в `cmake-build-debug/bin/assets` (или другой каталог запуска).

Если исполняемый файл запускается из `cmake-build-debug/bin`, то путь к текстуре `textures/brick.png` будет искаться как
`cmake-build-debug/bin/assets/textures/brick.png`.

## 2) Быстрая проверка и запуск (Windows PowerShell)

- Скопировать ассеты в папку запуска (выполнить из корня репозитория):

```powershell
# удалить старую папку assets в папке запуска (по желанию)
Remove-Item -Path .\cmake-build-debug\bin\assets -Recurse -Force -ErrorAction SilentlyContinue
# скопировать все ассеты
Copy-Item -Path .\App\assets -Destination .\cmake-build-debug\bin\assets -Recurse -Force
```

- Запустить приложение и сохранить stderr в файл:

```powershell
.\cmake-build-debug\bin\LENApp.exe 2> err.txt
notepad err.txt
```

- Если видите сообщение вида:

```
Texture::Load(): File not found: "D:\...\bin\assets\brick.png"
```

значит движок искал текстуру по другому относительному пути — проверьте, что в материалах и в коде указаны согласованные
пути (`textures/brick.png` либо `brick.png`, в зависимости от ожиданий кода).

## 3) Автоматическое копирование ассетов при сборке (CMake)

Добавьте в `CMakeLists.txt` (root или App/CMakeLists.txt) простую цель, которая копирует ассеты при сборке/установке:

```cmake
# Добавьте это в корневой CMakeLists или в таргет App
add_custom_target(copy_assets ALL
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_SOURCE_DIR}/App/assets
        ${CMAKE_BINARY_DIR}/bin/assets
        COMMENT "Copying assets to runtime folder"
)
# Если у вас есть таргет LENApp, добавьте зависимость
add_dependencies(LENApp copy_assets)
```

Это гарантирует, что при каждом `cmake --build` ассеты будут актуализироваться в папке запуска.

## 4) Работа с JSON (nlohmann::json)

- Базовый вызов парсинга:

```cpp
auto j = nlohmann::json::parse(contents);
```

- Проблемы, которые приводят к ошибкам парсинга:
    - Невалидный JSON (пропущенные запятые, комментарии, trailing commas и т.д.).
    - Кодировка: файл не в UTF-8 или содержит BOM (UTF-8 BOM — 0xEF 0xBB 0xBF).
    - Частично записанный или обрезанный файл.
- Безопасная предварительная проверка (рекомендуется в местах, где вход может быть ненадёжным):

```cpp
if (!nlohmann::json::accept(contents)) {
    // логируем и не парсим
}
else {
    auto j = nlohmann::json::parse(contents);
}
```

`json::accept` быстро проверяет, похож ли текст на JSON — это полезно если библиотека собрана в конфигурации без
исключений (в этом случае `parse` может вызывать `abort()` вместо `throw`).

- BOM: перед парсингом можно удалить UTF-8 BOM:

```cpp
if (contents.size() >= 3 && (unsigned char)contents[0] == 0xEF && (unsigned char)contents[1] == 0xBB && (unsigned char)contents[2] == 0xBF) {
    contents = contents.substr(3);
}
```

## 5) Почему появляется диалог "abort() has been called" и как это диагностировать

- `nlohmann::json` может быть собрана с выключенными исключениями (макрос `JSON_NOEXCEPTION` либо компилятор без
  исключений). В таком случае библиотека вызывает `std::abort()` при ошибках вместо броска исключения — вы увидите
  диалог CRT.
- Также `abort()` может быть вызван явно в коде/сторонних библиотеках при assert/ошибках.

Как получить стек вызовов (рекомендую Visual Studio):

1. Откройте проект/решение в Visual Studio.
2. В Debug -> Exception Settings включите «Break on thrown C++ exceptions» (опционально).
3. Запустите приложение в режиме Debug (F5).
4. При появлении диалога нажмите "Retry" — отладчик обычно остановит исполнение в месте вызова `abort()` и покажет Call
   Stack.
5. Скопируйте стек вызовов (Call Stack) и вставьте сюда — по нему можно точно определить, кто вызывает `abort()`.

Если вы не используете IDE — запустите exe в консоли и перенаправьте stderr (см. выше) — иногда библиотека печатает
диагностическое сообщение до abort.

## 6) Быстрые патчи и рекомендации (что делать немедленно)

- Всегда проверяйте ассеты на валидность JSON (локально можно сделать небольшой Python‑скрипт или C++ утилиту).
- Для стабильной работы в рантайме добавьте простые защиты:
    - В `Texture::Load` пробуйте несколько fallback путей (например, `textures/<name>`), но правильнее — использовать
      единый формат путей в ассетах.
    - В `ShaderProgram::SetTexture` проверьте `nullptr` перед использованием `texture->GetTextureID()`.
    - В точках, где парсите внешние файлы, сначала `json::accept`, затем `parse` в try/catch.

## 7) Примеры PowerShell команд для отладки

- Скопировать ассеты и запустить приложение, сохранив вывод:

```powershell
Copy-Item -Path .\App\assets -Destination .\cmake-build-debug\bin\assets -Recurse -Force
.\cmake-build-debug\bin\LENApp.exe 2> err.txt
Get-Content err.txt -Raw
```

## 8) Контакты и заметки для команды

- Если в логах встречаются `json::accept` ошибки — пришлите примеры файлов (первые несколько килобайт), я помогу
  диагностировать (BOM, кодировка, невидимые символы).
- Если вы хотите, чтобы я добавил простую цель CMake, которая копирует ассеты автоматически и только обновлённые файлы —
  скажите, и я подготовлю патч для `CMakeLists.txt`.

---

Этот документ можно расширить: добавить CI-проверку ассетов, тест-утилиту `tools/json_check`, и шаблоны для логов. Если
хотите — я внедрю одну из этих вещей автоматически.

