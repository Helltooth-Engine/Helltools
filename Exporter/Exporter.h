#pragma once

#include <QtWidgets/QMainWindow>
#include <QFileDialog>
#include <QMouseEvent>

#include "ui_Exporter.h"

#include <FreeImage/FreeImage.h>

class Exporter : public QMainWindow
{
	Q_OBJECT

private:
	Ui::ExporterClass ui;
	QPushButton* m_PathButton;
	QPushButton* m_ExportButton;

	QProgressBar* m_ResourceBar;
	QProgressBar* m_ResourcesBar;

	QListWidget* m_Paths;

	QLabel* m_ProgressAction;
	QLabel* m_ExportLocation;

	QIcon* m_ImageIcon;
	QIcon* m_ModelIcon;

	QMenu* m_DeleteMenu;

public:
	Exporter(QWidget *parent = Q_NULLPTR);
	~Exporter();

private slots:
	void SelectPathButton();
	void PathsRightClick(QPoint location);

	void ConvertSingle();
	void DeleteItem();

	void SelectExportLocation();
};
