#pragma once
#include <string>
#include <windows.h>

namespace FreeMove {
	namespace Utils {

		bool IsElevated();
		std::wstring GetCurrentUserName();
		std::wstring LoadAboutTextFromResource();
		std::wstring GetLocaleCategory();

		// safe UTF8->W string using WinAPI
		std::wstring UTF8ToWString(const std::string& utf8);
		std::string WStringToUTF8(const std::wstring& w);

	} // namespace Utils
} // namespace FreeMove
