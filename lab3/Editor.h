#ifndef LAB2_EDITOR_HEADER
#define LAB2_EDITOR_HEADER

#include <Framework.h>
#include <Shape.h>
#include <EventSystem.h>
#include <WindowEvent.h>

#include <array>
#include <memory>

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
		class Toolbar {
		public:
			enum class Item {
				Point,
				Line,
				Rect,
				Ellipse,

				Count
			}; // enum class Item

			class IItem {
			public:
				IItem(uint32_t id, RECT box) noexcept : m_box(box), m_id(id) {}
				virtual ~IItem() = default;

				inline bool HitTest(POINT pos) const { return PtInRect(&m_box, pos); }
				virtual void Draw(HDC hdc, bool forceHover = false) const;

				void OnHover() noexcept;
				void OnUnhover() noexcept;
				inline bool IsHovered() const noexcept { return m_hovered; }
				inline RECT GetBox() const noexcept { return m_box; }
				inline uint32_t GetID() const noexcept { return m_id; }

			protected:
				bool	 m_hovered	    = false;
				uint32_t m_id			= UINT32_MAX;
				COLORREF onHoverColor   = RGB(0, 212, 155);
				COLORREF onUnhoverColor = RGB(255, 255, 255);
				RECT	 m_box		    = {};
			}; // class Item

		public:
			Toolbar(Editor* editor, RECT toolbarBox, uint32_t itemWidth, COLORREF fillColor) noexcept :
				m_toolbarBox(toolbarBox), m_itemWidth(itemWidth), m_fillColor(fillColor), m_editor(editor) {}
			~Toolbar() = default;

			bool HitTest(POINT point) noexcept;

			void Draw(HDC hdc) const;
			void ResetHoverStates() noexcept;

			void OnCursorMove(POINT pos) noexcept;
			void OnResize(SIZE windowSize) noexcept;

			inline const RECT& GetBox() const noexcept { return m_toolbarBox; }
			inline SIZE GetItemBoxSize() const noexcept { return SIZE{ (LONG)m_itemWidth, m_toolbarBox.bottom }; }
			inline uint32_t GetHoveredID() const noexcept { return m_hoveredID; }
			inline IItem* GetItem(uint32_t index) noexcept { return m_items[index].get(); }
			inline const IItem* GetItem(uint32_t index) const noexcept { return m_items[index].get(); }

			template <typename ItemType, typename ... Types>
			requires(std::is_base_of_v<IItem, ItemType>)
			bool SetItem(uint32_t index, Types&& ... args) {
				if (index >= (uint32_t)Item::Count) {
					return false;
				}
				RECT itemBox = {};
				itemBox.left   = index * m_itemWidth;
				itemBox.top    = 0;
				itemBox.right  = (index + 1) * m_itemWidth;
				itemBox.bottom = m_toolbarBox.bottom;

				m_items[index] = std::make_unique<ItemType>(itemBox, std::forward<Types>(args)...);
				return true;
			}

		private:
			uint32_t			   m_itemWidth				 = {};
			uint32_t			   m_hoveredID				 = UINT32_MAX;
			COLORREF			   m_fillColor				 = {};
			Editor*				   m_editor					 = nullptr;
			RECT				   m_toolbarBox				 = {};
			std::unique_ptr<IItem> m_items[(int)Item::Count] = {};
		}; // class Toolbar

	public:
		Editor(Window* window, EventSystem* eventSystem);
		~Editor();

	public:
		static UINT GetMainMenuAsIntResource() noexcept;

		void SetCurrentShape(uint32_t id);
		void OnMouseButton(const WindowMouseButtonEvent& e);
		void Draw();

	private:
		void _CtorInitShapeEditors();
		void _CtorInitEventListeners();
		void _CtorCreateToolbar();

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
		std::unique_ptr<Toolbar>		 m_toolbar				= nullptr;
		IShapeEditor*					 m_shapeEditors[(int)_ShapeType::Count] = {};
		std::array<IShape*, 131>		 m_shapes;
	}; // class Editor
} // namespace lab
#endif // !LAB2_EDITOR_HEADER