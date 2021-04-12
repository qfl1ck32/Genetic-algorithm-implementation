#include "mainwindow.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;

    a.setWindowIcon(QIcon(":/icon.png"));

    QPalette palette;

    palette.setColor(QPalette :: Window, QColor(53, 53, 53));
    palette.setColor(QPalette :: WindowText, Qt :: white);
    palette.setColor(QPalette :: Base, QColor(25, 25, 25));
    palette.setColor(QPalette :: AlternateBase, QColor(53, 53, 53));
    palette.setColor(QPalette :: ToolTipBase, Qt :: white);
    palette.setColor(QPalette :: ToolTipText, Qt :: white);
    palette.setColor(QPalette :: Text, Qt :: white);
    palette.setColor(QPalette :: Button, QColor(53, 53, 53));
    palette.setColor(QPalette :: ButtonText, Qt :: white);
    palette.setColor(QPalette :: BrightText, Qt :: red);
    palette.setColor(QPalette :: Link, QColor(42, 130, 218));
    palette.setColor(QPalette :: Highlight, QColor(42, 130, 218));
    palette.setColor(QPalette :: HighlightedText, Qt :: white);
    palette.setColor(QPalette :: Active, QPalette :: Button, QColor(128, 128, 128).darker());
    palette.setColor(QPalette :: Disabled, QPalette :: ButtonText, QColor(128, 128, 128));

    a.setPalette(palette);

    w.setWindowState(Qt :: WindowMaximized);
    w.show();

    return a.exec();
}
