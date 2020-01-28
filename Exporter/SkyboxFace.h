#pragma once

#include <QLabel>
#include <QMouseEvent>

enum class Face {
	LEFT,
	RIGHT,
	FRONT,
	BACK,
	UP,
	DOWN
};

class SkyboxFace : public QLabel {
	Q_OBJECT
public:
	SkyboxFace(QWidget* parent);

	static SkyboxFace* s_SelectedFace;
	static QLabel* s_SkyboxTexture;

	void mouseMoveEvent(QMouseEvent* e) override;
	void mousePressEvent(QMouseEvent* e) override;

	Face GetFace();
	QPoint GetLayoutLocation();
	void SetLayoutLocation(const QPoint& point);
private:
	QLabel* asLabel() { return static_cast<QLabel*>(parent()); }
};
