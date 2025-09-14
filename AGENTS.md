AGENTS: how to build, test, and code in this repo

- Project type: openFrameworks C++ addon with Xcode/Makefile examples. No first-party tests exist (tests/.gitkeep only).

Build
- macOS (Xcode GUI): open example_MotionFromVideo/example_MotionFromVideo.xcodeproj or example_VideoMotionIntoFluid/example_VideoMotionIntoFluid.xcodeproj, select a scheme, Build & Run.
- macOS/Linux (Make):
  - cd example_MotionFromVideo && make -j
  - cd example_VideoMotionIntoFluid && make -j
- CI references: .travis.yml (Linux/macOS) and .appveyor.yml (Windows) rely on openFrameworks scripts/ci/addons/build.*; mirror those if scripting CI locally.

Lint/format
- Use clang-format (LLVM style) for C/C++ sources. Keep includes ordered: C system, C++ system, third-party, project, alphabetized within groups. Wrap at ~100 cols.
- Prefer -Wall -Wextra -Werror locally; fix warnings at source. Avoid unused-includes.

Tests
- No unit tests provided. For manual testing, run the example apps and verify optical-flow output. If adding tests, use Catch2 or GoogleTest under tests/ and wire with a Makefile target like make test. To run a single test (suggested):
  - ./tests/bin/Tests [TestNamePattern]

Code style
- Types: use explicit widths where relevant (uint8_t, float for GLSL interop). Prefer const&, span-like views to avoid copies. Avoid raw new/delete.
- Naming: snake_case for files, lowerCamelCase for variables/functions, UpperCamelCase for classes/structs, ALL_CAPS for macros/const globals.
- Imports/includes: include only what you use; forward-declare where possible in headers. Use #pragma once.
- Error handling: fail fast on GL/OF setup errors; guard FBO/texture allocation; check shader compile/link logs and log via ofLogError with actionable messages.
- Threading: openFrameworks is largely single-threaded for GL; confine GL calls to the render thread; protect shared state if adding worker threads.

Other conventions
- Target oF 0.12+ (see README). Add dependency ofxRenderer (addon_config.mk) when creating new examples.
- No Cursor or Copilot rules present in this repo at time of writing.
