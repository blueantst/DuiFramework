#pragma once
#include "core/constants.h"
#include "render/point.h"
#include <unordered_map>
#include <unordered_set>
#include <functional>

namespace ui
{
	class View;
	class Widget;
	class Event
	{
	public:
		Event(EventType type, View* owner);
		virtual ~Event();

		bool IsMouseEvent() const {
			return type_ == EVENT_MOUSE_DOWN ||
				type_ == EVENT_MOUSE_UP ||
				type_ == EVENT_MOUSE_DBCLICK ||
				type_ == EVENT_MOUSE_MOVE ||
				type_ == EVENT_MOUSE_ENTER ||
				type_ == EVENT_MOUSE_LEAVE ||
				type_ == EVENT_MOUSEWHEEL;
		}

		EventType type() const { return type_; }

		void StopPropagation();
		bool stopped_propagation() const { return !is_propagation_; }

		void SetOwner(View* v);
		View* owner() const;

		void SetSender(View* v);
		View* sender() const;

	protected:
		EventType type_;
		bool is_propagation_{ true }; //�Ƿ����ð��
		View* sender_;//��Ϣ��һ��������
		View* owner_;//��Ϣ������
	};

	typedef std::function<void(Event* evt)> EventAction;
	typedef std::unordered_set<EventAction> EventActionList;
	typedef std::unordered_map<View*, std::unordered_map<EventType, EventAction>>  EventListenMap;

	Point GetMousePosition(Widget* v);
	int GetKeyFlags();
}