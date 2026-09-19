#ifndef LAB2_EDITOR_HEADER
#define LAB2_EDITOR_HEADER

#include <Framework.h>
#include <Shape.h>
#include <EventSystem.h>
#include <WindowEvent.h>
#include <array>

namespace lab {
	class Window;

	class IShapeEditor {
	public:
		virtual ~IShapeEditor() = default;

		virtual void OnMouseLBPressed(POINT pos) { m_pressedPos = pos; }
		virtual void OnMouseRBPressed(POINT pos) = 0;
		virtual void OnMouseLBReleased(POINT pos) = 0;
		virtual void OnCursorMove(POINT pos) = 0;

		virtual IShape* GetIntermediate() const noexcept { return nullptr; }

		inline IShape* GetNewShape() noexcept {
			IShape* newShape = m_newShape;
			m_newShape = nullptr;
			return newShape;
		}

	protected:
		POINT   m_pressedPos = { -1, -1 };
		IShape* m_newShape	 = nullptr;
	}; // class IShapeEditor

	class PointEditor final : public IShapeEditor {
	public:
		virtual ~PointEditor() = default;
		virtual void OnMouseLBReleased(POINT pos) override;
		virtual void OnMouseRBPressed(POINT pos) override {}
		virtual void OnCursorMove(POINT pos) override;
	}; // class PointEditor

	class ITwoPointShapeEditor : public IShapeEditor {
	public:
		virtual ~ITwoPointShapeEditor() override;

		virtual void OnMouseLBPressed(POINT pos) override;
		virtual void OnMouseRBPressed(POINT pos) override;
		virtual IShape* GetIntermediate() const noexcept override;

	protected:
		bool    m_hasIntermediate = false;
		IShape* m_intermediate    = nullptr;
	}; // class TwoPointShapeEditor

	class LineEditor final : public ITwoPointShapeEditor {
	public:
		LineEditor();
		virtual void OnMouseLBPressed(POINT pos) override;
		virtual void OnMouseLBReleased(POINT pos) override;
		virtual void OnCursorMove(POINT pos) override;
	}; // class LineEditor

	class RectangleEditor final : public ITwoPointShapeEditor {
	public:
		RectangleEditor();
		virtual void OnMouseLBPressed(POINT pos) override;
		virtual void OnMouseLBReleased(POINT pos) override;
		virtual void OnCursorMove(POINT pos) override;
	}; // class RectangleEditor

	class EllipseEditor final : public ITwoPointShapeEditor {
	public:
		EllipseEditor();
		virtual void OnMouseLBPressed(POINT pos) override;
		virtual void OnMouseLBReleased(POINT pos) override;
		virtual void OnCursorMove(POINT pos) override;
	}; // class EllipseEditor

	class Editor {
	public:
		Editor(Window* window, EventSystem* eventSystem);
		~Editor();

	public:
		static UINT GetMainMenuAsIntResource() noexcept;
		void Render();

	private:
		void _EmplaceNewShapeIfExists();

	private:
		enum class _ShapeType {
			Point,
			Line,
			Rect,
			Ellipse,

			Count,
			None,
		} m_selectedShapeType = _ShapeType::None;

		size_t							 m_shapeCount			= 0;
		Window*							 m_window			    = nullptr;
		EventSystem*				     m_eventSystem			= nullptr;
		EventSystem::EventListenerHandle m_mouseButtonListener  = EventSystem::EventListenerHandle::Null();
		EventSystem::EventListenerHandle m_cursorMoveListener   = EventSystem::EventListenerHandle::Null();
		EventSystem::EventListenerHandle m_wmCommandListener    = EventSystem::EventListenerHandle::Null();
		IShapeEditor*					 m_shapeEditors[(int)_ShapeType::Count] = {};
		std::array<IShape*, 131>		 m_shapes;
	}; // class Editor
} // namespace lab
#endif // !LAB2_EDITOR_HEADER