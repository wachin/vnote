#ifndef SYSTEMTRAYHELPER_H
#define SYSTEMTRAYHELPER_H

#include <QString>

class QSystemTrayIcon;

namespace vnotex {
class MainWindow2;
class ConfigMgr2;

class SystemTrayHelper {
public:
  SystemTrayHelper() = delete;

  static QSystemTrayIcon *setupSystemTray(MainWindow2 *p_win, const ConfigMgr2 *p_configMgr, const QString &p_workspaceId = QString());
};
} // namespace vnotex

#endif // SYSTEMTRAYHELPER_H
