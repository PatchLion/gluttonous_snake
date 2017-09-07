#include "GluttonousSnake.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	CGluttonousSnake w;
	w.show();
	return a.exec();
}
