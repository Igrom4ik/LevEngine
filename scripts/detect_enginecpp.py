import json
from pathlib import Path

p = Path('Engine/Source/Core/Engine.cpp')
if not p.exists():
    print(json.dumps({'error': 'file not found', 'path': str(p)}))
    raise SystemExit(1)

code = p.read_text(encoding='utf-8', errors='replace')

tech = {
    'render_backends': [],
    'render_evidence': [],
    'evidence': [],
    'audio': [],
    'physics': [],
    'network': [],
    'render_init': []
}

import re


def _add_evidence(kind_list, name, pattern, flags=0, evidence_bucket='evidence'):
    for m in re.finditer(pattern, code, flags):
        if name not in kind_list:
            kind_list.append(name)
        line_no = code.count('\n', 0, m.start()) + 1
        start = max(0, m.start() - 60)
        end = min(len(code), m.end() + 60)
        snippet = code[start:end].replace('\n', '\\n')
        entry = {'match': m.group(0), 'snippet': snippet, 'pos': m.start(), 'line': line_no, 'category': name}
        tech[evidence_bucket].append(entry)


# OpenGL
_add_evidence(tech['render_backends'], 'OpenGL',
              r"\b(gladLoadGL|glewInit|glewExperimental|glfwInit|glfwCreateWindow|SDL_GL_CreateContext|#include\s*<GL/|#include\s*\"GL/)")
# Vulkan
_add_evidence(tech['render_backends'], 'Vulkan', r"\b(vkCreateInstance|vkCreateDevice|#include\s*<vulkan|VK_)")
# DirectX
_add_evidence(tech['render_backends'], 'DirectX',
              r"\b(D3D12CreateDevice|D3D11CreateDevice|#include\s*<d3d12.h>|#include\s*<d3d11.h>|ID3D12Device|ID3D11Device)")

# Audio
_add_evidence(tech['audio'], 'OpenAL', r"\b(openal|alcOpenDevice|ALCdevice|alGenSources|#include\s*<AL/)",
              flags=re.IGNORECASE)
_add_evidence(tech['audio'], 'FMOD', r"\b(fmod|FMOD_System_Create)", flags=re.IGNORECASE)

# Physics
_add_evidence(tech['physics'], 'Bullet', r"\b(btRigidBody|btCollision|Bullet)")
_add_evidence(tech['physics'], 'PhysX', r"\b(PxPhysics|PhysX|PxCreateFoundation)")

# Network
_add_evidence(tech['network'], 'Boost.Asio', r"\b(boost::asio|<boost/asio>)")

# init patterns
init_patterns = [(r"\bglfwInit\s*\(", 'glfwInit'), (r"\bglewInit\s*\(", 'glewInit'),
                 (r"\bgladLoadGL\s*\(", 'gladLoadGL'), (r"\bSDL_GL_CreateContext\s*\(", 'SDL_GL_CreateContext')]
for patt, name in init_patterns:
    for m in re.finditer(patt, code):
        line_no = code.count('\n', 0, m.start()) + 1
        snippet = code[max(0, m.start() - 60):min(len(code), m.end() + 60)].replace('\n', '\\n')
        tech['render_init'].append(name)
        tech['render_evidence'].append(
            {'match': m.group(0), 'snippet': snippet, 'pos': m.start(), 'line': line_no, 'category': 'Init'})

# dedup
seen = set();
out = []
for e in tech['render_evidence']:
    key = (e.get('match'), e.get('pos'))
    if key not in seen:
        seen.add(key);
        out.append(e)
tech['render_evidence'] = out

# make render_backends unique
tech['render_backends'] = list(dict.fromkeys(tech['render_backends']))

# decide render_primary
render_primary = None
for candidate in ('OpenGL', 'Vulkan', 'DirectX'):
    if candidate in tech['render_backends']:
        render_primary = candidate;
        break
if not render_primary and tech['render_init']:
    init_map = {'glfwInit': 'OpenGL', 'glewInit': 'OpenGL', 'gladLoadGL': 'OpenGL', 'SDL_GL_CreateContext': 'OpenGL'}
    for fn in tech['render_init']:
        if fn in init_map:
            render_primary = init_map[fn];
            break

out = {'file': str(p), 'render_primary': render_primary, 'tech': tech}
print(json.dumps(out, ensure_ascii=False, indent=2))
