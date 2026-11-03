# Spider Hero - First Demo

C++17 + OpenGL 3.3 Core, GLFW, GLAD. Procedural 2D shapes with modern GLSL shaders.

## Windows + VS Code (recommended: vcpkg)
1. Install Visual Studio Build Tools with Desktop development with C++, VS Code, CMake and Git.
2. Clone vcpkg, bootstrap it, then run: `vcpkg install glfw3 glad:x64-windows`
3. Open this folder in VS Code terminal.
4. Configure (replace VCPKG_ROOT with your vcpkg path):
   `cmake -S . -B build -DCMAKE_TOOLCHAIN_FILE="C:/vcpkg/scripts/buildsystems/vcpkg.cmake" -A x64`
   If using Ninja rather than Visual Studio generator, remove `-A x64` and use the appropriate triplet.
5. Build: `cmake --build build --config Release`
6. Run: `./build/Release/SpiderHero.exe` (Visual Studio generator) or `./build/SpiderHero.exe` (single-config generator).

## Controls
Space: pause/resume. R: restart. Esc: exit.

## Demo sequence
0-6s: swinging on web; 6-9s: descent/landing; 9-16s: waving on rooftop. Loops.

## Limitations
This is a fast initial prototype. The hero is drawn procedurally, not yet with final PNG sprite artwork. No sound or multiple scenes yet. Lighting is a static gradient sky, not a full dynamic lighting technique; that requirement remains for the final project.
