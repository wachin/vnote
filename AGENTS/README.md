# Índice de AGENTS/ — puntos de entrada para agentes

Esta carpeta contiene documentos de transferencia escritos **para agentes, no para humanos**:
estado exacto verificado, errores textuales, fixes concretos con código, y comandos de
verificación copy-paste.

## Documentos

| Documento | Qué contiene | Estado |
|---|---|---|
| [HANDOFF-multi-instance-dragdrop.md](HANDOFF-multi-instance-dragdrop.md) | Multi-instancia, `--workspace-id` (aislamiento config + tray icon coloreado), drag-drop sobre pestañas, fixes de build verificados, tareas P0–P5 en orden | **ACTIVO — empezar aquí** |

## Guías relacionadas en la raíz del repo (NO mover, son de otros flujos)

- `AGENTS.md` — convenciones globales del proyecto (MVC, ServiceLocator, estilo). Lectura obligatoria antes de escribir código.
- `Debian12-Branch-Guide.md` / `Debian12-Qt6-Build-Fixes.md` — cómo se preparó la rama `debian12-qt6-build-fixes`.
- `Roadmap.md` — checklist histórico del fork (traducción es_ES, build, PR upstream rechazado).
- `src/*/AGENTS.md` — convenciones por módulo (controllers, widgets, core, tests...).

## Reglas del repo para agentes

1. **Nunca instalar paquetes** sin confirmación del usuario (dar nombre exacto + comando).
2. No abrir PRs contra upstream sin confirmación explícita (PR de traducción rechazado por divergencia).
3. `libs/` son submódulos: no editar salvo instrucción expresa.
4. Código nuevo → archivos con sufijo `2` (arquitectura ServiceLocator); legacy marcado `VNOTEX_DEPRECATED` no se toca salvo bug.
5. Al completar/invalidar un ítem del handoff, actualizar ese documento en el mismo commit.
