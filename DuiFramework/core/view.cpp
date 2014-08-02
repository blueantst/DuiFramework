#include "stdafx.h"
#include "view.h"

#include <algorithm>
#include <cassert>

#include "core/rectangle.h"
#include "core/image_view.h"
#include "event/event_listen_manager.h"

namespace ui
{

	View::View()
	{

	}

	View::~View()
	{
		//EventListenManager::Default()->RemoveView(this);
	}

	View* View::parent() const
	{
		return parent_;
	}

	View* View::first_child() const
	{
		return first_child_;
	}

	View* View::last_child() const
	{
		return last_child_;
	}

	View* View::prev_sibling() const
	{
		return prev_sibling_;
	}

	View* View::next_sibling() const
	{
		return next_sibling_;
	}

	View* View::root() const
	{
		const View* p = this;
		while (p->parent_)
			p = p->parent_;
		return const_cast<View*>(p);
	}

	void View::GetViews(Views &child_array) const
	{
		for (View* p = first_child_; p != NULL; p = p->next_sibling_)
		{
			child_array.push_back(p);
		}
	}

	int32 View::GetViewCount() const
	{
		return child_count_;
	}

	View* View::AppendTo(View* parent)
	{
		return parent->Append(this);
	}

	View* View::Detach()
	{
		return parent_->Remove(this);
	}

	View* View::Append(View* child)
	{
		return InsertAfter(last_child_, child);
	}

	View* View::Remove(View* child)
	{
		//只允许移除子节点
		if (!child || child->parent_ != this)
		{
			assert(0); //<< "can only remove child node";
			return NULL;
		}

		//更新父子关系
		child->parent_ = NULL;
		if (first_child_ == child)
		{
			first_child_ = child->next_sibling_;
		}
		if (last_child_ == child)
		{
			last_child_ = child->prev_sibling_;
		}

		//更新兄弟关系
		if (child->prev_sibling_)
		{
			child->prev_sibling_->next_sibling_ = child->next_sibling_;
		}

		if (child->next_sibling_)
		{
			child->next_sibling_->prev_sibling_ = child->prev_sibling_;
		}

		child->next_sibling_ = NULL;
		child->prev_sibling_ = NULL;

		child_count_--;

		return child;
	}

	View* View::InsertAfter(View* ref, View* child)
	{
		if (is_leaf_view_ || ref == child || child == NULL || (ref && ref->parent_ != this))
		{
			assert(0);
			return NULL;
		}

		//先从原节点移除
		if (child->parent_)
		{
			child->parent_->Remove(child);
		}
		child->parent_ = this;

		child->prev_sibling_ = ref;
		if (ref)
		{//插到中间或最后
			child->next_sibling_ = ref->next_sibling_;
			if (ref->next_sibling_)
			{
				ref->next_sibling_->prev_sibling_ = child;
			}
			ref->next_sibling_ = child;

			assert(last_child_);
			//插到最后了
			if (last_child_->next_sibling_)
			{
				last_child_ = last_child_->next_sibling_;
			}
		}
		else
		{//插到最前
			child->next_sibling_ = first_child_;
			if (first_child_)
			{
				first_child_->prev_sibling_ = child;
			}
			first_child_ = child;

			//原来没有子元素
			if (last_child_ == NULL)
			{
				last_child_ = child;
			}
		}
		child_count_++;

		return child;
	}

	View* View::InsertBefore(View* ref, View* child)
	{
		if (is_leaf_view_ || ref == child || child == NULL || (ref && ref->parent_ != this))
		{
			assert(0);// << "insert before invalid";
			return NULL;
		}

		if (ref)
		{
			return InsertAfter(ref->prev_sibling_, child);
		}
		else
		{
			return InsertAfter(last_child_, child);
		}
	}


	void View::set_leaf_view(bool v)
	{
		is_leaf_view_ = v;
	}


	bool View::HasDescender(View* child, int* step)
	{
		*step = 0;
		for (View* v = child; v; v = v->parent(), (*step)++)
		{
			if (v == this)
				return true;
		}
		return false;
	}

	View* View::GetAncestorTo(View* other)
	{
		View* root_view = root();
		int step_this = 0;
		int step_that = 0;
		if (!root_view->HasDescender(other, &step_that))
			return NULL;

		if (step_that == 0)
			return other;

		if (root_view == this)
			return this;

		root_view->HasDescender(this, &step_this);

		View* view_this = this;
		View* view_that = other;

		if (step_this > step_that) {
			int delta = step_this - step_that;
			while (delta) {
				delta--;
				view_this = view_this->parent_;
			}
		} else if (step_this < step_that) {
			int delta = step_that - step_this;
			while (delta) {
				delta--;
				view_that = view_that->parent_;
			}
		}
		//然后同时向前查找
		while (view_this && view_that && view_this != view_that)
		{
			view_this = view_this->parent_;
			view_that = view_that->parent_;
		}

		if (view_this == view_that)
			return view_this;
		assert(0);
		return NULL;
	}


	View* View::Hittest(const Point& pt)
	{
		//hittest self
		if (!GetLocalBounds().Contains(pt))
			return NULL;

		for (View* p = last_child_; p != NULL; p = p->prev_sibling())
		{
			if (p)
			{
				Point pt_in_child = p->GetTransform().Invert().Apply(pt);
				View* v = p->Hittest(pt_in_child);
				if (v)
					return v;
			}
		}

		return this;
	}

	bool View::Hittest(const Point& pt, Views& views)
	{
		if (!GetLocalBounds().Contains(pt))
			return false;

		bool child_hittested = false;
		for (View* p = last_child_; p != NULL; p = p->prev_sibling())
		{
			if (p)
			{
				child_hittested = Hittest(pt, views) || child_hittested;
			}
		}

		if (!child_hittested)
			views.push_back(this);

		return true;
	}


	//const Widget* View::GetWidget() const
	//{
	//	return parent_ ? parent_->GetWidget() : NULL;
	//}

	//Widget* View::GetWidget()
	//{
	//	return const_cast<Widget*>(const_cast<const View*>(this)->GetWidget());
	//}

	void View::SetBounds(int x, int y, int width, int height)
	{
		SetBoundsRect(Rect(x, y, (std::max)(0, width), (std::max)(0, height)));
	}

	void View::SetBoundsRect(const Rect& bounds)
	{
		if (bounds == bounds_) {
			if (needs_layout_) {
				needs_layout_ = false;
				Layout();
				SchedulePaint();
			}
			return;
		}

		if (visible_) {
			// Paint where the view is currently.
			SchedulePaint();
		}

		Rect prev = bounds_;
		bounds_ = bounds;
		if (prev.size() != size()) {
			needs_layout_ = false;
			Layout();
			SchedulePaint();
		}
	}

	void View::SetSize(const Size& size)
	{
		SetBounds(x(), y(), size.width(), size.height());
	}

	void View::SetPosition(const Point& position)
	{
		SetBounds(position.x(), position.y(), width(), height());
	}

	void View::SetX(int x)
	{
		SetBounds(x, y(), width(), height());
	}

	void View::SetY(int y)
	{
		SetBounds(x(), y, width(), height());
	}

	Rect View::GetLocalBounds() const
	{
		return Rect(width(), height());
	}


	Rect View::GetContentsBounds() const
	{
		if (!border_.get())
			return GetLocalBounds();

		Rect rc(GetLocalBounds());
		rc.Inset(border_->GetPadding());
		return rc;
	}


	void View::SetVisible(bool visible)
	{
		if (visible != visible_) {
			// If the View is currently visible, schedule paint to refresh parent.
			// TODO(beng): not sure we should be doing this if we have a layer.
			if (visible_)
				SchedulePaint();

			visible_ = visible;

			OnVisibleChanged();

			// Notify the parent.
			//if (parent_)
			//	parent_->ChildVisibilityChanged(this);

			// This notifies all sub-views recursively.
			//PropagateVisibilityNotifications(this, visible_);
			//UpdateLayerVisibility();

			// If we are newly visible, schedule paint.
			if (visible_)
				SchedulePaint();
		}
	}

	bool View::IsDrawn() const
	{
		return visible_ && parent_ ? parent_->IsDrawn() : false;
	}

	void View::SetEnabled(bool enabled)
	{
		if (enabled != enabled_) {
			enabled_ = enabled;
			OnEnabledChanged();
		}
	}

	void View::SetLayout(LayoutManager* layout)
	{
		if (layout_manager_.get())
			layout_manager_->Uninstalled(this);

		layout_manager_.reset(layout);
		if (layout_manager_.get())
			layout_manager_->Installed(this);
	}


	void View::Layout()
	{
		needs_layout_ = false;

		LayoutBackground();

		// If we have a layout manager, let it handle the layout for us.
		if (layout_manager_.get())
			layout_manager_->Layout(this);

		// Make sure to propagate the Layout() call to any children that haven't
		// received it yet through the layout manager and need to be laid out. This
		// is needed for the case when the child requires a layout but its bounds
		// weren't changed by the layout manager. If there is no layout manager, we
		// just propagate the Layout() call down the hierarchy, so whoever receives
		// the call can take appropriate action.
		for (View* child = first_child(); child != NULL; child = child->next_sibling()) {
			if (child->needs_layout_ /*|| !layout_manager_.get()*/) {
				child->needs_layout_ = false;
				child->Layout();
			}
		}
	}


	void View::LayoutBackground()
	{
		if (background_.get())
			background_->SetBoundsRect(background_inside_ ? GetContentsBounds()
				: GetLocalBounds());
	}


	Size View::GetPreferredSize() const
	{
		if (layout_manager_.get())
			return layout_manager_->GetPreferredSize(this);
		return Size();
	}

	void View::OnVisibleChanged()
	{

	}

	void View::OnEnabledChanged()
	{

	}


	void View::SchedulePaint()
	{
		SchedulePaintInRect(GetLocalBounds());
	}

	void View::SchedulePaintInRect(const Rect& r)
	{
		if (!visible_ || !painting_enabled_)
			return;

		if (parent_) {
			// Translate the requested paint rect to the parent's coordinate system
			// then pass this notification up to the parent.
			parent_->SchedulePaintInRect(ConvertRectToParent(r));
		}
	}

	void View::DoPaint(Painter* painter)
	{
		if (!visible_)
			return;

		PaintBackground(painter);
		PaintBorder(painter);
		OnPaint(painter);
		OnPaintChildren(painter);
	}

	void View::OnPaint(Painter* painter)
	{
		//painter->DrawStringRect(L"测试", Font(L"微软雅黑", 18), ColorSetRGB(255,0,0,0), GetLocalBounds());
	}


	void View::OnPaintChildren(Painter* painter)
	{
		for (View* p = first_child_; p != NULL; p = p->next_sibling())
		{
			if (p)
			{
				ScopedPainter helper(painter,
					p->GetTransform());
				p->DoPaint(painter);
			}
		}
	}


	Transform View::GetTransform() const
	{
		return Transform(1.0, 0, 0, 1.0, x(), y());
	}


	bool View::GetTransformRelativeTo(const View* ancestor, Transform* transform) const
	{
		const View* p = this;

		while (p && p != ancestor) {
			transform->ConcatTransform(p->GetTransform());
			p = p->parent_;
		}

		return p == ancestor;
	}


	/*void View::ConvertPointToView(const View* source, const View* target, Point* point)
	{
		if (source == target)
			return;

		// |source| can be NULL.
		const View* root = target->root();
		if (source) {
			assert(source->root() == root);

			if (source != root)
				source->ConvertPointForAncestor(root, point);
		}

		if (target != root)
			target->ConvertPointFromAncestor(root, point);

		// API defines NULL |source| as returning the point in screen coordinates.
		if (!source) {
			*point = point->Subtract(
				root->GetWidget()->GetClientAreaScreenBounds().origin());
		}
	}

	void View::ConvertPointToWidget(const View* src, Point* point)
	{

	}

	void View::ConvertPointFromWidget(const View* dest, Point* p)
	{

	}

	void View::ConvertPointToScreen(const View* src, Point* point)
	{

	}

	void View::ConvertPointFromScreen(const View* dst, Point* point)
	{

	}*/

	Point View::ConvertPointFromWidget(const Point& pt) const
	{
		Views vs;
		for (const View* v = this; v; v = v->parent())
			vs.push_back(const_cast<View*>(v));

		Point x_pt = pt;
		for (View* v : vs) {
			x_pt = v->ConvertPointFromParent(x_pt);
		}
		return x_pt;
	}

	Point View::ConvertPointFromParent(const Point& pt) const
	{
		return GetTransform().Invert().Apply(pt);
	}

	Point View::ConvertPointToParent(const Point& pt) const
	{
		return GetTransform().Apply(pt);
	}

	Point View::ConvertPointToWidget(const Point& pt) const
	{
		Point x_pt = pt;
		for (const View* v = this; v; v = v->parent())
			x_pt = v->ConvertPointToParent(x_pt);
		return x_pt;
	}

	Rect View::ConvertRectToParent(const Rect& rect) const
	{
		return GetTransform().Apply(rect);
	}

	Rect View::ConvertRectToWidget(const Rect& rect) const
	{
		Rect x_rect = rect;
		for (const View* v = this; v; v = v->parent())
			x_rect = v->ConvertRectToParent(x_rect);
		return x_rect;
	}

	void View::PaintBackground(Painter* painter)
	{
		if (!background_.get())
			return;

		background_->DoPaint(painter);
	}

	void View::SetBorder(Border* border)
	{
		border_.reset(border);
	}

	Border* View::border() const
	{
		return border_.get();
	}

	void View::PaintBorder(Painter* painter)
	{
		if (!border_.get())
			return;

		border_->DoPaint(this, painter);
	}

	void View::SetBackground(View* background)
	{
		background_.reset(background);
		background_->parent_ = this;
		LayoutBackground();
	}

	void View::set_background_color(Color color)
	{
		if (!background_.get()) {
			background_.reset(new SolidRectangle(color));
			LayoutBackground();
		}

		SolidRectangle* normal_background
			= dynamic_cast<SolidRectangle*>(background_.get());
		if (!normal_background)
			return;

		normal_background->SetColor(color);
	}

	void View::set_background_image_id(const std::string& id)
	{
		if (id.empty())
			return;

		if (!background_.get()) {
			background_.reset(new ResourceImage(id));
			LayoutBackground();
		}

		ResourceImage* normal_background
			= dynamic_cast<ResourceImage*>(background_.get());
		if (!normal_background)
			return;

		normal_background->SetImageId(id);
	}


	void View::set_background_inside(bool v)
	{
		background_inside_ = v;
		LayoutBackground();
	}



	void View::SetCursor(HCURSOR cursor)
	{
		cursor_ = cursor;
	}


	HCURSOR View::GetCursor()
	{
		return cursor_ ? cursor_ : parent_->GetCursor();
	}

	

	void View::ConvertPointToTarget(View* source, View* target, Point* pt)
	{
		if (source == target)
			return;

		View* ancestor = source->GetAncestorTo(target);

		source->ConvertPointForAncestor(ancestor, pt);
		target->ConvertPointFromAncestor(ancestor, pt);
	}

	

	

	

	

	bool View::ConvertPointForAncestor(const View* ancestor, Point* point) const
	{
		Transform trans;
		bool result = GetTransformRelativeTo(ancestor, &trans);
		trans.TransformPoint(*point);
		return result;
	}

	bool View::ConvertPointFromAncestor(const View* ancestor, Point* point) const
	{
		Transform trans;
		bool result = GetTransformRelativeTo(ancestor, &trans);
		trans.Invert().TransformPoint(*point);
		return result;
	}

// 	void View::HandleEvent(Event* event)
// 	{
// 		EventListenManager::Default()->DispatchEvent(this, event);
// 	}

// 	void View::SetEventDelegate(EventDelegate* delegate)
// 	{
// 		event_delegate_.reset(delegate);
// 	}

// 	EventDispatcher* View::GetEventDispatcher() const
// 	{
// 		return parent() ? parent()->GetEventDispatcher() : NULL;
// 	}
// 
// 
// 	void View::DispatchPropagation(Event* event)
// 	{
// 		EventDispatcher* dispatcher = GetEventDispatcher();
// 		if (dispatcher)
// 		{
// 			dispatcher->DispatchPropagation(event, this);
// 		}
// 	}


	void View::SetFocus()
	{
		if (!focusable_)
			return;
		FocusManager* manager = GetFocusManager();
		if (manager) {
			manager->SetFocus(this);
		}
	}

	void View::SetFocusable(bool focusable)
	{
		focusable_ = focusable;
	}

	bool View::IsFocusable() const
	{
		return focusable_;
	}

	FocusManager* View::GetFocusManager() const
	{
		return parent_ ? parent_->GetFocusManager() : NULL;
	}

	void View::SetPropertyBoolean(const std::string& path, bool in_value)
	{
		if (!property_.get())
			property_.reset(new DictionaryValue);

		property_->SetBoolean(path, in_value);
	}

	void View::SetPropertyInteger(const std::string& path, int in_value)
	{
		if (!property_.get())
			property_.reset(new DictionaryValue);

		property_->SetInteger(path, in_value);
	}

	void View::SetPropertyDouble(const std::string& path, double in_value)
	{
		if (!property_.get())
			property_.reset(new DictionaryValue);

		property_->SetDouble(path, in_value);
	}

	void View::SetPropertyString(const std::string& path, const std::string& in_value)
	{
		if (!property_.get())
			property_.reset(new DictionaryValue);

		property_->SetString(path, in_value);
	}

	void View::SetPropertyString(const std::string& path, const std::wstring& in_value)
	{
		if (!property_.get())
			property_.reset(new DictionaryValue);

		property_->SetString(path, in_value);
	}

	bool View::GetPropertyBoolean(const std::string& path, bool* out_value) const
	{
		if (!property_.get())
			return false;

		return property_->GetBoolean(path, out_value);
	}

	bool View::GetPropertyInteger(const std::string& path, int* out_value) const
	{
		if (!property_.get())
			return false;

		return property_->GetInteger(path, out_value);
	}

	bool View::GetPropertyDouble(const std::string& path, double* out_value) const
	{
		if (!property_.get())
			return false;

		return property_->GetDouble(path, out_value);
	}

	bool View::GetPropertyString(const std::string& path, std::string* out_value) const
	{
		if (!property_.get())
			return false;

		return property_->GetString(path, out_value);
	}

	bool View::GetPropertyString(const std::string& path, std::wstring* out_value) const
	{
		if (!property_.get())
			return false;

		return property_->GetString(path, out_value);
	}
#if 0
	void View::set_layout_width_policy(LayoutSizePolicy p)
	{
		width_policy_ = p;
	}

	void View::set_layout_height_policy(LayoutSizePolicy p)
	{
		height_policy_ = p;
	}

	ui::LayoutSizePolicy View::get_layout_width_policy() const
	{
		return width_policy_;
	}

	ui::LayoutSizePolicy View::get_layout_height_policy() const
	{
		return height_policy_;
	}
#endif

	




	

	

}