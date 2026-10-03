#include <Shape.h>

void lab::PointShape::Draw(HDC hdc) const {
	SetPixel(hdc, m_pos.x, m_pos.y, m_color);
}

void lab::LineShape::Draw(HDC hdc) const {
	int oldBkMode = SetBkMode(hdc, TRANSPARENT);

	HPEN pen = CreatePen(m_outlineStyle, 1, m_color);
	HPEN oldPen = (HPEN)SelectObject(hdc, pen);

	MoveToEx(hdc, m_pos.x, m_pos.y, NULL);
	LineTo(hdc, m_to.x, m_to.y);

	DeleteObject(pen);
	SelectObject(hdc, oldPen);

	SetBkMode(hdc, oldBkMode);
}

void lab::RectangleShape::Draw(HDC hdc) const {
	int oldBkMode = SetBkMode(hdc, TRANSPARENT);

	HPEN pen = CreatePen(m_outlineStyle, 1, m_outlineColor);
	HBRUSH brush = m_hasFill ? CreateSolidBrush(m_color) : (HBRUSH)GetStockObject(NULL_BRUSH);

	HPEN oldPen = (HPEN)SelectObject(hdc, pen);
	HBRUSH oldBrush = (HBRUSH)SelectObject(hdc, brush);

	Rectangle(hdc, m_pos.x, m_pos.y, m_rightBottom.x, m_rightBottom.y);

	SelectObject(hdc, oldPen);
	SelectObject(hdc, oldBrush);

	if (m_hasFill) DeleteObject(brush);
	DeleteObject(pen);

	SetBkMode(hdc, oldBkMode);
}

void lab::EllipseShape::Draw(HDC hdc) const {
	int oldBkMode = SetBkMode(hdc, TRANSPARENT);

	HPEN pen = CreatePen(m_outlineStyle, 1, m_outlineColor);
	HBRUSH brush = m_hasFill ? CreateSolidBrush(m_color) : (HBRUSH)GetStockObject(NULL_BRUSH);

	HPEN oldPen = (HPEN)SelectObject(hdc, pen);
	HBRUSH oldBrush = (HBRUSH)SelectObject(hdc, brush);

	Ellipse(hdc, m_pos.x, m_pos.y, m_rightBottom.x, m_rightBottom.y);

	SelectObject(hdc, oldPen);
	SelectObject(hdc, oldBrush);

	if (m_hasFill) DeleteObject(brush);
	DeleteObject(pen);

	SetBkMode(hdc, oldBkMode);
}