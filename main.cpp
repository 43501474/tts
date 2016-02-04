////#define _CRTDBG_MAP_ALLOC
//#include <stdlib.h>
//#include <crtdbg.h>
//#ifdef _DEBUG
//   #ifndef DBG_NEW
//      #define DBG_NEW new ( _NORMAL_BLOCK , __FILE__ , __LINE__ )
//      #define new DBG_NEW
//   #endif
//#endif  // _DEBUG

#include "vld.h"
#include "mainwindow.h"
#include "BaiduTTS.h"
#include "QApplication"


int main(int argc, char *argv[])
{
	Q_UNUSED(argc);
	Q_UNUSED(argv);

	BaiduTTS oTTS;
    oTTS.tts("白日依山尽,黄河入海流", "test.wav");
	qDebug() << "main...";
	QApplication a(argc, argv);
	MainWindow w;
	w.show();

	return a.exec();
}
