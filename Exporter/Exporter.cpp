#include "Exporter.h"

Exporter::Exporter(QWidget *parent)
	: QMainWindow(parent) {
	ui.setupUi(this);

	QString path = QDir::currentPath() + "/settings.txt";
	std::string filePath = path.toUtf8().toStdString();

	std::ifstream f(filePath);
	if (f.good()) {

		std::string path;
		std::getline(f, path);
		m_ExportPath = QString(path.c_str());
		std::getline(f, path);
		m_ImportPath = QString(path.c_str());

		f.close();
	}
	else {
		m_ExportPath = QDir::currentPath();
		m_ImportPath = QDir::currentPath();
	}

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
	m_ExportLocation->setText("Export Location: " + m_ExportPath);
	m_ExportButton = ui.changeLocation;

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

	QString path = QDir::currentPath() + "/settings.txt";
	std::string filePath = path.toUtf8().toStdString();

	std::ofstream f(filePath);
	f << m_ExportPath.toUtf8().toStdString().c_str() << "\n";
	f << m_ImportPath.toUtf8().toStdString().c_str();

	f.close();
}

void Exporter::SelectPathButton() {
	QFileDialog dialog;
	dialog.setAcceptMode(QFileDialog::AcceptOpen);
	dialog.setNameFilter("Image files (*.png *.jpg *.bmp *.tga *.jpeg);;Model files (*.obj)");
	dialog.setFileMode(QFileDialog::ExistingFiles);
	dialog.setDirectory(m_ImportPath);

	if (dialog.exec() == QDialog::Accepted) {
		QStringList fileNames = dialog.selectedFiles();
		QString selectedFilter = dialog.selectedNameFilter();
		m_ImportPath = dialog.directory().absolutePath();
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
			ProcessModel(item->text());
		}
	}
}

void Exporter::ProcessTexture(const QString& path) {
	m_ResourceBar->setValue(0);
	m_ProgressAction->setText("Setting up for loading...");

	std::string stdPath = path.toStdString();

	FREE_IMAGE_FORMAT fif = FreeImage_GetFileType(stdPath.c_str());
	if (fif == FIF_UNKNOWN)
		fif = FreeImage_GetFIFFromFilename(stdPath.c_str());

	m_ProgressAction->setText("Reading Image...");
	m_ResourceBar->setValue(14);

	FIBITMAP* loadDib;
	if (FreeImage_FIFSupportsReading(fif)) loadDib = FreeImage_Load(fif, stdPath.c_str());
	
	FIBITMAP* dib;

	unsigned int testBpp = FreeImage_GetBPP(loadDib);
	if (testBpp == 24)
		dib = FreeImage_ConvertTo32Bits(loadDib);
	else
		dib = loadDib;

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

void Exporter::ProcessModel(const QString& path) {
	Assimp::Importer importer;
	
	m_ProgressAction->setText("Importing model...");
	m_ResourceBar->setValue(10);

	const aiScene* scene = importer.ReadFile(path.toStdString(), aiProcess_Triangulate | aiProcess_JoinIdenticalVertices);

	aiMesh* *meshes = scene->mMeshes;

	struct Vertex {
		float x, y, z;
		float uvX, uvY;
		float nX, nY, nZ;
	};

	std::vector<Vertex> vertices(0);
	std::vector<unsigned int> indices(0);

	bool hasBones = false;

	m_ProgressAction->setText("Gathering data...");
	m_ResourceBar->setValue(37);
	size_t verticesOffset = 0;
	size_t indicesOffset = 0;
	for (size_t i = 0; i < scene->mNumMeshes; i++) {
		if (meshes[i]->HasBones()) {
			//format not supported, move on
			continue;
		}
		aiMesh* currentMesh = meshes[i];

		vertices.resize(vertices.size() + meshes[i]->mNumVertices);

		for (size_t vertexIndex = 0; vertexIndex < currentMesh->mNumVertices; vertexIndex++) {
			if (currentMesh->HasPositions())
				memcpy(&vertices[verticesOffset + vertexIndex], &currentMesh->mVertices[vertexIndex], sizeof(float) * 3);
			
			if (currentMesh->HasTextureCoords(0))
				memcpy((char*)&vertices[verticesOffset + vertexIndex] + sizeof(float) * 3, &currentMesh->mTextureCoords[0][vertexIndex], sizeof(float) * 2);

			if (currentMesh->HasNormals())
				memcpy((char*)&vertices[verticesOffset + vertexIndex] + sizeof(float) * 5, &currentMesh->mNormals[vertexIndex], sizeof(float) * 3);
		}
		
		verticesOffset += currentMesh->mNumVertices;
		indices.resize(indices.size() + currentMesh->mNumFaces * 3);

		for (size_t faceIndex = 0; faceIndex < currentMesh->mNumFaces; faceIndex++) {
			aiFace face = currentMesh->mFaces[faceIndex];

			memcpy((char*)&indices[0] + indicesOffset * sizeof(unsigned int), &face.mIndices[0], sizeof(unsigned int) * face.mNumIndices);
			indicesOffset += face.mNumIndices;
		}
	}

	m_ProgressAction->setText("Storing the data...");
	m_ResourceBar->setValue(71);
	Field* fHasBones = new Field("hasBones", hasBones);

	float* verticesData = (float*)&vertices[0];
	unsigned int* indicesData = &indices[0];

	Array* aVerticesData = new Array("verticesData", verticesData, vertices.size() * sizeof(Vertex) / sizeof(float));
	Array* aIndicesData = new Array("indicesData", (int*)indicesData, indices.size());
		
	Object* object = new Object("model");
	object->addField(fHasBones);
	object->addArray(aVerticesData);
	object->addArray(aIndicesData);

	Database* db = new Database("model");
	db->addObject(object);

	Buffer buffer = Buffer(db->getSize());
	db->write(buffer);

	QStringList splitPath = path.split("/");
	QStringList splitPath2 = splitPath[splitPath.size() - 1].split(".");
	QString fileName = splitPath2[0];

	fileName += ".htmodel";

	fileName = m_ExportPath + "/" + fileName;

	m_ResourceBar->setValue(87);
	m_ProgressAction->setText("Saving the file...");
	buffer.writeFile(fileName.toStdString());

	delete db;

	importer.FreeScene();
	m_ProgressAction->setText("Done!");
	m_ResourceBar->setValue(100);
}