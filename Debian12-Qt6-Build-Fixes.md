# Debian 12 Qt 6 Build Fixes

This branch documents local build fixes needed to compile VNote on Debian 12 / MX Linux 23 with Qt 6.4.

## QCheckBox signal compatibility

File:

- `src/widgets/findandreplacewidget2.cpp`

Problem:

Qt 6.4 does not provide `QCheckBox::checkStateChanged`.

Fix:

Use the older compatible signal:

```cpp
QCheckBox::stateChanged
```

## Complete Event type for Qt metatype checks

Files:

- `src/views/combinednodeexplorer.cpp`
- `src/views/twocolumnsnodeexplorer.cpp`

Problem:

Qt 6.4 requires `vnotex::Event` to be fully defined when connecting signals using `QSharedPointer<Event>`.

Fix:

Add:

```cpp
#include <core/events.h>
```

## qwindowkit Wayland compatibility

Submodule:

- `libs/qwindowkit`

Files:

- `src/core/qwindowkit_linux.h`
- `src/core/contexts/linuxwaylandcontext.cpp`

Problem:

Debian 12 / Qt 6.4 does not provide `QNativeInterface::QWaylandApplication`, and `wl_display` was not visible as a global forward declaration.

Fixes:

- Add global forward declaration:

```cpp
struct wl_display;
```

- Guard Wayland native interface code for Qt 6.5 or newer:

```cpp
#if QT_VERSION >= QT_VERSION_CHECK(6, 5, 0)
...
#else
Q_UNUSED(data);
#endif
```

## Verification

The following command completed successfully:

```bash
cmake --build build --target vnote
```



```

Then add only the Debian 12 fixes:

```bash
git add Debian12-Qt6-Build-Fixes.md
git add src/views/combinednodeexplorer.cpp
git add src/views/twocolumnsnodeexplorer.cpp
git add src/widgets/findandreplacewidget2.cpp
```

Important: `libs/qwindowkit` is a submodule. If you want to include those fixes, you must commit inside the submodule first:

```bash
cd libs/qwindowkit
git status
git add src/core/qwindowkit_linux.h src/core/contexts/linuxwaylandcontext.cpp
git commit -m "Fix Qt 6.4 Wayland build compatibility"
cd ../..
git add libs/qwindowkit
```

Then commit the branch:

```bash
git commit -m "Fix Debian 12 Qt 6.4 build issues"
```

Do **not** add these to this Debian 12 branch unless you also want translation work mixed in:

```bash
Roadmap.md
src/core/coreconfig.cpp
src/data/core/translations/vnote_es_ES.qm
vnotex.log
```

Check before committing:

```bash
git status --short
```

For a clean Debian 12 fixes commit, staged files should be only:

```text
Debian12-Qt6-Build-Fixes.md
src/views/combinednodeexplorer.cpp
src/views/twocolumnsnodeexplorer.cpp
src/widgets/findandreplacewidget2.cpp
libs/qwindowkit
```

Then later, to return to your normal branch:

```bash
git switch master
```

If Git refuses because `Roadmap.md` or `src/core/coreconfig.cpp` has uncommitted translation work, either commit those separately on another branch or stash them first.