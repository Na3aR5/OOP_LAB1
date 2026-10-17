#ifndef LAB2_SHAPE_HEADER
#define LAB2_SHAPE_HEADER

#include <Framework.h>
#include <cmath>

namespace lab {
	class IShape {
	public:
		IShape(POINT pos, COLORREF color) : m_pos(pos), m_color(color) {}
		virtual ~IShape() = default;

		virtual void Draw(HDC hdc) = 0;
		virtual void SetRectPos(POINT pos) noexcept { m_pos = pos; }
		virtual void SetOutlineStyle(int style) noexcept { m_outlineStyle = style; }

		inline void SetPos(POINT pos) noexcept { m_pos = pos; }
		inline void SetColor(COLORREF color) noexcept { m_color = color; }

		inline POINT GetPos() const noexcept { return m_pos; }
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

		virtual void Draw(HDC hdc) override;
	}; // class PointShape

	// ------------------------------------------- LineShape -------------------------------------------

	class LineShape : virtual public IShape {
	public:
		LineShape(POINT from, POINT to, COLORREF color) : IShape(from, color), m_to(to) {}
		virtual ~LineShape() {};

		virtual void Draw(HDC hdc) override;
		virtual void SetRectPos(POINT pos) noexcept override;

		inline void SetTo(POINT to) noexcept { m_to = to; }
		inline void SetPoints(POINT from, POINT to) noexcept { m_pos = from; m_to = to; }

	protected:
		POINT m_to = { 0, 0 };
	}; // class LineShape

	// ----------------------------------------- OutlineShape -----------------------------------------

	class IOutlineShape : virtual public IShape {
	public:
		IOutlineShape(POINT pos, COLORREF color, COLORREF outlineColor) : IShape(pos, color), m_outlineColor(outlineColor) {}
		virtual ~IOutlineShape() {};

		inline void MakeTransparent(bool transparent = true) noexcept { m_hasFill = !transparent; }

	protected:
		bool	 m_hasFill		= true;
		COLORREF m_outlineColor = RGB(0, 0, 0);
	}; // class IOutlineShape

	// ----------------------------------------- RectangleShape -----------------------------------------

	class RectangleShape : public IOutlineShape {
	public:
		RectangleShape(POINT leftTop, POINT rightBottom, COLORREF color, COLORREF outlineCOlor) : IShape(leftTop, color),
			IOutlineShape(leftTop, color, outlineCOlor), m_rightBottom(rightBottom) {}
		virtual ~RectangleShape() = default;

		virtual void Draw(HDC hdc) override;
		virtual void SetRectPos(POINT pos) noexcept override;

		inline void SetPoints(POINT leftTop, POINT rightBottom) { m_pos = leftTop; m_rightBottom = rightBottom; }
		inline void SetRightBottom(POINT rightBottom) noexcept { m_rightBottom = rightBottom; }

		inline SIZE GetSize() const noexcept {
			return SIZE{ std::abs(m_rightBottom.x - m_pos.x), std::abs(m_rightBottom.y - m_pos.y) };
		}

	protected:
		POINT m_rightBottom = { 0, 0 };
	}; // class RectangleShape

	// ------------------------------------------ EllipseShape -------------------------------------------

	class EllipseShape : public IOutlineShape {
	public:
		EllipseShape(POINT leftTop, POINT rightBottom, COLORREF color, COLORREF outlineColor) : IShape(leftTop, color),
			IOutlineShape(leftTop, color, outlineColor), m_rightBottom(rightBottom) {}
		virtual ~EllipseShape() = default;

		virtual void Draw(HDC hdc) override;
		virtual void SetRectPos(POINT pos) noexcept override;

		inline void SetPoints(POINT leftTop, POINT rightBottom) { m_pos = leftTop; m_rightBottom = rightBottom; }
		inline void SetRightBottom(POINT rightBottom) noexcept { m_rightBottom = rightBottom; }

		inline SIZE GetDiameters() const noexcept {
			return SIZE{ std::abs(m_rightBottom.x - m_pos.x), std::abs(m_rightBottom.y - m_pos.y) };
		}

	private:
		POINT m_rightBottom = { 0, 0 };
	}; // class EllipseShape

	// ------------------------------------------ PointLineShape -------------------------------------------

	class PointLineShape final : public LineShape, public EllipseShape {
	public:
		PointLineShape(POINT from, POINT to, COLORREF color) : IShape(from, color),
		LineShape(from, to, color), EllipseShape(from, to, color, color) {
			this->EllipseShape::MakeTransparent();
		}
		virtual ~PointLineShape() = default;

		virtual void Draw(HDC hdc) override;
		virtual void SetRectPos(POINT pos) noexcept override;
	}; // class PointLineShape

	// ------------------------------------------ CubeFrameShape ---------------------------------------------

	class CubeFrameShape final : public LineShape, public RectangleShape {
	public:
		CubeFrameShape(POINT lt, POINT rb, COLORREF color) : IShape(lt, color),
		LineShape(lt, rb, color), RectangleShape(lt, rb, color, color) {
			this->RectangleShape::MakeTransparent();
		}
		virtual ~CubeFrameShape() = default;

		virtual void Draw(HDC hdc) override;
		virtual void SetRectPos(POINT pos) noexcept override;
	}; // class CubeFrameShape
} // namespace lab
#endif // !LAB2_SHAPE_HEADER