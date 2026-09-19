#ifndef LAB2_SHAPE_HEADER
#define LAB2_SHAPE_HEADER

#include <Framework.h>

namespace lab {
	class IShape {
	public:
		IShape(POINT pos, COLORREF color) : m_pos(pos), m_color(color) {}
		virtual ~IShape() = default;

		virtual void Draw(HDC hdc) const = 0;
		virtual void SetOutlineStyle(int style) noexcept { m_outlineStyle = style; }

		inline void SetPos(POINT pos) noexcept { m_pos = pos; }
		inline void SetColor(COLORREF color) noexcept { m_color = color; }
		inline COLORREF GetColor() const noexcept { return m_color; }

	protected:
		int		 m_outlineStyle = PS_SOLID;
		POINT	 m_pos			= { 0, 0 };
		COLORREF m_color		= RGB(0, 0, 0);
	}; // class IShape

	// ------------------------------------------- PointShape -------------------------------------------

	class PointShape final : public IShape {
	public:
		PointShape(POINT pos, COLORREF color) : IShape(pos, color) {}
		virtual ~PointShape() = default;

		virtual void Draw(HDC hdc) const override;
	}; // class PointShape

	// ------------------------------------------- LineShape -------------------------------------------

	class LineShape final : public IShape {
	public:
		LineShape(POINT from, POINT to, COLORREF color) : IShape(from, color), m_to(to) {}
		virtual ~LineShape() = default;

		virtual void Draw(HDC hdc) const override;

		inline void SetTo(POINT to) noexcept { m_to = to; }
		inline void SetPoints(POINT from, POINT to) noexcept { m_pos = from; m_to = to; }

	private:
		POINT m_to = { 0, 0 };
	}; // class LineShape

	// ----------------------------------------- OutlineShape -----------------------------------------

	class IOutlineShape : public IShape {
	public:
		IOutlineShape(POINT pos, COLORREF color, COLORREF outlineColor) : IShape(pos, color), m_outlineColor(outlineColor) {}
		virtual ~IOutlineShape() = default;

		inline void MakeTransparent(bool transparent = true) noexcept { m_hasFill = !transparent; }

	protected:
		bool	 m_hasFill		= true;
		COLORREF m_outlineColor = RGB(0, 0, 0);
	}; // class IOutlineShape

	// ----------------------------------------- RectangleShape -----------------------------------------

	class RectangleShape final : public IOutlineShape {
	public:
		RectangleShape(POINT leftTop, POINT rightBottom, COLORREF color, COLORREF outlineCOlor) :
			IOutlineShape(leftTop, color, outlineCOlor), m_rightBottom(rightBottom) {}
		virtual ~RectangleShape() = default;

		virtual void Draw(HDC hdc) const override;

		inline void SetPoints(POINT leftTop, POINT rightBottom) { m_pos = leftTop; m_rightBottom = rightBottom; }
		inline void SetRightBottom(POINT rightBottom) noexcept { m_rightBottom = rightBottom; }

	private:
		POINT m_rightBottom = { 0, 0 };
	}; // class RectangleShape

	// ------------------------------------------ EllipseShape -------------------------------------------

	class EllipseShape final : public IOutlineShape {
	public:
		EllipseShape(POINT leftTop, POINT rightBottom, COLORREF color, COLORREF outlineColor) :
			IOutlineShape(leftTop, color, outlineColor), m_rightBottom(rightBottom) {}
		virtual ~EllipseShape() = default;

		virtual void Draw(HDC hdc) const override;

		inline void SetPoints(POINT leftTop, POINT rightBottom) { m_pos = leftTop; m_rightBottom = rightBottom; }
		inline void SetRightBottom(POINT rightBottom) noexcept { m_rightBottom = rightBottom; }

	private:
		POINT m_rightBottom = { 0, 0 };
	}; // class EllipseShape
} // namespace lab
#endif // !LAB2_SHAPE_HEADER