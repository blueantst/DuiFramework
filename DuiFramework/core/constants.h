#pragma once

namespace ui
{

	enum HorizontalAlignment{
		ALIGN_LEFT = 0, 
		ALIGN_CENTER,  
		ALIGN_RIGHT, 
	};

	enum VerticalAlignment{
		ALIGN_TOP = 0,
		ALIGN_VCENTER,
		ALIGN_BOTTOM,
	};

	enum TextStyle{
		TEXT_TOP = 0,
		TEXT_LEFT = 0,
		TEXT_CENTER = 1,
		TEXT_RIGHT = 1 << 1,
		TEXT_VCENTER = 1 << 2,
		TEXT_BOTTOM = 1 << 3,

		//TEXT_WORDBREAK = 1 << 4, //�����ʻ���
		//TEXT_MUTLILINE = 1 << 5, //����, ������
		TEXT_NOCLIP = 1 << 6,//�����вü�


		TEXT_PATH_ELLIPSIS = 1 << 7,//�����м���ʡ�Ժ�
		TEXT_END_ELLIPSIS = 1 << 8,//������β��ʡ�Ժ�
		TEXT_END_WORD_ELLIPSIS = 1 << 9,//������β��ʡ�Ժ�
	};
}