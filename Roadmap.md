# Roadmap de traduccion al espanol de VNote

Este archivo sirve como lista de seguimiento para terminar, compilar y probar la
traduccion espanola de VNote en este fork:

- Proyecto original: <https://github.com/vnotex/vnote>
- Fork de trabajo: <https://github.com/wachin/vnote>

## Estado actual

- [x] Crear el archivo de traduccion `src/data/core/translations/vnote_es_ES.ts`.
- [x] Agregar `vnote_es_ES.ts` a `VX_TS_FILES` en `src/CMakeLists.txt`.
- [x] Traducir las cadenas de VNote al espanol.
- [x] Validar que no queden traducciones marcadas como `unfinished`.
- [x] Validar que los marcadores de Qt (`%1`, `%2`, `%n`) se conserven.
- [x] Validar que `lrelease` genere correctamente el archivo `.qm`.
- [ ] Revisar manualmente la traduccion en Qt Linguist para corregir estilo, acentos,
      contexto y textos demasiado largos en la interfaz.
- [ ] Agregar `es_ES` al flujo de empaquetado cuando se quiera generar AppImage
      o instaladores finales. En `src/Packaging.cmake` actualmente aparece:
      `--translations zh_CN,ja`.
- [ ] Probar VNote compilado con la interfaz en espanol.
- [ ] Abrir un pull request desde `wachin/vnote` hacia `vnotex/vnote`.

## Herramientas necesarias

Este fork compila con Qt 6. El workflow local de referencia esta en:

```text
.github/workflows/ci-linux.yml
```

Dependencias principales:

- [ ] Git.
- [ ] CMake 3.20 o superior.
- [ ] Compilador C++17.
- [ ] Qt 6 con `qtwebengine`, `qtwebchannel`, `qtpositioning`, `qtpdf`,
      `qtimageformats`, `qt5compat` y herramientas Linguist.
- [ ] Qt Creator o Qt Linguist para revisar `vnote_es_ES.ts`.
- [ ] Paquetes Linux usados por CI: `extra-cmake-modules`, `libxkbcommon-dev`,
      `libxcb-cursor-dev`, `libfcitx5-qt-dev`, `fcitx-libs-dev`, `fuse`.

Nota: las instrucciones antiguas de la pagina web usan paquetes Qt5. Para esta
rama es mejor seguir el CI actual, que instala Qt 6.8.3.

## Inicializar el repositorio

Si se clona desde cero:

```bash
git clone https://github.com/wachin/vnote.git vnote
cd vnote
git submodule update --init --recursive
```

Si el repositorio ya existe, actualizar submodulos:

```bash
git submodule sync --recursive
git submodule update --init --recursive
```

## Editar la traduccion

Abrir el archivo en Qt Linguist:

```bash
linguist src/data/core/translations/vnote_es_ES.ts
```

Tambien se puede abrir el proyecto en Qt Creator y editar el archivo `.ts` desde
la pestana de traducciones.

Checklist de revision:

- [ ] Revisar menus principales.
- [ ] Revisar dialogs de notas, carpetas y cuadernos.
- [ ] Revisar busqueda.
- [ ] Revisar exportacion.
- [ ] Revisar ViewArea, splits, pestanas y espacios de trabajo.
- [ ] Revisar mensajes de error y confirmacion.
- [ ] Revisar plurales (`%n`) en Qt Linguist.
- [ ] Confirmar que ninguna traduccion rompa placeholders (`%1`, `%2`, `%n`).

## Actualizar cadenas fuente

Cuando cambie el codigo y aparezcan nuevas llamadas a `tr()`, actualizar los
archivos `.ts` desde el build de CMake:

```bash
cmake --build build --target update_translations
```

Si ese target no existe en la version local de Qt/CMake, usar el target que CMake
genere para `lupdate`, o ejecutar manualmente:

```bash
lupdate src -ts src/data/core/translations/vnote_es_ES.ts
```

Advertencia: `lupdate src` puede mostrar errores al intentar parsear JavaScript
moderno/minificado dentro de `src/data/extra/web`. Si el `.ts` se actualiza bien,
esos mensajes no bloquean la traduccion.

## Compilar los idiomas

Desde una carpeta `build` configurada con CMake:

```bash
cmake --build build --target lrelease
```

Validacion rapida solo para espanol:

```bash
lrelease src/data/core/translations/vnote_es_ES.ts -qm /tmp/vnote_es_ES.qm
```

Resultado esperado:

```text
Generated 1626 translation(s) (1626 finished and 0 unfinished)
```

Los `.qm` generados por CMake quedan normalmente bajo:

```text
build/src/translations/
```

## Compilar VNote para probarlo

Configurar y compilar:

```bash
mkdir -p build
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --target vnote
cmake --build build --target lrelease
```

Si se desea construir el paquete usado por el CI:

```bash
cmake --build build --target pack
```

El CI hace pasos adicionales para AppImage, `fcitx-qt5`, `qt6ct`, OpenSSL y
linuxdeploy. Para probar la traduccion localmente, normalmente basta con compilar
el binario `vnote` y los `.qm`.

## Probar la interfaz en espanol

1. Ejecutar VNote compilado:

   ```bash
   ./build/src/vnote
   ```

2. Abrir `Settings`.
3. Ir a `General`.
4. Cambiar `Language:` a `es_ES` o `Spanish`, segun como aparezca en la lista.
5. Reiniciar VNote si la aplicacion lo solicita.
6. Verificar que los menus y dialogos aparezcan en espanol.

Si la traduccion no carga:

- [ ] Confirmar que existe `build/src/translations/vnote_es_ES.qm`.
- [ ] Confirmar que `vnote_es_ES.ts` esta listado en `src/CMakeLists.txt`.
- [ ] Confirmar que VNote esta encontrando la carpeta `translations`.
- [ ] Probar instalando el proyecto o copiando temporalmente `vnote_es_ES.qm`
      junto a las otras traducciones que use el binario.

## Preparar contribucion

Antes de abrir el pull request:

```bash
git status --short
git diff -- src/CMakeLists.txt src/data/core/translations/vnote_es_ES.ts Roadmap.md
lrelease src/data/core/translations/vnote_es_ES.ts -qm /tmp/vnote_es_ES.qm
```

Checklist final:

- [ ] La traduccion compila con `lrelease`.
- [ ] VNote arranca correctamente.
- [ ] La interfaz se puede cambiar a espanol.
- [ ] Se revisaron visualmente los dialogos principales.
- [ ] Se agrego `es_ES` al empaquetado si se va a distribuir un paquete final.
- [ ] Se creo un commit claro, por ejemplo:

  ```bash
  git add src/CMakeLists.txt src/data/core/translations/vnote_es_ES.ts Roadmap.md
  git commit -m "Add Spanish translation"
  ```

