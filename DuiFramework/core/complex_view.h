#pragma once
#include "core/view.h"
#include "event/event_listener.h"

namespace ui
{
	//��ʾһ��view�������
	class ComplexView : public View
	{
	public:
		ComplexView();
		virtual ~ComplexView();

		//void Listen(View* subview, EventType type, EventAction action);
		//void Listen(EventType type, EventAction action);
	protected:
		//EventListener listener_;
	};
}