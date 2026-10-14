# C Coding Standards and Project Rules

This document defines the rules for writing, reviewing, and modifying C code
in the OpenLook repository. The project is a Linux port of a historical XView
and OPEN LOOK codebase, so compatibility and focused modernization are both
important.

## 1. Language and supported environment

- The project is written in C, not C++.
- New code must be valid C99. The top-level CMake project sets
  `CMAKE_C_STANDARD` to 99.
- GCC is the primary compiler. A supported Clang build is useful as a
  secondary portability check, but it does not replace the GCC build.
- Linux with X11 is the primary target environment.
- CMake 3.16 or newer is the supported build system. Do not add a parallel
  nested-Makefile build system.
- The project does not require a Z80 cross-compiler or a project-specific
  Docker image.

Required development dependencies are documented in `README.md`. They
currently include X11, Xext, and libtirpc development files.

## 2. Project directory structure

Do not create source files outside the appropriate project directories. Do
not leave editor backups, stray object files, core files, logs, or temporary
files in the source tree.

- `apps/<name>/` contains standalone application sources such as `clock`,
  `props`, and `textedit`.
- `src/olwm/` contains the Open Look window manager sources.
- `lib/olgx/` and `lib/libxview/` contain the library implementations.
- `include/olgx/`, `include/pixrect/`, `include/xview/`, and
  `include/xview_private/` contain exported headers.
- Private headers should remain next to their implementation inside
  `apps/`, `src/`, or `lib/`.
- `resources/` contains fonts, bitmaps, images, and other packaged assets.
- `config/` contains menus, application defaults, and similar runtime
  configuration.
- `docs/man/` contains manual-page sources.
- `docs/standards/` contains project standards.
- `tests/` contains automated tests when they are added; test data belongs
  in `tests/data/`.
- `build/` contains CMake state, object files, dependency files, and other
  intermediate build products.
- `bin/` is the generated OPENWINHOME staging tree. CMake places final
  executables, libraries, public headers, manual pages, fonts, and runtime
  resources there.

Do not create nested build directories under `apps/`, `src/`, `lib/`,
`resources/`, or `tests/`.

## 3. Compatibility and naming

The repository contains public APIs and historical implementation names that
predate modern C naming conventions. Names such as `XView`, `ExitOLWM`, and
the existing public types and macros are compatibility interfaces.

- Preserve existing exported symbols, resource names, command-line options,
  structure layouts, and header paths unless an intentional compatibility
  change has been approved.
- Do not mechanically rename legacy functions, types, files, or fields just
  to enforce a new naming convention.
- Use lowercase snake_case for new private functions and variables unless the
  surrounding module has a strong established convention that improves
  consistency.
- Use uppercase names for macros and true compile-time constants.
- Use lowercase names for new C source and header files. Documentation and
  top-level metadata may follow their established naming conventions.
- Keep X11, Xt, XView, and system-library names exactly as their APIs define
  them.

## 4. Headers and implementation files

- C implementation files use the `.c` suffix. Do not introduce `.cpp` files
  into the C build.
- Exported declarations belong in the appropriate header under `include/`.
- Declarations used only by an application or library implementation belong
  in a private header beside that implementation.
- New declarations must use complete C prototypes. Do not introduce implicit
  function declarations, implicit `int`, or unprototyped new functions.
- Include the header that declares a function in the file implementing that
  function whenever the existing module structure permits it.
- Keep function bodies out of headers except for small, clearly justified
  `static inline` helpers.
- Use forward declarations to avoid unnecessary include dependencies, but do
  not duplicate incompatible declarations in multiple headers.

When safely modifying old K&R-style definitions, modernize their prototypes
if doing so does not change callback compatibility or the public ABI. Avoid
unrelated whole-file rewrites.

## 5. File headers and legal notices

Preserve existing Sun Microsystems, XView, OPEN LOOK, and contributor notices
in imported files. Do not replace them with an invented GPL notice or claim
copyright ownership that the repository does not establish.

New files must begin with a concise comment that explains the file's purpose
and important implementation constraints. Add a legal notice only when the
project's applicable notice is known. For example:

```c
/*
 * signal_queue.c: signal-safe notification support for the event loop.
 *
 * Signal handlers write only to a non-blocking pipe. Cleanup and other
 * non-async-signal-safe work runs in normal event-loop context.
 */
```

Keep comment and source lines readable on an 80-column terminal where
practical.

## 6. Function documentation

Document every new public function immediately above its declaration. The
comment should explain:

- what the function does;
- the meaning and ownership of parameters;
- the return value;
- important side effects, limits, and error behavior.

Document private helpers when their purpose, lifetime rules, signal-safety,
or ownership rules are not obvious. Existing historical comments may retain
their style; improve inaccurate comments when changing the related code.

## 7. General coding rules

- Fix every compiler warning caused or exposed by a change. Do not hide a
  warning with a cast or broad warning suppression unless the conversion is
  intentional and documented.
- Prefer small, focused functions and narrow variable scope.
- Avoid adding global state. When legacy global state must be used, make
  ownership and lifetime clear.
- Check allocation, file, pipe, process, and X11 operation failures where a
  failure can be handled meaningfully.
- Use fixed-width or appropriately sized system types when truncation is
  possible. Do not convert pointers through `int`.
- Signal handlers may use only async-signal-safe operations. Defer Xlib,
  allocation, stdio, and cleanup work to normal process context.
- Do not change observable legacy behavior as a side effect of a warning or
  formatting cleanup.
- Use four spaces and no tabs in new code. When editing a legacy function,
  follow its local indentation unless the entire function is being
  deliberately reformatted.
- Put a function's opening brace on its own line, matching the established
  project style:

```c
static int
example_function(int value)
{
    return value + 1;
}
```

- Keep new source files reasonably focused. The approximate 500-line goal
  applies to new modules, not as a reason to split historical files during an
  unrelated fix.

## 8. Build and diagnostic rules

The normal out-of-source build is:

```bash
cmake -S . -B build
cmake --build build -j"$(nproc)"
```

The normal build must complete without compiler warnings or errors. Compiler
flags belong in the CMake configure step, not after `cmake --build`. When
checking legacy C compatibility, use the strict diagnostics supported by the
selected GCC version, for example:

```bash
cmake -S . -B build \
  -DCMAKE_C_FLAGS="-Werror=implicit-function-declaration \
  -Werror=implicit-int -Werror=return-mismatch \
  -Werror=incompatible-pointer-types -Werror=int-conversion"
cmake --build build -j"$(nproc)"
```

Use additional `-Wall`, `-Wextra`, and relevant targeted warnings while
developing new code. Treat diagnostics in changed code as defects. Do not add
target-wide warning suppressions merely to preserve an unsafe legacy
construct.

For memory-corruption, lifetime, and undefined-behavior work, configure a
Debug build with the sanitizers supported by the host compiler:

```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug \
  -DCMAKE_C_FLAGS="-g -O1 -fno-omit-frame-pointer \
  -fsanitize=address,undefined"
cmake --build build -j"$(nproc)"
```

Run sanitizer builds only in an environment where the X11 and runtime
dependencies are compatible with the selected sanitizer. Report any
external-library limitation rather than silently skipping the check.

Object files and CMake state must remain in `build/`. Final artifacts are
expected in `bin/` because the top-level CMake project intentionally stages a
complete local OPENWINHOME tree there.

## 9. Testing

- Run the most relevant automated tests after each change. If no automated
  test covers the behavior, perform and report a focused runtime check.
- Use Xephyr for nested window-manager and XView application tests so the
  developer's active X session is not replaced.
- Configure Xephyr with the packaged Open Look font directories when testing
  font-dependent programs.
- Verify normal client shutdown through the window-manager protocol. Test
  signal handling with the X server kept alive so server shutdown does not
  mask application bugs.
- For link changes, check both shared-library dependencies and unresolved
  relocations where appropriate.
- Do not leave Xephyr, window-manager, client, or helper processes running
  after a test.
- Do not commit generated build products, sanitizer output, logs, or core
  files.

## 10. Documentation

- Keep `README.md` consistent with supported dependencies, build commands,
  staging layout, installation, and runtime instructions.
- Store manual-page source files in `docs/man/`; CMake stages and installs
  them as section 1 manual pages.
- Store coding and repository standards in `docs/standards/`.
- Put other project notes in a clearly named subdirectory under `docs/` when
  a distinct category is useful.
- Update comments and documentation when behavior or paths change.
