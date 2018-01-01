#include "Exporter.h"

Exporter::Exporter(QWidget *parent)
	: QMainWindow(parent) {
	ui.setupUi(this);
	m_Button = ui.pushButton;
	m_ProgressBar = ui.progressBar;
	m_ProgressBar->setValue(0);
	m_Paths = ui.files;
	m_ImageIcon = new QIcon("Resources/image.png");
	m_ModelIcon = new QIcon("Resources/model.png");
	connect(m_Button, SIGNAL(released()), this, SLOT(SelectPathButton()));
}


void Exporter::SelectPathButton() {
	QString fileName = QFileDialog::getOpenFileName(this, "Open Image", QDir::currentPath(), tr("Image files (*.png *.jpg *.bmp *.tga *.jpeg);;Model files (*.obj)"));
	QListWidgetItem* item = new QListWidgetItem(fileName);
	item->setIcon(*m_ImageIcon);
	m_Paths->addItem(item);
}