# Guía de la rama `debian12-qt6-build-fixes`

Esta rama es un camino propio, separado del de los desarrolladores originales
(vnotex/vnote). Ellos pasaron a una rama nueva, muy adelantada y con una
arquitectura distinta, por lo que no aceptaron el PR de la traducción al
español. El desarrollo sigue de forma independiente en el fork
<https://github.com/wachin/vnote>.

Qué contiene esta rama:

- Correcciones para compilar con Qt 6.4 en Debian 12 / MX Linux 23
  (ver `Debian12-Qt6-Build-Fixes.md`).
- Traducción completa al español (`vnote_es_ES.ts`, 1626 cadenas,
  0 sin terminar), registrada en `src/CMakeLists.txt` y en el selector de
  idioma (`src/core/coreconfig.cpp`).

## Compilar el programa (con la traducción incluida)

```bash
cmake --build build --target vnote
cmake --build build --target lrelease
./build/src/vnote
```

El `.qm` en español queda en `build/src/translations/vnote_es_ES.qm`.
Para activarlo: Ajustes → General → Language → `español de España`,
y reiniciar VNote.

## Comandos de la traducción

Validación rápida del `.ts`:

```bash
lrelease src/data/core/translations/vnote_es_ES.ts -qm /tmp/vnote_es_ES.qm
```

Resultado esperado: `1626 traducciones (1626 terminados y 0 no finalizados)`.

Regenerar el `.ts` cuando el código agregue o cambie cadenas `tr()`:

```bash
cmake --build build --target update_translations
```

Si ese target no existe con el CMake/Qt local, manual:

```bash
lupdate src -ts src/data/core/translations/vnote_es_ES.ts
```

Nota: `lupdate src` puede imprimir errores al analizar los JavaScript de
`src/data/extra/web`; si el `.ts` se actualiza bien, esos avisos no bloquean.

Revisión de estilo en Qt Linguist:

```bash
linguist src/data/core/translations/vnote_es_ES.ts
```

## Commits de esta rama

Pendiente actual, agrupado por tema:

| Tema | Archivos |
|------|----------|
| Traducción | `src/core/coreconfig.cpp`, `Roadmap.md` |
| Correcciones Debian 12 | `src/views/combinednodeexplorer.cpp`, `src/views/twocolumnsnodeexplorer.cpp`, `src/widgets/findandreplacewidget2.cpp`, `Debian12-Qt6-Build-Fixes.md` |
| Submódulos | `libs/qwindowkit`, `libs/vtextedit` |

Revisar antes de hacer commit:

```bash
git status --short
git diff
```

Commit de la traducción:

```bash
git add src/core/coreconfig.cpp Roadmap.md
git commit -m "i18n(es): register es_ES in the language selector"
```

Commit de las correcciones de compilación:

```bash
git add src/views/combinednodeexplorer.cpp src/views/twocolumnsnodeexplorer.cpp src/widgets/findandreplacewidget2.cpp Debian12-Qt6-Build-Fixes.md
git commit -m "fix: Debian 12 / Qt 6.4 build compatibility"
```

Submódulos (ver advertencia abajo) — primero commitear dentro, luego el
puntero en el repo principal:

```bash
git -C libs/qwindowkit status
git -C libs/qwindowkit add -A
git -C libs/qwindowkit commit -m "fix: Wayland compile for Qt 6.4"
git add libs/qwindowkit
git commit -m "update qwindowkit submodule"
```

## Push de esta rama

Esta rama todavía no tiene upstream en `origin`. Primera vez (crea la rama
en el remoto y la enlaza):

```bash
git push -u origin debian12-qt6-build-fixes
```

Siguientes veces:

```bash
git push
```

## Consejos y advertencias

- **Nunca commitear** `vnotex.log` ni
  `src/data/core/translations/vnote_es_ES.qm`: son artefactos (log en
  tiempo de ejecución y `.qm` generado a mano para pruebas). El `.qm`
  real que usa el binario sale en `build/src/translations/`.
- **Submódulos con cambios locales** (aparecen con `m` en `git status`):
  si un submódulo tiene cambios sin commitear dentro, un
  `git submodule update` puede borrarlos. Commitear dentro del submódulo
  primero, siempre.
- **Commits separados por tema** (traducción vs. correcciones de
  compilación): facilita cherry-picks o rebaseos futuros.
- **No cambiar de rama con cambios sin commitear**: commitear o hacer
  `git stash` antes de `git switch master`, o Git puede negarse o mezclar
  el trabajo.
- **`.gitignore`**: ya cubre lo típico de Linux (`build*`,
  `compile_commands.json`, `tags`, `.cache`, `.vscode`). Faltan dos reglas
  propias de esta rama:

  ```gitignore
  *.log
  src/data/core/translations/vnote_*.qm
  ```

  Los `.qm` que el repo sí trackea (`qtbase_*.qm`, `qtv_*.qm`, etc.) no
  coinciden con esa regla, así que no se ven afectados.
- **No rebasear** sobre la rama nueva de upstream a la ligera: su
  arquitectura cambió, las cadenas `tr()` serán otras y el `.ts` tendría
  que regenerarse con `lupdate` y completarse de nuevo. Si algún día se
  porta la traducción, copiar `vnote_es_ES.ts` y regenerarla sobre esa
  rama.
