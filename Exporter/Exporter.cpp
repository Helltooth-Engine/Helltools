#include "Exporter.h"

Exporter::Exporter(QWidget *parent)
	: QMainWindow(parent) {
	ui.setupUi(this);
	m_PathButton = ui.pushButton;

	m_ResourceBar = ui.resourceBar;
	m_ResourceBar->setValue(0);

	m_ResourcesBar = ui.resourcesBar;
	m_ResourcesBar->setValue(0);

	m_ProgressAction = ui.progressAction;

	m_Paths = ui.files;
	m_Paths->setSelectionMode(QAbstractItemView::ExtendedSelection);

	m_ImageIcon = new QIcon("Resources/image.png");
	m_ModelIcon = new QIcon("Resources/model.png");

	m_ExportLocation = ui.exportLocation;
	m_ExportLocation->setText("Export Location: " + QDir::currentPath());
	m_ExportButton = ui.changeLocation;

	m_DeleteMenu = new QMenu("Action");
	m_DeleteMenu->addAction("Convert single", this, SLOT(ConvertSingle()));
	m_DeleteMenu->addAction("Delete entry", this, SLOT(DeleteItem()));

	m_Paths->setContextMenuPolicy(Qt::CustomContextMenu);

	connect(m_Paths, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(PathsRightClick(QPoint)));
	connect(m_PathButton, SIGNAL(released()), this, SLOT(SelectPathButton()));
	connect(m_ExportButton, SIGNAL(released()), this, SLOT(SelectExportLocation()));
}

Exporter::~Exporter() {
	delete m_ImageIcon, m_ModelIcon;
}

void Exporter::SelectPathButton() {
	QFileDialog dialog;
	dialog.setAcceptMode(QFileDialog::AcceptOpen);
	dialog.setNameFilter("Image files (*.png *.jpg *.bmp *.tga *.jpeg);;Model files (*.obj)");
	dialog.setFileMode(QFileDialog::ExistingFiles);
	dialog.setDirectory(QDir::current());

	if (dialog.exec() == QDialog::Accepted) {
		QStringList fileNames = dialog.selectedFiles();
		QString selectedFilter = dialog.selectedNameFilter();
		if (selectedFilter.split(" ")[0] == "Image") {
			for (int i = 0; i < fileNames.size(); i++) {
				QListWidgetItem* item = new QListWidgetItem(*m_ImageIcon, fileNames[i]);
				m_Paths->addItem(item);
			}
		}
		else if (selectedFilter.split(" ")[0] == "Model") {
			for (int i = 0; i < fileNames.size(); i++) {
				QListWidgetItem* item = new QListWidgetItem(*m_ModelIcon, fileNames[i]);
				m_Paths->addItem(item);
			}
		}
	}
}

void Exporter::PathsRightClick(QPoint location) {
	m_DeleteMenu->exec(m_Paths->mapToGlobal(location));
}

void Exporter::ConvertSingle() {

}

void Exporter::DeleteItem() {
	auto items = m_Paths->selectedItems();
	for (QListWidgetItem* item : items) {
		delete item;
	}
}

void Exporter::SelectExportLocation() {
	QFileDialog dialog;
	dialog.setAcceptMode(QFileDialog::AcceptOpen);
	dialog.setFileMode(QFileDialog::DirectoryOnly);
	dialog.setDirectory(QDir::current());
	if (dialog.exec() == QDialog::Accepted) {
		QString location = dialog.selectedFiles()[0];
		m_ExportLocation->setText("Export Location: " + location);
	}
}