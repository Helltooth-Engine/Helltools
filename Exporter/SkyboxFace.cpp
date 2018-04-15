#include "SkyboxFace.h"

SkyboxFace* SkyboxFace::s_SelectedFace = nullptr;
QLabel* SkyboxFace::s_SkyboxTexture = nullptr;

SkyboxFace::SkyboxFace(QWidget* parent)
	: QLabel(parent) {

}


void SkyboxFace::mouseMoveEvent(QMouseEvent* e) {
	QPointF pos = e->windowPos();
	int skyboxX = pos.x() - s_SkyboxTexture->x();
	int skyboxY = pos.y() - s_SkyboxTexture->y();
	int gridSnapX = skyboxX / asLabel()->width();
	int gridSnapY = skyboxY / asLabel()->height();
	asLabel()->move(s_SkyboxTexture->x() + gridSnapX * asLabel()->width(), s_SkyboxTexture->y() + gridSnapY * asLabel()->height());
}

void SkyboxFace::mousePressEvent(QMouseEvent* e) {
	setStyleSheet("border-image: url(Resources/selectActive.png);");

	if (s_SelectedFace && s_SelectedFace != this)
		s_SelectedFace->setStyleSheet("border-image: url(Resources/select.png);");

	s_SelectedFace = this;
}