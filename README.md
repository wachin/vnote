# VNote
![CI-Windows](https://github.com/vnotex/vnote/actions/workflows/ci-win.yml/badge.svg?branch=master) ![CI-Linux](https://github.com/vnotex/vnote/actions/workflows/ci-linux.yml/badge.svg?branch=master) ![CI-MacOS](https://github.com/vnotex/vnote/actions/workflows/ci-macos.yml/badge.svg?branch=master)

[简体中文](README_zh_CN.md)

> **Note about this fork**: this is an independent fork ([wachin/vnote](https://github.com/wachin/vnote)) on the `debian12-qt6-build-fixes` branch, which builds with the system Qt 6.4 on Debian 12 / MX Linux 23. It adds a complete **Spanish translation** and support for **running multiple instances** (one per workspace). See [Debian12-Branch-Guide.md](Debian12-Branch-Guide.md) for the branch workflow.

[Project on Gitee](https://gitee.com/vnotex/vnote)

A pleasant note-taking platform.

For more information, please visit [**VNote's Home Page**](https://vnotex.github.io/vnote).

![VNote](pics/vnote.png)

## Description
**VNote** is a Qt-based, free and open source note-taking application, focusing on Markdown now. VNote is designed to provide a pleasant note-taking platform with excellent editing experience.

VNote is **NOT** just a simple editor for Markdown. By providing notes management, VNote makes taking notes in Markdown simpler. In the future, VNote will support more formats besides Markdown.

Utilizing Qt, VNote could run on **Linux**, **Windows**, and **macOS**.

![Main](pics/main.png)

![Main2](pics/main2.png)

## Downloads
Continuous builds on `master` branch could be found at the [Continuous Build](https://github.com/vnotex/vnote/releases/tag/continuous-build) release.

Latest stable builds could be found at the [latest release](https://github.com/vnotex/vnote/releases/latest). Alternative download services are available:

* [Tianyi Netdisk](https://cloud.189.cn/t/Av67NvmEJVBv)
* [Baidu Netdisk](https://pan.baidu.com/s/1lX69oMBw8XuJshQDN3HiHw?pwd=f8fk)

## Supports
* [GitHub Issues](https://github.com/vnotex/vnote/issues);
* Email: `tamlokveer at gmail.com`;
* [Telegram](https://t.me/vnotex);
* WeChat Public Account: vnotex;

Thank [users who donated to VNote](https://github.com/vnotex/vnote/wiki/Donate-List)!

## Development

After cloning the repository, run the initialization script to set up your development environment:

**Linux/macOS:**
```bash
bash scripts/init.sh
```

**Windows:**
```cmd
scripts\init.cmd
```

This script will:
* Initialize and update all git submodules
* Install pre-commit hooks for automatic code formatting with clang-format
* Set up the vtextedit submodule pre-commit hook

If you cloned without the script, initialize the submodules manually:

```bash
git submodule sync --recursive
git submodule update --init --recursive
```

For more development guidelines, see [AGENTS.md](AGENTS.md).

### Building on Debian 12 / Debian 13 (system Qt 6)

This fork builds against the **system Qt** packages — no Qt online installer is needed. Verified with Qt 6.4 (Debian 12 / MX Linux 23) and Qt 6.8.2 (Debian 13 trixie).

#### Required packages

```bash
sudo apt-get install -y \
    build-essential cmake ninja-build git clang-format \
    qt6-base-dev qt6-base-dev-tools qt6-base-private-dev qt6-svg-dev \
    qt6-webchannel-dev qt6-webengine-dev qt6-5compat-dev \
    qt6-tools-dev qt6-l10n-tools linguist-qt6 \
    extra-cmake-modules libxkbcommon-dev libxcb-cursor-dev \
    libxcb-xkb-dev libx11-dev libxcb1-dev
```

What each group is for:

| Packages | Purpose |
|----------|---------|
| `build-essential`, `cmake` (≥ 3.20), `ninja-build`, `git`, `clang-format` | Toolchain |
| `qt6-base-dev` (+ tools) | Qt Core/Gui/Widgets/Network/Sql/PrintSupport |
| `qt6-base-private-dev` | Qt private headers — required by qwindowkit (`Qt6::CorePrivate`, `Qt6::GuiPrivate`) |
| `qt6-svg-dev` | SVG icons support |
| `qt6-webchannel-dev`, `qt6-webengine-dev` | Markdown preview (WebEngine) and WebChannel IPC |
| `qt6-5compat-dev` | Qt5 compatibility layer (optional component used by the project) |
| `qt6-l10n-tools`, `linguist-qt6` | `lrelease`, `lupdate`, Qt Linguist — required for translations |
| `extra-cmake-modules`, `libxkbcommon-dev`, `libxcb-cursor-dev`, `libxcb-xkb-dev`, `libx11-dev`, `libxcb1-dev` | Platform integration (qwindowkit window decorations, keyboard handling) |

Notes:

* The branch contains fixes originally needed to compile with **Qt 6.4**
  (Debian 12): `QCheckBox::stateChanged` instead of `checkStateChanged`,
  full `Event` type includes, and a Wayland guard in qwindowkit. They remain
  compatible with newer Qt versions. See
  [Debian12-Qt6-Build-Fixes.md](Debian12-Qt6-Build-Fixes.md).
* If you switch Qt versions (e.g. after a distro upgrade), delete the build
  directory and reconfigure from scratch, or CMake may keep stale paths:
  `rm -rf build && mkdir build && cd build && cmake .. -DCMAKE_BUILD_TYPE=Release`.
* The `qt6-webengine-dev` package pulls in `libqt6webenginecore6` etc.
  automatically.

#### Build

```bash
mkdir -p build
cd build
cmake .. -G Ninja -DCMAKE_BUILD_TYPE=Release
ninja vnote
ninja lrelease   # generate .qm translation files
```

Or with plain Makefiles:

```bash
mkdir -p build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
cmake --build . --target vnote -- -j$(nproc)
cmake --build . --target lrelease
```

Run it:

```bash
./src/vnote     # from the build directory
```

#### Interface language (Spanish)

The translation files live in `src/data/core/translations/`. After building,
enable it in `Settings → General → Language → español de España` and restart.
The list of translation-related commands is documented in
[Debian12-Branch-Guide.md](Debian12-Branch-Guide.md).

#### Multiple instances

By default VNote allows only one instance (a second launch forwards the files
to the running one). This fork adds two opt-in ways to run several instances
(useful on Linux workspaces):

* `Settings → General → Allow multiple instances` — persistent preference,
  takes effect after restart.
* `vnote --new-instance` — start one additional instance, regardless of the
  setting above.

Caveats: instances share the same session/config, so the last one to close
wins the session state; avoid editing the same note in two instances at the
same time.

## License
VNote is licensed under [GNU LGPLv3](https://opensource.org/licenses/LGPL-3.0). Code base of VNote could be used freely by VNoteX.
