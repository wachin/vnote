#include "systemtrayhelper.h"

#include <QApplication>
#include <QIcon>
#include <QMenu>
#include <QPainter>
#include <QPixmap>
#include <QSystemTrayIcon>

#include "mainwindow2.h"
#include "widgetsfactory.h"
#include <core/configmgr2.h>
#include <core/coreconfig.h>
#include <utils/widgetutils.h>

using namespace vnotex;

QSystemTrayIcon *SystemTrayHelper::setupSystemTray(MainWindow2 *p_win, const ConfigMgr2 *p_configMgr, const QString &p_workspaceId) {
#if defined(Q_OS_MACOS)
  QIcon icon(":/vnotex/data/core/logo/vnote_mono.png");
  icon.setIsMask(true);
#else
  QIcon icon(":/vnotex/data/core/logo/256x256/vnote.png");
#endif

  // If workspace ID is provided, modify the icon color for visual distinction
  if (!p_workspaceId.isEmpty()) {
    // Generate a color based on the workspace ID hash
    uint hash = qHash(p_workspaceId);
    // Convert hash to a hue value (0-359)
    int hue = hash % 360;
    // Create a color with the hue, full saturation, and medium lightness
    QColor color = QColor::fromHsv(hue, 255, 200);
    
    // Apply the color as a mask or overlay to the icon
    QPixmap pixmap(icon.pixmap(QSize(64, 64))); // Use a reasonable size
    QPixmap coloredPixmap(pixmap.size());
    coloredPixmap.fill(Qt::transparent);
    
    QPainter painter(&coloredPixmap);
    painter.setCompositionMode(QPainter::CompositionMode_Source);
    painter.drawPixmap(0, 0, pixmap);
    painter.setCompositionMode(QPainter::CompositionMode_SourceIn);
    painter.fillRect(coloredPixmap.rect(), color);
    painter.end();
    
    icon = QIcon(coloredPixmap);
  }

  auto trayIcon = new QSystemTrayIcon(icon, p_win);
  trayIcon->setToolTip(qApp->applicationName());

  MainWindow2::connect(trayIcon, &QSystemTrayIcon::activated, p_win,
                      [p_win](QSystemTrayIcon::ActivationReason p_reason) {
                        Q_UNUSED(p_reason);
#if !defined(Q_OS_MACOS)
                        if (p_reason == QSystemTrayIcon::Trigger) {
                          p_win->showMainWindow();
                        }
#endif
                      });

  auto menu = WidgetsFactory::createMenu(p_win);
  trayIcon->setContextMenu(menu);

  const auto &coreConfig = p_configMgr->getCoreConfig();

  {
    auto act = menu->addAction(MainWindow2::tr("Show Main Window"), menu,
                               [p_win]() { p_win->showMainWindow(); });

    WidgetUtils::addActionShortcutText(act, coreConfig.getShortcut(CoreConfig::Global_WakeUp));
  }

  menu->addSeparator();

  menu->addAction(MainWindow2::tr("Quit"), menu, [p_win]() { p_win->quitApp(); });

  return trayIcon;
}
