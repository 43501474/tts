#include "vld.h"
#include "mainwindow.h"
#include "QApplication"
#include "Player.h"
#include <QDebug>

int main(int argc, char *argv[])
{
	Q_UNUSED(argc);
	Q_UNUSED(argv);

	QApplication a(argc, argv);
	MainWindow w;
	w.show();

	return a.exec();
}
