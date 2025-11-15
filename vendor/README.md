This folder is intended to contain vendored third-party libraries used by the project when building on Windows.

Expected layout (optional — the build will work without vendor/ but App will not link vendor libs):

vendor/
  ├─ glfw-3.4/
  │   └─ CMakeLists.txt (GLFW sources)
  │   └─ include/
  │   └─ src/
  └─ glew/
      ├─ include/
      └─ build/
          └─ cmake/
              └─ CMakeLists.txt (GLEW CMake scripts)

Notes:
- By default the project will use FetchContent to download GLFW from GitHub into `vendor/` (controlled by the CMake option `VENDOR_GLFW_FROM_GIT` and the tag `VENDOR_GLFW_GIT_TAG`). This makes it easier to pin and update GLFW versions.
- If you prefer to keep a local copy, set `VENDOR_GLFW_FROM_GIT` to OFF and place the GLFW sources under `vendor/glfw-3.4/`.
- If present, the top-level CMake will attempt to add_subdirectory() for these libraries and link them into the app target.
- The project is intentionally restricted to Windows builds only. If you need multi-platform support, remove the WIN32 check in the top-level CMakeLists.txt and adapt vendor paths accordingly.
