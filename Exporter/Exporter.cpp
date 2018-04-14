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
		std::getline(f, path);
		m_OpenSetting = QString(path.c_str());
		f.close();
	}
	else {
		m_ExportPath = QDir::currentPath();
		m_ImportPath = QDir::currentPath();
		m_OpenSetting = "Image";
	}

	setAcceptDrops(true);

	m_Skybox = ui.skybox;
	m_Skybox->setVisible(false);
	m_Skybox->setScaledContents(true);
	m_Skybox->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
	
	m_Faces.push_back(ui.left);
	m_Faces.push_back(ui.right);
	m_Faces.push_back(ui.front);
	m_Faces.push_back(ui.back);
	m_Faces.push_back(ui.top);
	m_Faces.push_back(ui.bottom);

	ui.left   ->setText("L");
	ui.right  ->setText("R");
	ui.front  ->setText("F");
	ui.back   ->setText("B");
	ui.top    ->setText("T");
	ui.bottom ->setText("Bt");
	
	QFont font = ui.left->font();
	font.setPointSize(20);
	font.setBold(true);

	m_FaceWidth = m_Skybox->width() / 4;
	m_FaceHeight = m_Skybox->height() / 3;
	
	ui.top    ->setGeometry(QRect(m_Skybox->x() + 1 * m_FaceWidth, m_Skybox->y() + 0 * m_FaceHeight, m_FaceWidth, m_FaceHeight));
	ui.bottom ->setGeometry(QRect(m_Skybox->x() + 1 * m_FaceWidth, m_Skybox->y() + 2 * m_FaceHeight, m_FaceWidth, m_FaceHeight));
	ui.back   ->setGeometry(QRect(m_Skybox->x() + 0 * m_FaceWidth, m_Skybox->y() + 1 * m_FaceHeight, m_FaceWidth, m_FaceHeight));
	ui.left   ->setGeometry(QRect(m_Skybox->x() + 1 * m_FaceWidth, m_Skybox->y() + 1 * m_FaceHeight, m_FaceWidth, m_FaceHeight));
	ui.front  ->setGeometry(QRect(m_Skybox->x() + 2 * m_FaceWidth, m_Skybox->y() + 1 * m_FaceHeight, m_FaceWidth, m_FaceHeight));
	ui.right  ->setGeometry(QRect(m_Skybox->x() + 3 * m_FaceWidth, m_Skybox->y() + 1 * m_FaceHeight, m_FaceWidth, m_FaceHeight));
	
	for (QLabel* face : m_Faces) {
		face->setFont(font);
		face->setAlignment(Qt::AlignCenter);
		face->setFixedWidth(m_FaceWidth);
		face->setFixedHeight(m_FaceHeight);
		//face->setVisible(false);
		face->setScaledContents(true);
		face->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
		face->setStyleSheet("border-image: url(Resources/select.png);");
	}
	

	m_PathButton = ui.pushButton;
	m_FolderButton = ui.selectFolder;

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
	connect(m_FolderButton, SIGNAL(released()), this, SLOT(SelectPathFolder()));
	connect(m_ExportButton, SIGNAL(released()), this, SLOT(SelectExportLocation()));
	connect(m_ConvertAll, SIGNAL(released()), this, SLOT(ConvertAll()));
	connect(m_Paths, SIGNAL(itemClicked(QListWidgetItem*)), this, SLOT(OnListClick(QListWidgetItem*)));
	connect(ui.texture2D, SIGNAL(toggled(bool)), this, SLOT(TextureTypeToggled(bool)));
}

Exporter::~Exporter() {
	delete m_ImageIcon, m_ModelIcon;

	QString path = QDir::currentPath() + "/settings.txt";
	std::string filePath = path.toUtf8().toStdString();

	std::ofstream f(filePath);
	f << m_ExportPath.toUtf8().toStdString().c_str() << "\n";
	f << m_ImportPath.toUtf8().toStdString().c_str() << "\n";
	f << m_OpenSetting.toUtf8().toStdString().c_str();

	f.close();
}

void Exporter::dragEnterEvent(QDragEnterEvent* e) {
	if (e->mimeData()->hasUrls()) {
		e->acceptProposedAction();
	}
}

void Exporter::dropEvent(QDropEvent *e) {
	QList<QUrl> urls = e->mimeData()->urls();
	for (const QUrl &url : urls) {
		QString fileName = url.toLocalFile();
		QStringList imageExt = QStringList() << "jpg" << "png" << "bmp" << "tga" << "jpeg";
		QStringList splitFileName = fileName.split(".");

		if (splitFileName[splitFileName.size() - 1] == "obj") {
			QListWidgetItem* item = new QListWidgetItem(*m_ModelIcon, fileName);
			item->setData(Qt::UserRole, 1);
			m_Paths->addItem(item);
		}
		else {
			for (const QString& extensions : imageExt) {
				if (splitFileName[splitFileName.size() - 1] == extensions) {
					QListWidgetItem* item = new QListWidgetItem(*m_ImageIcon, fileName);
					item->setData(Qt::UserRole, 0);
					m_Paths->addItem(item);
					break;
				}
			}
		}
	}
	m_NewFiles = true;
}

void Exporter::SelectPathButton() {
	QFileDialog dialog;
	dialog.setAcceptMode(QFileDialog::AcceptOpen);
	if(m_OpenSetting == "Image")
		dialog.setNameFilter("Image files (*.png *.jpg *.bmp *.tga *.jpeg);;Model files (*.obj)");
	else if(m_OpenSetting == "Model")
		dialog.setNameFilter("Model files (*.obj);;Image files (*.png *.jpg *.bmp *.tga *.jpeg)");

	dialog.setFileMode(QFileDialog::ExistingFiles);
	dialog.setDirectory(m_ImportPath);

	if (dialog.exec() == QDialog::Accepted) {
		QStringList fileNames = dialog.selectedFiles();
		QString selectedFilter = dialog.selectedNameFilter();
		m_ImportPath = dialog.directory().absolutePath();
		m_OpenSetting = selectedFilter.split(" ")[0];
		if (m_OpenSetting == "Image") {
			for (int i = 0; i < fileNames.size(); i++) {
				QListWidgetItem* item = new QListWidgetItem(*m_ImageIcon, fileNames[i]);
				item->setData(Qt::UserRole, 0);
				m_Paths->addItem(item);
				
			}
		}
		else if (m_OpenSetting == "Model") {
			for (int i = 0; i < fileNames.size(); i++) {
				QListWidgetItem* item = new QListWidgetItem(*m_ModelIcon, fileNames[i]);
				item->setData(Qt::UserRole, 1);
				m_Paths->addItem(item);
			}
		}
		m_NewFiles = true;
	}
}

void Exporter::SelectPathFolder() {
	QFileDialog dialog;
	dialog.setAcceptMode(QFileDialog::AcceptOpen);
	dialog.setFileMode(QFileDialog::DirectoryOnly);
	dialog.setDirectory(m_ImportPath);

	if (dialog.exec() == QDialog::Accepted) {
		QDirIterator it(dialog.directory().absolutePath(), QStringList() << "*.jpg" << "*.png" << "*.bmp" << "*.tga" << "*.jpeg" << "*.obj", QDir::Files, QDirIterator::Subdirectories);
		while (it.hasNext()) {
			QString current = it.next();
			QStringList stringSplit = current.split(".");
			if (stringSplit[stringSplit.size() - 1] == "obj") {
				QListWidgetItem* item = new QListWidgetItem(*m_ModelIcon, current);
				item->setData(Qt::UserRole, 1);
				m_Paths->addItem(item);
			}
			else {
				QListWidgetItem* item = new QListWidgetItem(*m_ImageIcon, current);
				item->setData(Qt::UserRole, 0);
				m_Paths->addItem(item);
			}
		}
		m_NewFiles = true;
	}
}

QString Exporter::CreateFileName(const QString& path, const QString& extension) {
	if (m_NewFiles) {
		// Decide on the nameLevel
		m_NameLevel = 0;
		bool found = true;
		QStringList split1;
		QStringList split2;
		while (found) {
			found = false;
			for (int i = 0; i < m_Paths->count() && !found; i++) {
				for (int j = i + 1; j < m_Paths->count(); j++) {
					split1 = m_Paths->item(i)->text().split("/");
					split2 = m_Paths->item(j)->text().split("/");

					bool result = true;

					for (int k = 1; k <= m_NameLevel + 1; k++)
						if (split1[split1.size() - (k)] != split2[split2.size() - (k)])
							result = false;

					if (result) {
						m_NameLevel++;
						found = true;
						break;
					}
				}
			}

		}
		m_NewFiles = false;
	}
	
	QString folderName = "";
	
	QStringList splitPath1 = path.split("/");
	for (int i = splitPath1.size() - m_NameLevel - 1; i < splitPath1.size() - 1; i++)
		folderName += "/" + splitPath1[i];

	QStringList splitPath2 = splitPath1[splitPath1.size() - 1].split(".");
	QString fileName = splitPath2[splitPath2.size() - 2];

	fileName += extension;

	folderName = m_ExportPath + folderName;
	QDir().mkdir(folderName);
	return folderName +  "/" + fileName;
}


void Exporter::PathsRightClick(QPoint location) {
	m_DeleteMenu->exec(m_Paths->mapToGlobal(location));
}

void Exporter::ConvertSingle() {
	auto items = m_Paths->selectedItems();
	float m_Step = 100.0f / (float)items.count();
	float currentProgress = 0;
	for (QListWidgetItem* item : items) {
		currentProgress += m_Step;
		m_ResourcesBar->setValue(currentProgress);
		if (item->data(Qt::UserRole) == 0) {
			Process2DTexture(item->text());
		}
		else if (item->data(Qt::UserRole) == 1) {
			ProcessModel(item->text());
		}
	}
}

void Exporter::DeleteItem() {
	auto items = m_Paths->selectedItems();
	for (QListWidgetItem* item : items) {
		delete item;
		m_NewFiles = true;
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
			Process2DTexture(item->text());
		}
		else if (item->data(Qt::UserRole) == 1) {
			ProcessModel(item->text());
		}
	}
}

void Exporter::Process2DTexture(const QString& path) {
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

	QString fileName = CreateFileName(path, ".httexture");

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

	QString fileName = CreateFileName(path, ".htmodel");


	m_ResourceBar->setValue(87);
	m_ProgressAction->setText("Saving the file...");
	buffer.writeFile(fileName.toStdString());

	delete db;

	importer.FreeScene();
	m_ProgressAction->setText("Done!");
	m_ResourceBar->setValue(100);
}

void Exporter::OnListClick(QListWidgetItem* item) {
	m_CurrentSelected = item;
	if (item->data(Qt::UserRole) == 0) {
		ui.textureFormat->setEnabled(true);
		bool found = false;
		for (size_t i = 0; i < m_TextureTypes.size(); i++) {
			if (m_TextureTypes[i].first == item) {
				ui.texture2D->setChecked(m_TextureTypes[i].second.type == Type::TEXTURE_2D);
				ui.texture3D->setChecked(m_TextureTypes[i].second.type == Type::TEXTURE_3D);
				found = true;
				break;
			}
		}
		if (!found) {
			m_TextureTypes.push_back({ item, {Type::TEXTURE_2D, nullptr } } );
			ui.texture2D->setChecked(true);
		}
	}
	else {
		ui.textureFormat->setEnabled(false);
	}
}


void Exporter::TextureTypeToggled(bool checked) {
	if (m_CurrentSelected) {
		for (size_t i = 0; i < m_TextureTypes.size(); i++) {
			if (m_TextureTypes[i].first == m_CurrentSelected) {
				m_TextureTypes[i].second.type = checked ? Type::TEXTURE_2D : Type::TEXTURE_3D;
				if (m_TextureTypes[i].second.type == Type::TEXTURE_3D) {
					if (m_TextureTypes[i].second.map == nullptr)
						m_TextureTypes[i].second.map = new QPixmap(m_CurrentSelected->text());
					m_Skybox->setVisible(true);
					m_Skybox->setPixmap(*m_TextureTypes[i].second.map);
					for (QLabel* face : m_Faces) {
						face->setVisible(true);
					}
				}
				else {
					for (QLabel* face : m_Faces)
						face->setVisible(false);
					m_Skybox->setVisible(false);
				}
				break;
			}
		}
	}
}
