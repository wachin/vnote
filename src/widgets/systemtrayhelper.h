#ifndef SYSTEMTRAYHELPER_H
#define SYSTEMTRAYHELPER_H

#include <QIcon>
#include <QString>

class QSystemTrayIcon;

namespace vnotex {
class MainWindow2;
class ConfigMgr2;

class SystemTrayHelper {
public:
  SystemTrayHelper() = delete;

  static QSystemTrayIcon *setupSystemTray(MainWindow2 *p_win, const ConfigMgr2 *p_configMgr, const QString &p_workspaceId = QString());

  // Build the app icon colored by the workspace ID hash with the workspace
  // number drawn as a badge. Used by both the tray icon and the window icon.
  static QIcon makeWorkspaceIcon(const QString &p_workspaceId);
};
} // namespace vnotex

#endif // SYSTEMTRAYHELPER_H
