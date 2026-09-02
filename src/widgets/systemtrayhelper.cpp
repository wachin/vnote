#include "systemtrayhelper.h"

#include <QApplication>
#include <QFont>
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

QIcon SystemTrayHelper::makeWorkspaceIcon(const QString &p_workspaceId) {
  QIcon icon;
#if defined(Q_OS_MACOS)
  icon = QIcon(":/vnotex/data/core/logo/vnote_mono.png");
#else
  icon = QIcon(":/vnotex/data/core/logo/256x256/vnote.png");
#endif

  if (p_workspaceId.isEmpty()) {
    return icon;
  }

  // Render the base icon at a fixed size.
  QPixmap pixmap = icon.pixmap(QSize(64, 64));
  QPixmap result(pixmap.size());
  result.fill(Qt::transparent);

  QPainter painter(&result);
  painter.setRenderHint(QPainter::Antialiasing);
  painter.drawPixmap(0, 0, pixmap);

  // Tint by workspace ID hash (hue) with SourceIn composition.
  const uint hash = qHash(p_workspaceId);
  const QColor tint = QColor::fromHsv(static_cast<int>(hash % 360), 255, 200);
  painter.setCompositionMode(QPainter::CompositionMode_SourceIn);
  painter.fillRect(result.rect(), tint);
  painter.setCompositionMode(QPainter::CompositionMode_SourceOver);

  // Workspace number badge in the bottom-right corner.
  bool isInt = false;
  const int number = p_workspaceId.toInt(&isInt);
  const QString badgeText = isInt ? QString::number(number) : p_workspaceId.left(2);

  QFont font = painter.font();
  font.setBold(true);
  font.setPixelSize(result.height() / 3);
  painter.setFont(font);

  const QRectF badgeRect(result.width() * 0.5, result.height() * 0.5,
                         result.width() * 0.5, result.height() * 0.5);
  painter.setPen(Qt::NoPen);
  painter.setBrush(QColor(200, 30, 30));
  painter.drawEllipse(badgeRect);

  painter.setPen(Qt::white);
  painter.drawText(badgeRect, Qt::AlignCenter, badgeText);
  painter.end();

  return QIcon(result);
}

QSystemTrayIcon *SystemTrayHelper::setupSystemTray(MainWindow2 *p_win, const ConfigMgr2 *p_configMgr, const QString &p_workspaceId) {
  QIcon icon = makeWorkspaceIcon(p_workspaceId);
#if defined(Q_OS_MACOS)
  icon.setIsMask(true);
#endif

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
