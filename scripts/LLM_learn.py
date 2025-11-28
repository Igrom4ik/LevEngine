import argparse
import hashlib
import json
import os
import re
import sys
from pathlib import Path
from typing import Dict, List, Optional

# Ensure stdout/stderr use UTF-8 encoding on Windows consoles to avoid UnicodeEncodeError
# This attempts to reconfigure Python's text IO streams when supported (Python 3.7+).
try:
    if hasattr(sys.stdout, 'reconfigure'):
        sys.stdout.reconfigure(encoding='utf-8', errors='replace')
    if hasattr(sys.stderr, 'reconfigure'):
        sys.stderr.reconfigure(encoding='utf-8', errors='replace')
except Exception:
    # Best-effort: if reconfigure is unavailable or fails, continue without crashing
    pass

# Скрипт для создания JSONL датасета для обучения на игровом движке

parser = argparse.ArgumentParser(description='Создание JSONL датасета из исходников игрового движка')
parser.add_argument('--root', '-r',
                    default=os.path.abspath(os.path.join(os.path.dirname(__file__), '..', 'Engine', 'Source')),
                    help='Корневая папка для сканирования')
parser.add_argument('--repo-root',
                    default=os.path.abspath(os.path.join(os.path.dirname(__file__), '..')),
                    help='Корень репозитория')
parser.add_argument('--out', '-o',
                    default=os.path.abspath(os.path.join('D:/deepseek', 'dataset.jsonl')),
                    help='Выходной JSONL файл')
parser.add_argument('--ext', '-e', nargs='+',
                    default=['.js', '.ts', '.jsx', '.tsx', '.h', '.hpp', '.cpp', '.c', '.py', '.md', '.json'],
                    help='Расширения файлов для включения')
parser.add_argument('--exclude', '-x', nargs='*',
                    default=['.git', 'node_modules', 'cmake-build-debug', 'build', 'bin', '.venv', 'dist'],
                    help='Имена директорий для исключения')
parser.add_argument('--excerpt-lines', type=int, default=100,
                    help='Количество строк для excerpt')
parser.add_argument('--max-tokens', type=int, default=8000,
                    help='Максимальное количество токенов на запись')
parser.add_argument('--stream', action='store_true',
                    help='Добавлять записи инкрементально')
parser.add_argument('--verbose', action='store_true')
parser.add_argument('--include-api-docs', action='store_true',
                    help='Включить генерацию API документации')
parser.add_argument('--include-examples', action='store_true',
                    help='Включить примеры кода')
parser.add_argument('--force-key-from-evidence', action='store_true',
                    help='Помечать как ключевые все файлы, где найдена технологическая evidence')

args = parser.parse_args()

ROOT = Path(args.root).resolve()
REPO = Path(args.repo_root).resolve()
OUT = Path(args.out).resolve()
EXTS = tuple(e.lower() for e in args.ext)
EXCLUDE = set(e.lower() for e in args.exclude)
EXCERPT_LINES = args.excerpt_lines
MAX_TOKENS = args.max_tokens
STREAM = args.stream
VERBOSE = args.verbose
INCLUDE_API_DOCS = args.include_api_docs
INCLUDE_EXAMPLES = args.include_examples
FORCE_KEY_FROM_EVIDENCE = args.force_key_from_evidence

BINARY_EXTS = {'.exe', '.dll', '.lib', '.pdb', '.ilk', '.o', '.obj', '.a', '.so',
               '.class', '.jar', '.pyc', '.pyo', '.wasm', '.bin', '.dat'}


def is_binary_by_ext(p: Path) -> bool:
    """Проверка является ли файл бинарным по расширению"""
    return p.suffix.lower() in BINARY_EXTS


def sha256_text(text: str) -> str:
    """Вычисление SHA256 хеша текста"""
    h = hashlib.sha256()
    h.update(text.encode('utf-8'))
    return h.hexdigest()


def estimate_tokens(text: str) -> int:
    """Примерная оценка количества токенов (1 токен ≈ 4 символа для русского/английского)"""
    return len(text) // 3


def extract_jsdoc_comments(code: str) -> List[str]:
    """Извлечение JSDoc комментариев из JavaScript кода"""
    pattern = r'/\*\*([\s\S]*?)\*/'
    return [m.group(1).strip() for m in re.finditer(pattern, code)]


def _strip_comments(s: str) -> str:
    """Удаляет C/C++ комментарии (/* ... */ и //...) из текста, чтобы избежать ложных совпадений при regex-поиске."""
    # Удаляем блочные комментарии /* ... */
    s = re.sub(r'/\*([\s\S]*?)\*/', '', s)
    # Удаляем однострочные //...
    s = re.sub(r'//.*', '', s)
    return s


def extract_functions(code: str, language: str) -> List[Dict]:
    """Извлечение функций из кода с их описанием"""
    functions = []

    if language in ['js', 'jsx', 'ts', 'tsx']:
        # JavaScript/TypeScript функции
        # function name() { ... }
        pattern = r'function\s+(\w+)\s*\([^)]*\)\s*{[^}]*}'
        for match in re.finditer(pattern, code):
            functions.append({
                'name': match.group(1),
                'code': match.group(0),
                'type': 'function'
            })

        # const name = () => { ... }
        pattern = r'(?:const|let|var)\s+(\w+)\s*=\s*(?:async\s*)?\([^)]*\)\s*=>\s*{[^}]*}'
        for match in re.finditer(pattern, code):
            functions.append({
                'name': match.group(1),
                'code': match.group(0),
                'type': 'arrow_function'
            })

    elif language in ['cpp', 'hpp', 'c', 'h']:
        # C++ функции
        pattern = r'(?:void|int|float|double|bool|\w+)\s+(\w+)\s*\([^)]*\)\s*{[^}]*}'
        for match in re.finditer(pattern, code):
            functions.append({
                'name': match.group(1),
                'code': match.group(0),
                'type': 'function'
            })

    return functions


def extract_api_methods(code: str) -> List[Dict]:
    """Извлечение API методов типа game.newRectObject"""
    methods = []

    # Паттерн для методов вида object.method(...)
    pattern = r'(\w+)\.(\w+)\s*\(([^)]*)\)'

    for match in re.finditer(pattern, code):
        obj_name = match.group(1)
        method_name = match.group(2)
        params = match.group(3)

        methods.append({
            'entity': f'{obj_name}.{method_name}',
            'object': obj_name,
            'method': method_name,
            'params': params.strip(),
            'full_call': match.group(0)
        })

    return methods


def extract_includes(code: str) -> List[str]:
    """Извлечение директив #include (как "...", так и <...>)"""
    includes = []
    for m in re.finditer(r'#include\s*[<\"]([^>\"]+)[>\"]', code):
        includes.append(m.group(1))
    return includes


def top_comment(code: str) -> str:
    """Извлечение верхнего комментария файла"""
    lines = code.splitlines()
    top = []

    # Извлечение // комментариев
    for ln in lines[:30]:
        s = ln.strip()
        if s.startswith('//'):
            top.append(s.lstrip('/').strip())
        elif s == '':
            if top:
                continue
        else:
            break

    if top:
        return '\n'.join(top)

    # Извлечение /* */ комментариев
    m = re.search(r"/\*([\s\S]*?)\*/", '\n'.join(lines[:200]))
    return (m.group(1).strip() if m else '')


def categorize_code(code: str, filename: str) -> Dict:
    """Категоризация кода по типу"""
    category_info = {
        'type': 'SOURCE_CODE',
        'category': 'GENERAL',
        'concepts': []
    }

    lower_code = code.lower()
    lower_name = filename.lower()

    # API методы
    if 'game.' in lower_code or 'api' in lower_name:
        category_info['type'] = 'API_METHOD'
        category_info['category'] = 'API'

    # Геометрия
    if any(kw in lower_code for kw in ['rect', 'circle', 'polygon', 'shape', 'geometry']):
        category_info['category'] = 'GEOMETRY'
        category_info['concepts'].append('geometry')

    # Физика
    if any(kw in lower_code for kw in ['physics', 'collision', 'velocity', 'force']):
        category_info['category'] = 'PHYSICS'
        category_info['concepts'].append('physics')

    # Графика
    if any(kw in lower_code for kw in ['render', 'draw', 'sprite', 'texture', 'shader']):
        category_info['category'] = 'GRAPHICS'
        category_info['concepts'].append('graphics')

    # Игровая логика
    if any(kw in lower_code for kw in ['game', 'player', 'enemy', 'score', 'level']):
        category_info['concepts'].append('game_logic')

    # Сетка/Grid системы
    if any(kw in lower_code for kw in ['grid', 'cell', 'tile', 'board']):
        category_info['type'] = 'GRID_SYSTEM'
        category_info['concepts'].append('grid_generation')

    return category_info


def detect_technologies(code: str) -> Dict[str, object]:
    """Распознает используемые технологии в тексте и возвращает словарь с evidence.
    Возвращает: {
        'render_backends': ['OpenGL','Vulkan','DirectX'],
        'render_evidence': [{'match':'glfwInit','snippet':'...','line':123, 'category':'OpenGL'}],
        'evidence': [ ...all categories... ],
        'audio': ['OpenAL','FMOD'],
        'physics': ['Bullet','PhysX'],
        'network': ['Boost.Asio'],
        'render_init': ['glfwInit','glewInit']
    }
    """
    tech = {
        'render_backends': [],
        'render_evidence': [],
        'evidence': [],
        'audio': [],
        'physics': [],
        'network': [],
        'ecs': [],
        'math': [],
        'sdl_gl': [],
        'render_init': []
    }

    # Helper to record evidence
    def _add_evidence(kind_list, name, pattern, flags=0, evidence_bucket='evidence'):
        for m in re.finditer(pattern, code, flags):
            if name not in kind_list:
                kind_list.append(name)
            # compute line number
            line_no = code.count('\n', 0, m.start()) + 1
            # capture small snippet (current line and neighbors)
            start = max(0, m.start() - 80)
            end = min(len(code), m.end() + 80)
            snippet = code[start:end].replace('\n', '\\n')
            entry = {'match': m.group(0), 'snippet': snippet, 'pos': m.start(), 'line': line_no, 'category': name}
            tech[evidence_bucket].append(entry)

    # OpenGL: функции и макросы
    _add_evidence(tech['render_backends'], 'OpenGL',
                  r"\b(glEnable|glCreateShader|glShaderSource|glCompileShader|glUseProgram|glGetShaderiv|glGetShaderInfoLog|glGetProgramiv|glGetProgramInfoLog|glUniform\w*|glBindBuffer|glBufferData|glVertexAttribPointer|glDrawElements|glDrawArrays|glClearColor|GL_)",
                  flags=re.IGNORECASE)
    # include GL headers
    for m in re.finditer(r"#include\s*[<\"](GL/[^>\"]+)[>\"]", code, flags=re.IGNORECASE):
        if 'OpenGL' not in tech['render_backends']:
            tech['render_backends'].append('OpenGL')
        line_no = code.count('\n', 0, m.start()) + 1
        snippet = code[max(0, m.start() - 80):min(len(code), m.end() + 80)].replace('\n', '\\n')
        tech['render_evidence'].append(
            {'match': m.group(0), 'snippet': snippet, 'pos': m.start(), 'line': line_no, 'category': 'Include'})

    # Vulkan
    _add_evidence(tech['render_backends'], 'Vulkan', r"\b(vkCreateInstance|vkCreateDevice|#include\s*<vulkan|VK_)",
                  flags=re.IGNORECASE)
    # DirectX
    _add_evidence(tech['render_backends'], 'DirectX',
                  r"\b(D3D12CreateDevice|D3D11CreateDevice|#include\s*<d3d|ID3D12Device|ID3D11Device)",
                  flags=re.IGNORECASE)

    # Windowing / context / input libs
    _add_evidence(tech['sdl_gl'], 'SDL', r"\b(SDL_Init|SDL_CreateWindow|SDL_GL_CreateContext|#include\s*<SDL2/)",
                  flags=re.IGNORECASE)
    _add_evidence(tech['render_backends'], 'GLFW',
                  r"\b(glfwInit|glfwCreateWindow|#include\s*<GLFW/|#include\s*<glfw3.h>)", flags=re.IGNORECASE)

    # Math / GLM
    _add_evidence(tech['math'], 'GLM', r"\b(glm::|#include\s*<glm/)", flags=re.IGNORECASE)

    # Audio
    _add_evidence(tech['audio'], 'OpenAL', r"\b(openal|alcOpenDevice|ALCdevice|alGenSources|#include\s*<AL/)",
                  flags=re.IGNORECASE)
    _add_evidence(tech['audio'], 'FMOD', r"\b(fmod|FMOD_System_Create|FMOD_)", flags=re.IGNORECASE)

    # Physics
    _add_evidence(tech['physics'], 'Bullet', r"\b(btRigidBody|btCollision|Bullet)")
    _add_evidence(tech['physics'], 'PhysX', r"\b(PxPhysics|PhysX|PxCreateFoundation)")

    # ECS / Component systems
    _add_evidence(tech['ecs'], 'EnTT', r"\b(entt::|#include\s*<entt/)", flags=re.IGNORECASE)
    _add_evidence(tech['ecs'], 'CustomECS', r"\b(Entity|Component|System|Scene|ResourceManager|EventSystem)\b",
                  flags=re.IGNORECASE)

    # Networking
    _add_evidence(tech['network'], 'Boost.Asio', r"\b(boost::asio|<boost/asio>)")
    _add_evidence(tech['network'], 'AsioStandAlone', r"\b(asio::|#include\s*<asio/)")

    # Serialization / JSON / archive
    _add_evidence(tech['evidence'], 'nlohmann_json', r"\b(nlohmann::json|#include\s*<nlohmann/json\.hpp>)",
                  flags=re.IGNORECASE)
    _add_evidence(tech['evidence'], 'rapidjson', r"\b(rapidjson::|#include\s*<rapidjson/)", flags=re.IGNORECASE)

    # Init functions explicitly
    init_patterns = [(r"\bglfwInit\s*\(", 'glfwInit'), (r"\bglewInit\s*\(", 'glewInit'),
                     (r"\bgladLoadGL\s*\(", 'gladLoadGL'), (r"\bSDL_GL_CreateContext\s*\(", 'SDL_GL_CreateContext')]
    for patt, name in init_patterns:
        for m in re.finditer(patt, code):
            line_no = code.count('\n', 0, m.start()) + 1
            snippet = code[max(0, m.start() - 80):min(len(code), m.end() + 80)].replace('\n', '\\n')
            tech['render_init'].append(name)
            tech['render_evidence'].append(
                {'match': m.group(0), 'snippet': snippet, 'pos': m.start(), 'line': line_no, 'category': 'Init'})

    # Deduplicate lists
    def _dedup_list(lst):
        seen = set()
        out = []
        for e in lst:
            key = e if isinstance(e, str) else (e.get('match'), e.get('pos'))
            if key not in seen:
                seen.add(key)
                out.append(e)
        return out

    for k in ('render_backends', 'audio', 'physics', 'network', 'ecs', 'math'):
        tech[k] = list(dict.fromkeys(tech.get(k, [])))

    tech['render_evidence'] = _dedup_list(tech['render_evidence'])
    tech['evidence'] = _dedup_list(tech['evidence'])

    return tech


def make_entry(path: Path) -> Optional[Dict]:
    """Создание записи датасета из файла"""
    try:
        raw = path.read_bytes()
    except Exception as e:
        if VERBOSE:
            print(f'Ошибка чтения {path}: {e}', file=sys.stderr)
        return None

    try:
        text = raw.decode('utf-8', errors='replace')
    except Exception:
        text = ''

    if not text.strip():
        return None

    lines = text.splitlines()
    excerpt = '\n'.join(lines[:EXCERPT_LINES])

    # Вычисление относительного пути
    try:
        path_rel = str(path.relative_to(REPO))
    except ValueError:
        path_rel = os.path.relpath(str(path), start=str(REPO))
    path_rel = path_rel.replace('\\', '/')

    language = path.suffix.lower().lstrip('.')
    category_info = categorize_code(text, path.name)

    # Извлечение функций и API методов
    functions = extract_functions(text, language)
    api_methods = extract_api_methods(text)
    jsdoc_comments = extract_jsdoc_comments(text) if language in ['js', 'jsx', 'ts', 'tsx'] else []
    includes = extract_includes(text)

    # Detect technologies and render backends
    tech = detect_technologies(text)

    # Extract classes/structs/enums/macros/namespaces for architecture signals
    stripped = _strip_comments(text)
    classes = [m.group(1) for m in re.finditer(r"\bclass\s+([A-Za-z_]\w*)", stripped)]
    structs = [m.group(1) for m in re.finditer(r"\bstruct\s+([A-Za-z_]\w*)", stripped)]
    enums = [m.group(1) for m in re.finditer(r"\benum(?:\s+class)?\s+([A-Za-z_]\w*)", stripped)]
    defines = [m.group(1) for m in re.finditer(r"^\s*#define\s+([A-Za-z_]\w*)", stripped, flags=re.MULTILINE)]
    namespaces = [m.group(1) for m in re.finditer(r"\bnamespace\s+([A-Za-z_]\w*)", stripped)]

    # Extract constexpr/static const names (constants)
    constants = []
    for m in re.finditer(r"\bconstexpr\s+(?:[\w:<>]+\s+)+([A-Za-z_]\w*)\s*=", stripped):
        constants.append(m.group(1))
    for m in re.finditer(r"\bstatic\s+const\s+(?:[\w:<>]+\s+)+([A-Za-z_]\w*)\s*=", stripped):
        constants.append(m.group(1))

    # remove duplicates while preserving order
    def _uniq(seq):
        seen = set()
        out = []
        for x in seq:
            if x not in seen:
                seen.add(x)
                out.append(x)
        return out

    classes = _uniq(classes)
    structs = _uniq(structs)
    enums = _uniq(enums)
    defines = _uniq(defines)
    namespaces = _uniq(namespaces)
    constants = _uniq(constants)

    arch_tags = []
    if classes or structs:
        arch_tags.append('OOP')
    if tech.get('ecs'):
        arch_tags.append('ECS')
    if 'ResourceManager' in text:
        arch_tags.append('ResourceManager')
    if 'EventSystem' in text or 'EventDispatcher' in text:
        arch_tags.append('EventSystem')
    if 'Scene' in text and 'GameObject' in text:
        arch_tags.append('SceneGraph')

    # Determine primary render backend (priority: OpenGL, Vulkan, DirectX)
    render_primary = None
    for candidate in ('OpenGL', 'Vulkan', 'DirectX'):
        if candidate in tech.get('render_backends', []):
            render_primary = candidate
            break
    # If no backend from includes, but render_init functions indicate OpenGL, set accordingly
    if not render_primary and tech.get('render_init'):
        init_map = {'glfwInit': 'OpenGL', 'glewInit': 'OpenGL', 'gladLoadGL': 'OpenGL',
                    'SDL_GL_CreateContext': 'OpenGL'}
        for init_fn in tech.get('render_init'):
            if init_fn in init_map:
                render_primary = init_map[init_fn]
                break

    # mark key file heuristics: explicit KEY_FILES or any tech evidence (render/audio/physics/network/ecs) or special names
    is_key = path_rel in KEY_FILES
    if not is_key:
        if FORCE_KEY_FROM_EVIDENCE:
            # more aggressive: mark if any evidence exists
            if tech.get('render_backends') or tech.get('audio') or tech.get('physics') or tech.get(
                    'network') or tech.get('ecs') or tech.get('math'):
                is_key = True
        else:
            # default heuristics
            if tech.get('render_backends') or tech.get('audio') or tech.get('physics') or tech.get(
                    'network') or tech.get('ecs'):
                is_key = True
            elif path.name.lower() in ('cmakelists.txt', 'engineconfig.h', 'version.h', 'engineconfig.h.template'):
                is_key = True

    # collect cmake options (if CMakeLists)
    cmake_opts = []
    if 'cmake_minimum_required' in text.lower() or 'project(' in text.lower():
        for m in re.finditer(r"-D(\w+)=(\S+)", text):
            cmake_opts.append((m.group(1), m.group(2)))

    # collect dependency files (includes that look like external libs)
    dependency_files = [inc for inc in includes if any(s in inc.lower() for s in
                                                       ('boost', 'glm', 'glfw', 'glew', 'vulkan', 'fmod', 'physx',
                                                        'bullet', 'entt', 'nlohmann', 'rapidjson', 'sdl'))]

    # Создание метаданных
    meta = {
        'id': f'{language.upper()}-{sha256_text(text)[:8]}',
        'path': str(path),
        'path_relative': path_rel,
        'filename': path.name,
        'size_bytes': len(raw),
        'lines': len(lines),
        'mtime': path.stat().st_mtime,
        'language': language,
        'excerpt': excerpt,
        'top_comment': top_comment(text),
        'sha256': sha256_text(text),
        'tokens': estimate_tokens(text),
        'type': category_info['type'],
        'category': category_info['category'],
        'concepts': category_info['concepts'],
        'functions_count': len(functions),
        'api_methods_count': len(api_methods),
        'includes': includes,
        'includes_count': len(includes),
        'technologies': {
            'render_backends': tech.get('render_backends', []),
            'audio': tech.get('audio', []),
            'physics': tech.get('physics', []),
            'network': tech.get('network', []),
            'ecs': tech.get('ecs', []),
            'math': tech.get('math', [])
        },
        'render_evidence': tech.get('render_evidence', []),
        'render_primary': render_primary,
        'architecture_tags': arch_tags,
        'classes': classes,
        'structs': structs,
        'enums': enums,
        'defines': defines,
        'namespaces': namespaces,
        'constants': constants,
        'cmake_options': cmake_opts,
        'dependency_files': dependency_files,
        'is_key_file': is_key
    }

    # If key file, include truncated full text to ensure model sees full context
    if is_key:
        meta['full_text'] = text[:20000]

    # Создание синтетических Q&A пар
    summary = meta['top_comment'] or (excerpt.splitlines()[0] if excerpt.splitlines() else '')

    messages = [
        {'role': 'system',
         'content': 'Ты опытный помощник-программист, специализирующийся на разработке игровых движков. Отвечай конкретно и по делу на русском языке.'},
        {'role': 'system', 'content': f'Контекст: файл {path.name} из проекта игрового движка.'},
        {'role': 'system', 'content': (excerpt if len(excerpt) < 4000 else excerpt[:4000])}
    ]

    # Базовый вопрос о файле
    messages.append({'role': 'user', 'content': f'Что делает файл {path.name}?'})
    messages.append({'role': 'assistant', 'content': summary or 'Исходный код компонента игрового движка.'})

    # Вопросы об API методах
    if INCLUDE_API_DOCS and api_methods:
        unique_entities = list(set(m['entity'] for m in api_methods))[:3]
        for entity in unique_entities:
            messages.append({'role': 'user', 'content': f'Как использовать {entity}?'})
            methods_info = [m for m in api_methods if m['entity'] == entity]
            if methods_info:
                example = methods_info[0]['full_call']
                messages.append({'role': 'assistant', 'content': f'Метод {entity} используется так: {example}'})

    # Вопросы о функциях
    if INCLUDE_EXAMPLES and functions:
        func = functions[0]
        messages.append({'role': 'user', 'content': f'Покажи пример использования функции {func["name"]}'})
        messages.append({'role': 'assistant', 'content': f'```{language}\n{func["code"]}\n```'})

    # Добавляем краткую сводку технологий в системные сообщения — поможет при векторизации
    tech_summary = []
    if meta.get('render_primary'):
        tech_summary.append(f"render_primary={meta['render_primary']}")
    if meta.get('technologies'):
        rb = meta['technologies'].get('render_backends', [])
        if rb:
            tech_summary.append('render_backends=' + ','.join(rb))
        audio = meta['technologies'].get('audio', [])
        if audio:
            tech_summary.append('audio=' + ','.join(audio))
        phys = meta['technologies'].get('physics', [])
        if phys:
            tech_summary.append('physics=' + ','.join(phys))
        net = meta['technologies'].get('network', [])
        if net:
            tech_summary.append('network=' + ','.join(net))
    if tech_summary:
        messages.insert(0, {'role': 'system', 'content': 'TECH_SUMMARY: ' + '; '.join(tech_summary)})

    # Проверка на превышение токенов
    total_tokens = sum(estimate_tokens(json.dumps(m)) for m in messages)
    if total_tokens > MAX_TOKENS:
        # Обрезаем excerpt
        meta['excerpt'] = meta['excerpt'][:2000]
        messages[2]['content'] = meta['excerpt']

    return {
        'id': meta['id'],
        'title': f'{path.name} - {category_info["category"]}',
        'text': excerpt,
        'metadata': meta,
        'messages': messages,
        'tokens': estimate_tokens(json.dumps(messages))
    }


# Добавим набор специальных имён файлов, которые следует включать даже без расширения
SPECIAL_FILES = {"cmakelists.txt", "makefile", "dockerfile", "readme", "readme.md"}
# Ключевые файлы, которые следует пометить как важные и включить полный текст
KEY_FILES = {
    'Engine/Source/Core/Engine.cpp',
    'Engine/CMakeLists.txt',
    'App/Source/main.cpp',
    'Engine/Source/io/FileSystem.cpp',
    'Engine/Source/Core/graphics/GraphicsAPI.cpp',
    'Engine/Source/Core/graphics/ShaderProgram.cpp',
    'Engine/Source/Core/render/Mesh.cpp',
    'Engine/Source/Core/render/VertexLayout.hpp'
}


def walk_and_collect(root: Path) -> List[Dict]:
    """Рекурсивный обход и сбор файлов"""
    entries = []

    for dirpath, dirnames, filenames in os.walk(root):
        # Исключение директорий
        dirnames[:] = [d for d in dirnames if d.lower() not in EXCLUDE]

        for fn in filenames:
            p = Path(dirpath) / fn

            if is_binary_by_ext(p):
                continue

            fname_lower = fn.lower()
            # Включаем файл если его расширение в списке или имя в SPECIAL_FILES
            if not (fname_lower.endswith(EXTS) or fname_lower in SPECIAL_FILES):
                continue

            entry = make_entry(p)
            if entry:
                entries.append(entry)
                if VERBOSE:
                    print(f'✓ Обработан: {entry["metadata"]["path_relative"]} ({entry["tokens"]} токенов)')

    return entries


def main():
    """Главная функция"""
    print(f'🚀 Начало сбора датасета из {ROOT}')
    print(f'📁 Расширения: {", ".join(EXTS)}')
    print(f'🚫 Исключения: {", ".join(EXCLUDE)}')
    print(f'📊 Макс. токенов на запись: {MAX_TOKENS}')
    print()

    OUT.parent.mkdir(parents=True, exist_ok=True)

    if STREAM:
        fh = OUT.open('a', encoding='utf-8')
        count = 0

        for entry in walk_and_collect(ROOT):
            # Запись в одну строку без переносов (JSONL формат)
            fh.write(json.dumps(entry, ensure_ascii=False, separators=(',', ':')) + '\n')
            count += 1

        fh.close()
        print(f'\n✅ Записано {count} записей в {OUT}')
    else:
        entries = walk_and_collect(ROOT)

        with OUT.open('w', encoding='utf-8') as fh:
            for e in entries:
                # Запись в одну строку без переносов (JSONL формат)
                fh.write(json.dumps(e, ensure_ascii=False, separators=(',', ':')) + '\n')

        total_tokens = sum(e['tokens'] for e in entries)
        print(f'\n✅ Записано {len(entries)} записей в {OUT}')
        print(f'📊 Всего токенов: {total_tokens:,}')
        print(f'📈 Средний размер записи: {total_tokens // len(entries) if entries else 0} токенов')


if __name__ == '__main__':
    main()
