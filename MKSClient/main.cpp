#include "mksclient.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	MKSClient w;
	w.show();
	return a.exec();
}
