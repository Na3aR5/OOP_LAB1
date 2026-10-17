#include <Shape.h>

void lab::PointShape::Draw(HDC hdc) {
	SetPixel(hdc, m_pos.x, m_pos.y, m_color);
}

void lab::LineShape::Draw(HDC hdc) {
	int oldBkMode = SetBkMode(hdc, TRANSPARENT);

	HPEN pen = CreatePen(m_outlineStyle, 1, m_color);
	HPEN oldPen = (HPEN)SelectObject(hdc, pen);

	MoveToEx(hdc, m_pos.x, m_pos.y, NULL);
	LineTo(hdc, m_to.x, m_to.y);

	DeleteObject(pen);
	SelectObject(hdc, oldPen);

	SetBkMode(hdc, oldBkMode);
}

void lab::LineShape::SetRectPos(POINT pos) noexcept {
	LONG dx = m_to.x - m_pos.x;
	LONG dy = m_to.y - m_pos.y;
	m_pos = pos;
	m_to = POINT{ m_pos.x + dx, m_pos.y + dy };
}

void lab::RectangleShape::Draw(HDC hdc) {
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

void lab::RectangleShape::SetRectPos(POINT pos) noexcept {
	LONG dx = m_rightBottom.x - m_pos.x;
	LONG dy = m_rightBottom.y - m_pos.y;
	m_pos = pos;
	m_rightBottom = POINT{ m_pos.x + dx, m_pos.y + dy };
}

void lab::EllipseShape::Draw(HDC hdc) {
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

void lab::EllipseShape::SetRectPos(POINT pos) noexcept {
	LONG dx = m_rightBottom.x - m_pos.x;
	LONG dy = m_rightBottom.y - m_pos.y;
	m_pos = pos;
	m_rightBottom = POINT{ m_pos.x + dx, m_pos.y + dy };
}

void lab::PointLineShape::Draw(HDC hdc) {
	constexpr int radius = 5;

	POINT oldPos = m_pos;

	POINT vec = { m_to.x - m_pos.x, m_to.y - m_pos.y };
	double vecLen = std::hypot(vec.x, vec.y);
	LONG vx = (LONG)std::round((double)vec.x / vecLen * radius);
	LONG vy = (LONG)std::round((double)vec.y / vecLen * radius);

	this->LineShape::Draw(hdc);
	this->EllipseShape::SetPoints(
		POINT{ m_pos.x - vx - radius, m_pos.y - vy - radius },
		POINT{ m_pos.x - vx + radius, m_pos.y - vy + radius }
	);
	this->EllipseShape::Draw(hdc);
	this->EllipseShape::SetPoints(
		POINT{ m_to.x + vx - radius, m_to.y + vy - radius },
		POINT{ m_to.x + vx + radius, m_to.y + vy + radius }
	);
	this->EllipseShape::Draw(hdc);
	this->LineShape::SetPos(oldPos);
}

void lab::PointLineShape::SetRectPos(POINT pos) noexcept {
	this->LineShape::SetRectPos(pos);
}

void lab::CubeFrameShape::Draw(HDC hdc) {
	SIZE size = this->RectangleShape::GetSize();

	POINT lt = m_pos;
	POINT rb = m_rightBottom;

	this->RectangleShape::Draw(hdc);
	this->RectangleShape::SetPoints(
		POINT{ lt.x + size.cx / 2, lt.y - size.cy / 2 },
		POINT{ rb.x + size.cx / 2, rb.y - size.cy / 2 }
	);
	this->RectangleShape::Draw(hdc);

	this->LineShape::SetPoints(lt, POINT{ lt.x + size.cx / 2, lt.y - size.cy / 2 });
	this->LineShape::Draw(hdc);

	this->LineShape::SetPoints(rb, POINT{ rb.x + size.cx / 2, rb.y - size.cy / 2 });
	this->LineShape::Draw(hdc);

	this->LineShape::SetPoints(POINT{ rb.x, lt.y }, POINT{ rb.x + size.cx / 2, lt.y - size.cy / 2 });
	this->LineShape::Draw(hdc);

	this->LineShape::SetPoints(POINT{ lt.x, rb.y }, POINT{ lt.x + size.cx / 2, rb.y - size.cy / 2 });
	this->LineShape::Draw(hdc);

	this->RectangleShape::SetPoints(lt, rb);
}

void lab::CubeFrameShape::SetRectPos(POINT pos) noexcept {
	this->RectangleShape::SetRectPos(pos);
}