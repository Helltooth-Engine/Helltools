#pragma once
#include <QtWidgets/QMainWindow>
#include <QFileDialog>
#include <QMouseEvent>

#include "ui_Exporter.h"
#define FREEIMAGE_BIGENDIAN
#include <FreeImage/FreeImage.h>

#include <Assimp/Importer.hpp>
#include <Assimp/scene.h>
#include <Assimp/postprocess.h>
#include "Cereal.h"

using namespace Cereal;

class Exporter : public QMainWindow {
	Q_OBJECT

private:
	Ui::ExporterClass ui;
	QPushButton* m_PathButton;
	QPushButton* m_ExportButton;
	QPushButton* m_ConvertAll;

	QProgressBar* m_ResourceBar;
	QProgressBar* m_ResourcesBar;

	QListWidget* m_Paths;

	QLabel* m_ProgressAction;
	QLabel* m_ExportLocation;

	QIcon* m_ImageIcon;
	QIcon* m_ModelIcon;

	QMenu* m_DeleteMenu;

	QString m_ExportPath;

public:
	Exporter(QWidget *parent = Q_NULLPTR);
	~Exporter();

	void ProcessModel(const QString& path);
	void ProcessTexture(const QString& path);

private slots:
	void SelectPathButton();
	void PathsRightClick(QPoint location);

	void ConvertSingle();
	void DeleteItem();

	void SelectExportLocation();

	void ConvertAll();
};
