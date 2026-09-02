# HANDOFF: Multi-instancia + `--workspace-id` + drag-drop sobre pestañas

> **Documento para AGENTES (no humanos).** Estado del trabajo al 2026-09-02.
> Rama: `debian12-qt6-build-fixes` (fork personal de vnotex/vnote).
> Repo: `/home/wachin/Dev/VNote-Dev/vnote`. Build dir: `build/` (ya configurado, Unix Makefiles, Qt 6.4 / Debian 12).
>
> **Regla #1 (obligatoria):** Si el desarrollo requiere instalar un paquete, DETENTE y avísale al
> usuario el nombre exacto + comando (`sudo apt install <pkg>`). NUNCA instales nada por tu cuenta.
>
> **Regla #2:** Lee `AGENTS.md` (raíz) y `src/widgets/AGENTS.md` para convenciones MVC/DI antes de
> escribir código. Resumen crítico: MVC estricto, `ServiceLocator&` por constructor, prefijos
> `p_`/`m_`, C++14, 2 espacios, no tocar `libs/` (submódulos), commits estilo `feat(scope): ...`.

---

## 0. Mapa del documento

| Sección | Contenido |
|---|---|
| 1 | Contexto: qué es este fork y qué motiva el trabajo |
| 2 | Trabajo YA terminado (commits) — NO rehacer |
| 3 | Estado EXACTO del build verificado el 2026-09-02 (qué compila, qué no) |
| 4 | Tareas P0–P5 en orden de ejecución, con fixes concretos y verificación |
| 5 | Comandos de verificación rápida (copy-paste) |
| 6 | Historia completa del fork (contexto para decisiones futuras) |

---

## 1. Contexto

El usuario ejecuta varias instancias de VNote en escritorios virtuales de Linux y quiere:

1. **Multi-instancia** con configuraciones separadas (`--workspace-id <id>` aísla config + datos).
2. **Distinción visual** entre instancias (color del icono de bandeja derivado del hash del ID).
3. **Drag-and-drop**: arrastrar archivos .md desde el explorador de archivos a la barra de
   pestañas de VNote y que se abran en pestañas nuevas.
4. Que el ajuste "Allow multiple instances" de Settings **persista** tras Aplicar→Reiniciar.

Además este fork mantiene: build Debian 12/Qt 6.4, traducción es_ES completa (upstream rechazó el
PR por divergencia de ramas — ver §6), y una migración a arquitectura limpia (MainWindow2,
ViewArea2, ViewAreaController, ServiceLocator) que convive con el código legacy (sufijo `2`).

**Punto de partida obligatorio:** `git status` → hay 2 archivos modificados sin commitear
(`src/core/configmgr2.h`, `src/main.cpp`) que SON parte de la solución (ver §3).

---

## 2. Trabajo YA terminado (NO rehacer)

Verificado en el árbol actual. Commits relevantes (orden cronológico inverso):

| Commit | Qué hace | Estado |
|---|---|---|
| `ea3e91fa` | feat(app): workspace isolation + tray colors + drag-drop | **COMMIT INCOMPLETO: rompió el build** (ver §3/P0/P1). El contenido conceptual está bien; faltan correcciones de compilación. |
| `c75a1df0` | fix(settings): `flushConfigWrites()` antes del prompt de reinicio | ✅ Terminado. Bug de persistencia resuelto. |
| `4cc666ff` | update-readme (documenta `--new-instance`, `--workspace-id`, drag-drop) | ✅ (drag-drop aún no funciona — el README se adelanta; ajustar si hace falta) |
| `2570cfce` | feat(app): multiple instance support (`--new-instance` + setting + `SingleInstanceGuard::tryListenOnly()`) | ✅ Terminado y probado por el usuario. |
| `9cb90e19` | i18n(es): registra es_ES en selector de idiomas | ✅ `src/core/coreconfig.cpp:105`, `src/CMakeLists.txt:26`, `.ts`/`.qm` en `src/data/core/translations/`. |
| `c09434d3` + anteriores | build Debian 12/Qt 6.4, qwindowkit fork | ✅ |

Piezas concretas YA implementadas y verificadas (no tocar salvo bug):

- **Aislamiento por workspace** — `src/core/configmgr2.cpp:40-65`: ctor `ConfigMgr2(ConfigCoreService*, const QString &p_workspaceId, ...)`;
  si el ID no está vacío, `appDataPath` y `localDataPath` reciben sufijo `/<workspaceId>`. Implementación de `getWorkspaceId()` en línea 216.
- **Color de tray por workspace** — `src/widgets/systemtrayhelper.cpp:18-47`: acepta `p_workspaceId`, hash → matiz HSV, repinta el pixmap
  (`CompositionMode_SourceIn`). Lo invoca `MainWindow2::setupSystemTray()` (`src/widgets/mainwindow2.cpp:858-859`) pasando
  `ConfigMgr2::getWorkspaceId()`.
- **Parseo CLI** — `src/commandlineoptions.cpp`: opciones `--new-instance` y `--workspace-id <id>` ya parsean a `m_newInstance` / `m_workspaceId` (header `src/commandlineoptions.h:30-35`).
- **Guard multi-instancia** — `src/core/singleinstanceguard.h`: `tryRun()` / `tryListenOnly()` / `requestOpenFiles()` / `requestShow()`
  + señales `openFilesRequested` / `showRequested` (server side).
- **Archivo IPC** — la escritura del lado cliente funciona (segunda instancia → primera instancia).

---

## 3. Estado EXACTO del build (verificado 2026-09-02)

Diagnóstico obtenido con `-fsyntax-only` usando los flags reales de `compile_commands.json`
(método reproducible en §5). Toolchain: gcc, mensajes en español, Qt 6.4.

| Archivo | ¿Compila? | Detalle |
|---|---|---|
| `src/main.cpp` | ✅ 0 errores | PERO tiene bug de runtime confirmado (P2): parsea antes de crear `Application`. |
| `src/widgets/mainwindow2.cpp` | ❌ 6 errores | Métodos `protected` de `ViewArea2` (P0). |
| `src/widgets/viewarea2.cpp` | ❌ 5+ errores | `dropEvent` roto (P1). Este TU nunca llegó a compilar. |
| Resto (core_services, vxcore, VTextEdit, etc.) | ✅ | Targets `[ 0%..46%]` ya construidos en `build/`. |
| Binario final `build/src/vnote` | ❌ no existe | Nunca enlazó. Aparecerá en `build/src/vnote` al completar `make vnote`. |

### Working tree sin commitear (PARTE de la solución — no descartar)

```
M src/core/configmgr2.h   # +3 líneas: declara getWorkspaceId() (mainwindow2.cpp:859 la necesita)
M src/main.cpp            # parseo temprano de cmdOptions + pasa m_workspaceId a ConfigMgr2
```

### Errores textuales (para búsqueda rápida tras un rebuild)

`mainwindow2.cpp` — todos "is protected within this context":
```
248:  m_viewArea->loadLayoutFromSession(layout);
352:  QJsonObject layout = m_viewArea->saveLayout();
388:  auto *viewWin = m_viewArea->getCurrentViewWindow();
514:  auto *viewWin = m_viewArea->getCurrentViewWindow();
549:  auto *win = m_viewArea->getCurrentViewWindow();
569:  auto *viewWin = m_viewArea->getCurrentViewWindow();
```

`viewarea2.cpp`:
```
1423: error: 'm_view' was not declared in this scope
1435: error: 'm_workspaces' ... is private within this context   (de ViewAreaController)
1476: error: 'QFileInfo' was not declared in this scope
1485: error: invalid use of incomplete type 'class vnotex::BufferService'
1498: error: expected declaration before '}' token   (llave sobrante al final del archivo)
```

Causa raíz histórica: el commit `ea3e91fa` (a) movió accidentalmente 3 métodos de `ViewArea2`
de `public:` a `protected:` al insertar los overrides de drag-drop (comparar `git show HEAD~1:src/widgets/viewarea2.h`
líneas 68-72 vs HEAD líneas 77-81), y (b) añadió un `dropEvent` en `viewarea2.cpp` escrito como si
`ViewArea2` fuera el controlador (usa `m_view`, `m_workspaces` que son miembros de `ViewAreaController`).

---

## 4. Tareas pendientes (ejecutar EN ESTE ORDEN)

### P0 — Arreglar `viewarea2.h`: devolver 3 métodos a `public:`

**Archivo:** `src/widgets/viewarea2.h`

**Fix:** Cortar estas 3 declaraciones (hoy bajo el primer `protected:`, ~líneas 76-81) y
pegarlas en la sección `public:` (tras `getController()`, ~línea 49):

```cpp
  QJsonObject saveLayout() const;
  void loadLayoutFromSession(const QJsonObject &p_layout);
  ViewWindow2 *getCurrentViewWindow() const;
```

No muevas `saveSession()`/`restoreSession()` (sus consumidores no dan error).
No muevas los overrides `dragEnterEvent`/`dropEvent` (deben quedar `protected`).

**Verificación:** §5 comando [B] → 0 errores en `mainwindow2.cpp`.

**Criterio de done:** `mainwindow2.cpp` compila. Después de P1, `make vnote` completa y genera
`build/src/vnote`.

---

### P1 — Reescribir `dropEvent` de `ViewArea2` (versión MVC correcta)

**Archivos:** `src/widgets/viewarea2.cpp`, `src/widgets/viewarea2.h` (menor), ctor de `ViewArea2`.

**Borrar:** TODO el bloque desde el comentario `// Drag and drop support.` (~línea 1388) hasta
el FINAL del archivo, incluida la llave `}` sobrante (~línea 1498-1501). Ese bloque completo es
el dropEvent roto.

**Reemplazar con:**

```cpp
// Drag and drop support.
void ViewArea2::dragEnterEvent(QDragEnterEvent *p_event) {
  if (p_event->mimeData()->hasUrls()) {
    p_event->acceptProposedAction();
  } else {
    QWidget::dragEnterEvent(p_event);
  }
}

void ViewArea2::dropEvent(QDropEvent *p_event) {
  if (!p_event->mimeData()->hasUrls()) {
    QWidget::dropEvent(p_event);
    return;
  }

  QStringList paths;
  const auto urls = p_event->mimeData()->urls();
  for (const QUrl &url : urls) {
    if (url.isLocalFile()) {
      paths << url.toLocalFile();
    }
  }
  if (paths.isEmpty()) {
    p_event->ignore();
    return;
  }

  auto *bufferSvc = m_services.get<BufferService>();
  if (!bufferSvc || !m_controller) {
    qWarning() << "ViewArea2::dropEvent: services unavailable";
    p_event->ignore();
    return;
  }

  FileOpenSettings settings;
  settings.m_focus = true;
  bool opened = false;
  for (const QString &path : paths) {
    QFileInfo fi(path);
    if (!fi.exists() || !fi.isFile()) {
      continue;
    }
    // External file: NodeIdentifier with empty notebookId + ABSOLUTE path.
    // vxcore_buffer_open() accepts NULL notebook_id for external absolute
    // paths (libs/vxcore/include/vxcore/vxcore.h:486-492), and
    // BufferCoreService::openBuffer passes nullptr when notebookId is empty
    // (src/core/services/buffercoreservice.cpp:20-22).
    NodeIdentifier nodeId;
    nodeId.relativePath = fi.absoluteFilePath();
    Buffer2 buffer = bufferSvc->openBuffer(nodeId, settings);
    if (buffer.isValid()) {
      // The controller already handles "no current workspace -> create one"
      // (viewareacontroller.cpp:64-90) and dedupes already-open tabs
      // (viewareacontroller.cpp:97-103). Do NOT replicate that logic here.
      m_controller->openBuffer(buffer, settings);
      opened = true;
    } else {
      qWarning() << "ViewArea2::dropEvent: failed to open" << path;
    }
  }

  if (opened) {
    p_event->acceptProposedAction();
    updateScreenVisibility();
  } else {
    p_event->ignore();
  }
}
```

**Includes a añadir en `viewarea2.cpp`** (los que faltan; `QDragEnterEvent`/`QDropEvent`/`QMimeData`
ya llegan vía `viewarea2.h`):

```cpp
#include <QFileInfo>
#include <core/services/bufferservice.h>  // completa el forward decl de viewareacontroller.h:31
```

**Además — `setAcceptDrops`:** el ctor de `ViewArea2` NO llama `setAcceptDrops(true)` (verificado;
solo el legacy `ViewArea::ViewArea` lo hace, `src/widgets/viewarea.cpp:47`). Añadir al ctor de
`ViewArea2` (tras `setSizePolicy`):

```cpp
  setAcceptDrops(true);
```

**Riesgo conocido / paso manual obligatorio (drag sobre la BARRA de pestañas):**
`ViewSplit2` hereda `QTabWidget`. Si al soltar sobre la barra de pestañas el drop no llega a
`ViewArea2` (QTabWidget puede consumir eventos de drop según `dragDropMode`), implementar en
`ViewSplit2` un `dropEvent` mínimo que emita una señal nueva `filesDropped(const QStringList &)`
(patrón existente de señales: `src/widgets/viewsplit2.h:109-136`), y conectarla en
`ViewArea2::wireSplitSignals()` al mismo flujo de arriba. Extraer el bucle de apertura a un
método privado `ViewArea2::openDroppedFiles(const QStringList &p_paths)` para reutilizarlo en
ambos puntos y en P3.

**Plan B si `vxcore_buffer_open(ctx, NULL, abs_path)` falla en runtime** (la firma lo promete
pero NO ha sido probada — verifícalo con el binario): usar el mecanismo de importación externa
de vxcore (`vxcore_folder_import_external_file`, `vxcore.h:181-188`) no es equivalente (copia el
archivo a la libreta). Alternativa correcta: investigar cómo abre la app archivos sueltos hoy
(`BufferMgr::open(const QString&)` legacy en `src/core/buffermgr.cpp:113` existe pero es singleton
legacy) y portar ese flujo a `BufferService` como `openExternalBuffer(const QString &p_absPath)`.
Reporta el resultado en este archivo (sección §7 al final) para el siguiente agente.

**Verificación:** §5 [B] → `viewarea2.cpp` 0 errores; §5 [C] → binario enlaza. Prueba manual:
arrastar un .md desde el explorador al área de pestañas.

---

### P2 — Bug de runtime en `main.cpp`: parseo antes de `Application` (CONFIRMADO empíricamente)

**Síntoma probado** (test compilado y ejecutado el 2026-09-02):
`QCoreApplication::arguments()` llamado ANTES de construir la instancia de `Application`
imprime `"Please instantiate the QApplication object first"` y devuelve **lista vacía**.
Consecuencia: `--workspace-id`, `--new-instance`, `--verbose`, etc. se IGNORAN silenciosamente.

**Archivo:** `src/main.cpp`, bloque actual ~líneas 162-189.

**Fix (dos partes):**

1. Construir la lista de argumentos desde `argc`/`argv` (disponibles en `main`):

```cpp
  // QCoreApplication::arguments() requires an app instance; build the list
  // from argc/argv so early parsing (needed by ConfigMgr2) actually works.
  QStringList rawArguments;
  for (int i = 0; i < argc; ++i) {
    rawArguments << QString::fromLocal8Bit(argv[i]);
  }
  CommandLineOptions cmdOptions;
  auto parseResult = cmdOptions.parse(rawArguments);
```

2. **Corregir la rama `Error`:** hoy el if/else hace `return 0` en Error, pero el comentario
   original advierte que WebEngine inyecta flags desconocidos → Error debe CONTINUAR la
   ejecución (solo fprintf), igual que el `case Error: break;` del diseño original:

```cpp
  if (parseResult == CommandLineOptions::VersionRequested) {
    auto versionStr =
        QStringLiteral("%1 %2").arg(ConfigMgr2::c_appName, ConfigMgr2::getApplicationVersion());
    qInfo() << versionStr;
    vxcore_context_destroy(context);
    return 0;
  } else if (parseResult == CommandLineOptions::HelpRequested) {
    qInfo() << cmdOptions.m_helpText;
    vxcore_context_destroy(context);
    return 0;
  } else if (parseResult == CommandLineOptions::Error) {
    fprintf(stderr, "%s\n", qPrintable(cmdOptions.m_errorMsg));
    // Arguments to WebEngineView will be unknown ones. So just let it go.
  }
  // Ok (or tolerated Error): continue with normal startup.
```

**Verificación:** construir (§5 [C]) y ejecutar:
`./build/src/vnote --workspace-id demo -v 2>&1 | grep -i locale` (debe mostrar locale, no salir
vacío). Confirmar en disco que se creó el sufijo: la config/datos aislados aparecen bajo la ruta
de datos de la app con segmento final `demo/` (lógica: `configmgr2.cpp:63-65`).

---

### P3 — Señales IPC sin consumidor (multi-instancia "forwards" archivos que nadie abre)

**Hecho verificado:** `grep -rn "openFilesRequested\|showRequested" src/ --include="*.cpp" |
grep -v singleinstanceguard` → **0 resultados**. Es decir: cuando la 2ª instancia llama
`guard.requestOpenFiles()`, la 1ª instancia (que tiene el IPC server) emite
`SingleInstanceGuard::openFilesRequested` … y nadie la escucha. Ídem `showRequested`.
También `Application::openFileRequested` (evento macOS FileOpen) está sin conectar.

**Sub-bug P3b (verificado):** `MainWindow2::kickOffPostInit(const QStringList &p_pathsToOpen)`
(`src/widgets/mainwindow2.cpp:182`) captura `p_pathsToOpen` en el lambda pero **nunca lo usa**
→ los archivos pasados por CLI (`vnote archivo.md`) nunca se abren.

**Fix propuesto (en `src/main.cpp`, tras `mainWindow.show();`):**

```cpp
    // P3: consume IPC requests from secondary instances.
    QObject::connect(&guard, &SingleInstanceGuard::openFilesRequested, &mainWindow,
                     &MainWindow2::openExternalFiles);
    QObject::connect(&guard, &SingleInstanceGuard::showRequested, &mainWindow,
                     []() { /* mainWindow.showMainWindow(); + raise + activate */ });
```

Esto requiere añadir a `MainWindow2` un slot público:

```cpp
void MainWindow2::openExternalFiles(const QStringList &p_paths) {
  // Reuse the same external-file flow from P1 (ViewArea2::openDroppedFiles):
  // get ViewAreaController via m_viewArea->getController(), BufferService via
  // ServiceLocator, build external NodeIdentifiers (empty notebookId + abs path).
}
```

Y en `kickOffPostInit` llamar `openExternalFiles(p_pathsToOpen)` dentro del `singleShot`
cuando la lista no esté vacía.

**Verificación manual:** instancia 1 corriendo → `./build/src/vnote /ruta/nota.md` con
multi-instancia DESACTIVADO → la instancia 1 debe traerse al frente y abrir la nota.
Con multi-instancia ACTIVADO, la 2ª instancia abre la nota en su propia ventana.

---

### P4 — Ajustar README tras P1/P2/P3

`README.md` (~líneas 148-170) ya documenta `--new-instance`, `--workspace-id` y drag-drop.
Una vez que P1-P3 pasen verificación manual, confirmar que la sección describe el
comportamiento real (especialmente: si drag-drop quedó limitado al área de contenido y no a la
barra de pestañas, ajustar el texto). No hacer antes.

### P5 (opcional, no bloquea) — Detección del escritorio virtual del DE

El usuario preguntó por detectar el workspace/escritorio virtual actual (KDE/GNOME) para
auto-asignar `--workspace-id`. NO implementado. Estado de la investigación:

- **X11:** viable vía `xdotool get_desktop` (wrapper de lanzamiento) o leyendo el atom
  `_NET_CURRENT_DESKTOP` con xcb (Qt6 eliminó QtX11Extras como módulo aparte; requeriría
  `qt6-base-private-dev` — YA instalado en este sistema — y acceso directo a la API xcb).
- **Wayland:** no hay estándar multi-DE; el protocolo de virtual desktops (ext-workspace,
  kde-plasma) es privativo por compositor.
- **Recomendación pragmática:** crear lanzadores/atajos por escritorio en el DE que ejecuten
  `vnote --new-instance --workspace-id ws<N>` con N fijo por lanzador. Cero código en C++.

---

## 5. Comandos de verificación rápida (copy-paste)

**[A] Syntax-check de UN archivo con los flags EXACTOS del build** (rápido, no requiere make):

```bash
cd /home/wachin/Dev/VNote-Dev/vnote/build && python3 -c "
import json, subprocess, shlex
with open('compile_commands.json') as f:
    cmds = json.load(f)
TARGET = 'viewarea2.cpp'   # <- cambia aquí: viewarea2.cpp / mainwindow2.cpp / main.cpp
for c in cmds:
    if c['file'].endswith('/src/' + TARGET) or c['file'].endswith('/widgets/' + TARGET):
        parts = shlex.split(c['command'])
        parts = [p for p in parts if not p.startswith('-o')]
        parts.append('-fsyntax-only')
        r = subprocess.run(parts, cwd=c.get('directory','.'), capture_output=True, text=True)
        errs = [l for l in (r.stderr or '').split('\n') if 'error' in l]
        print(TARGET, 'EXIT:', r.returncode, '| errores:', len(errs))
        print('\n'.join(errs[:15]))
        break
"
```

**[B] = [A] cambiando TARGET.** Usa [A] para viewarea2.cpp, mainwindow2.cpp, main.cpp.

**[C] Build completo** (usa background o timeout — tarda >2 min la primera vez):

```bash
cd /home/wachin/Dev/VNote-Dev/vnote/build && make vnote -j4
# El binario aparecerá en: build/src/vnote
```

**[D] Estado git** (ver §3 antes de descartar cualquier cosa):

```bash
cd /home/wachin/Dev/VNote-Dev/vnote && git status --short && git diff --stat
```

**[E] Tests** (tras enlazar; ver `tests/AGENTS.md`):

```bash
cd /home/wachin/Dev/VNote-Dev/vnote/build && ctest --output-on-failure
```

**[F] Regenerar CMake si cambian CMakeLists:**

```bash
cd /home/wachin/Dev/VNote-Dev/vnote/build && cmake .. && make vnote -j4
```

---

## 6. Historia completa del fork (contexto para decisiones)

1. **Origen:** fork de vnotex/vnote para mantener fixes locales.
2. **Build Debian 12 / Qt 6.4:** serie de commits (`c09434d3`, `73d6959b`, `44cce1b8`, `e4ba34c7`)
   — qwindowkit parcheado para Wayland/Qt 6.4 (fork propio del submódulo), deps del sistema
   Debian 12. El sistema YA tiene instalados los paquetes Qt6 dev necesarios
   (incl. `qt6-webchannel-dev qt6-webengine-dev qt6-5compat-dev qt6-base-private-dev`).
3. **Traducción es_ES:** completa (1626/1620 entradas, `vnote_es_ES.ts` + `.qm`, registrada en
   CMake y selector de idiomas, commit `9cb90e19`). **El PR a upstream fue RECHAZADO (~2026-08-29)**
   porque upstream migró a una rama muy divergente. Si se reintenta, hay que rehacerlo sobre su
   nueva rama base. El Roadmap local (`Roadmap.md`) lo refleja.
4. **Migración a arquitectura limpia** (paralela al legacy): `MainWindow2`, `ViewArea2` +
   `ViewAreaController` (MVC), `ServiceLocator` (DI), servicios `*CoreService` sobre la librería C
   `libs/vxcore`. El legacy (`MainWindow`, `ViewArea`, `BufferMgr`, singleton `VNoteX`)
   convive, marcado `VNOTEX_DEPRECATED`. **Todo trabajo nuevo va en los archivos con sufijo `2`.**
5. **Multi-instancia + workspace-id + drag-drop:** commits `2570cfce`, `c75a1df0`, `ea3e91fa`
   (este último incompleto — motivo de este handoff) + working tree pendiente de commit (§3).
6. **Upstream PR:** NO abrir PRs contra upstream sin confirmación explícita del usuario
   (rechazo previo documentado).

## 7. Bitácora de verificación runtime (rellenar al avanzar)

- [ ] P0: `mainwindow2.cpp` compila (verificado con [B]).
- [ ] P1: `viewarea2.cpp` compila; binario enlaza ([C]).
- [ ] P1: `vxcore_buffer_open(ctx, NULL, "/ruta/abs.md")` resultado: ______ (funciona / error ___).
- [ ] P1: drop sobre barra de pestañas: funciona directo / requirió señal `filesDropped` / no funciona.
- [ ] P2: `vnote --workspace-id demo` crea rutas con sufijo `demo/` (sí/no).
- [ ] P2: `vnote archivo.md` abre el archivo tras P3b (sí/no).
- [ ] P3: segunda instancia reenvía archivos a la primera (sí/no).
- [ ] Commit final: `feat(app): fix build of workspace isolation and drag-drop` (estilo del repo).

> Al terminar cada ítem, actualízalo aquí. Si descubres un hecho nuevo (API que no existe,
> comportamiento distinto al documentado), corrige la sección correspondiente de este archivo
> en el mismo commit — este documento es la fuente de verdad para el siguiente agente.
