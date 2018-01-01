#include "Exporter.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[]) {
	QApplication a(argc, argv);
	Exporter w;
	w.show();
	return a.exec();
}
