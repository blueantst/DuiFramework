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

	protected:
		EventListener listener_;
	};
}