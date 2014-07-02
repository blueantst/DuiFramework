#pragma once

#include "render/size.h"

#include <string>

namespace ui
{
	HMODULE GetModuleFromAddress(void* address);

	void CenterAndSizeWindow(HWND parent, HWND window, const Size& pref);

	bool ReadFileToString(const std::wstring& path,	std::string* contents);

	//������������ϵ����Ļ����ϵһ��
	HBITMAP CreateDIB(int width, int height, void** pBits);
}