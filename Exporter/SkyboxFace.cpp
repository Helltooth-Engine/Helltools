#include "SkyboxFace.h"

SkyboxFace* SkyboxFace::s_SelectedFace = nullptr;

SkyboxFace::SkyboxFace(QWidget* parent)
	: QLabel(parent) {

}


void SkyboxFace::mouseMoveEvent(QMouseEvent* e) {
	QPointF pos = e->windowPos();
	asLabel()->move(pos.x() - asLabel()->width() / 2, pos.y() - asLabel()->height() / 2);
}

void SkyboxFace::mousePressEvent(QMouseEvent* e) {
	setStyleSheet("border-image: url(Resources/selectActive.png);");

	if (s_SelectedFace && s_SelectedFace != this)
		s_SelectedFace->setStyleSheet("border-image: url(Resources/select.png);");

	s_SelectedFace = this;
}