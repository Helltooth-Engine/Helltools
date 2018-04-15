#pragma once

#include <QLabel>
#include <QMouseEvent>

class SkyboxFace : public QLabel {
	Q_OBJECT
public:
	SkyboxFace(QWidget* parent);

	static SkyboxFace* s_SelectedFace;

	void mouseMoveEvent(QMouseEvent* e) override;
	void mousePressEvent(QMouseEvent* e) override;

private:
	QLabel* asLabel() { return static_cast<QLabel*>(parent()); }
};
