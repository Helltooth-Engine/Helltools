#include "Exporter.h"

Exporter::Exporter(QWidget *parent)
	: QMainWindow(parent) {
	ui.setupUi(this);
	m_PathButton = ui.pushButton;
	m_ConvertAll = ui.convert;

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
	m_ExportPath = QDir::currentPath();

	m_DeleteMenu = new QMenu("Action");
	m_DeleteMenu->addAction("Convert single", this, SLOT(ConvertSingle()));
	m_DeleteMenu->addAction("Delete entry", this, SLOT(DeleteItem()));

	m_Paths->setContextMenuPolicy(Qt::CustomContextMenu);

	connect(m_Paths, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(PathsRightClick(QPoint)));
	connect(m_PathButton, SIGNAL(released()), this, SLOT(SelectPathButton()));
	connect(m_ExportButton, SIGNAL(released()), this, SLOT(SelectExportLocation()));
	connect(m_ConvertAll, SIGNAL(released()), this, SLOT(ConvertAll()));
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
				item->setData(Qt::UserRole, 0);
				m_Paths->addItem(item);
				
			}
		}
		else if (selectedFilter.split(" ")[0] == "Model") {
			for (int i = 0; i < fileNames.size(); i++) {
				QListWidgetItem* item = new QListWidgetItem(*m_ModelIcon, fileNames[i]);
				item->setData(Qt::UserRole, 1);
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
		m_ExportPath = location;
	}
}

void Exporter::ConvertAll() {
	float m_Step = 100.0f / (float)m_Paths->count();
	float currentProgress = 0;
	for (int i = 0; i < m_Paths->count(); i++) {
		currentProgress += m_Step;
		m_ResourcesBar->setValue(currentProgress);
		QListWidgetItem* item = m_Paths->item(i);
		if (item->data(Qt::UserRole) == 0) {
			ProcessTexture(item->text());
		}
		else if (item->data(Qt::UserRole) == 1) {

		}
	}
}

void Exporter::ProcessTexture(QString path) {
	m_ResourceBar->setValue(0);
	m_ProgressAction->setText("Setting up for loading...");

	std::string stdPath = path.toStdString();

	FREE_IMAGE_FORMAT fif = FreeImage_GetFileType(stdPath.c_str());
	if (fif == FIF_UNKNOWN)
		fif = FreeImage_GetFIFFromFilename(stdPath.c_str());

	m_ProgressAction->setText("Reading Image...");
	m_ResourceBar->setValue(14);

	FIBITMAP* dib;
	if (FreeImage_FIFSupportsReading(fif)) dib = FreeImage_Load(fif, stdPath.c_str());

	m_ProgressAction->setText("Copying data from image...");
	m_ResourceBar->setValue(28);

	unsigned char* pixels = FreeImage_GetBits(dib);
	unsigned int width = FreeImage_GetWidth(dib);
	unsigned int height = FreeImage_GetHeight(dib);
	unsigned int bpp = FreeImage_GetBPP(dib);

	unsigned int size = width * height * bpp / 8;

	unsigned char* result = new unsigned char[size];

	memcpy(result, pixels, size);

	FreeImage_Unload(dib);

	m_ProgressAction->setText("Creating fields...");
	m_ResourceBar->setValue(42);

	
	Field* fWidth = new Field("width", (int)width);
	Field* fHeight = new Field("height", (int)height);
	Field* fBpp = new Field("bpp", (int)bpp);

	m_ProgressAction->setText("Creating array and storing pixels...");
	m_ResourceBar->setValue(56);

	Array* array = new Array("pixels", result, size);

	Object* object = new Object("texture2D");
	object->addField(fWidth);
	object->addField(fHeight);
	object->addField(fBpp);
	object->addArray(array);

	m_ProgressAction->setText("Creating database and buffer");
	m_ResourceBar->setValue(70);

	Database* database = new Database("texture");
	database->addObject(object);

	Buffer buffer = Buffer(database->getSize());
	database->write(buffer);

	m_ProgressAction->setText("Writing to file...");
	m_ResourceBar->setValue(86);

	//Creating file name & adding extesion

	QStringList splitPath = path.split("/");
	QStringList splitPath2 = splitPath[splitPath.size() - 1].split(".");
	QString fileName = splitPath2[0];

	fileName += ".httexture";

	fileName = m_ExportPath + "/" + fileName;

	buffer.writeFile(fileName.toStdString());

	m_ProgressAction->setText("Done!");
	m_ResourceBar->setValue(100);
	delete[] result;
	delete database;
}