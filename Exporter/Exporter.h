#pragma once

#include <QtWidgets/QMainWindow>
#include <QFileDialog>
#include "ui_Exporter.h"

class Exporter : public QMainWindow
{
	Q_OBJECT

private:
	Ui::ExporterClass ui;
	QPushButton* m_Button;
	QProgressBar* m_ProgressBar;
	QListWidget* m_Paths;

	QIcon* m_ImageIcon;
	QIcon* m_ModelIcon;

public:
	Exporter(QWidget *parent = Q_NULLPTR);

private slots:
	void SelectPathButton();

};
