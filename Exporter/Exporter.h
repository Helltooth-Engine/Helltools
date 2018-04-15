#pragma once
#include <QtWidgets/QMainWindow>
#include <QFileDialog>
#include <QMouseEvent>
#include <QDirIterator>
#include <QMimeData>
#include <QPair>
#include <QList>

#include "ui_Exporter.h"
#define FREEIMAGE_BIGENDIAN
#include <FreeImage/FreeImage.h>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include "Cereal.h"

#include "SkyboxFace.h"

using namespace Cereal;

enum class Type {
	TEXTURE_2D,
	TEXTURE_3D,
};

struct Texture {
	Type type;
	QPixmap* map = nullptr;

	~Texture() { if (map) delete map; }
};

class Exporter : public QMainWindow {
	Q_OBJECT

private:
	Ui::ExporterClass ui;
	QPushButton* m_PathButton;
	QPushButton* m_ExportButton;
	QPushButton* m_ConvertAll;
	QPushButton* m_FolderButton;

	QProgressBar* m_ResourceBar;
	QProgressBar* m_ResourcesBar;

	QListWidget* m_Paths;

	QLabel* m_ProgressAction;
	QLabel* m_ExportLocation;
	QLabel* m_Skybox;

	QList<SkyboxFace*> m_Faces;

	QIcon* m_ImageIcon;
	QIcon* m_ModelIcon;

	QMenu* m_DeleteMenu;

	QString m_ExportPath;
	QString m_ImportPath;
	QString m_OpenSetting;

	QList<QPair<QListWidgetItem*, Texture>> m_TextureTypes;

	QListWidgetItem* m_CurrentSelected = nullptr;

	int m_NameLevel = 0;

	int m_FaceWidth, m_FaceHeight;

	bool m_NewFiles = false;

public:
	Exporter(QWidget *parent = Q_NULLPTR);
	~Exporter();

	void ProcessModel(const QString& path);
	void Process2DTexture(const QString& path);

	void dropEvent(QDropEvent* e) override;
	void dragEnterEvent(QDragEnterEvent* e) override;

	QString CreateFileName(const QString& path, const QString& extension);

private slots:
	void SelectPathButton();
	void SelectPathFolder();

	void PathsRightClick(QPoint location);

	void ConvertSingle();
	void DeleteItem();

	void SelectExportLocation();

	void ConvertAll();
	
	void OnListClick(QListWidgetItem* item);

	void TextureTypeToggled(bool checked);
};
