//
// ForEveryFile
// (c) 2015 Andrzej Budzanowski
//

#include <iostream>
#include <string>
#include <vector>

#define VC_EXTRALEAN
#include <Windows.h>
#include <Shellapi.h>

bool run_for_files = false,
	 run_for_directories = false,
	 run_recursively = false,
	 dry_run = false;
std::wstring exec_str;

namespace
{
	template < typename T >
		struct iterable
		{
			using type = T;

			type _first,
				 _last;

			iterable(type first, type last)
				: _first(first), _last(last)
			{
			}

			type begin()
			{
				return _first;
			}

			type end()
			{
				return _last;
			}
		};

	template < typename T >
		iterable<T> make_iterable(T obj)
		{
			return iterable<T>(std::begin(obj), std::end(obj));
		}

	template < typename T >
		iterable<T> make_iterable(T begin, T end)
		{
			return iterable<T>(begin, end);
		}
}

namespace
{
	template < typename T >
	bool replace(std::basic_string<T> & str, const std::basic_string<T> & what,
				 const std::basic_string<T> & to)
	{
		unsigned start_pos = str.find(what);

		if (start_pos == std::basic_string<T>::npos)
			return false;

		str.replace(start_pos, what.length(), to);
		return true;
	}

	template < typename T >
	void replace_all(std::basic_string<T> & str, const std::basic_string<T> & what,
					 const std::basic_string<T> & to)
	{
		if (what.empty())
			return;

		size_t start_pos = 0;

		while ((start_pos = str.find(what, start_pos)) != std::basic_string<T>::npos)
		{
			str.replace(start_pos, what.length(), to);
			start_pos += to.length(); // In case 'to' contains 'from', like replacing 'x' with 'yx'
		}
	}
}

namespace
{
	std::wstring extract_file_name(const std::wstring & path)
	{
		if (path.empty())
			return path;

		// szukamy ostatniego '\'
		unsigned u = path.rfind(L'\\');
		if (u == path.npos)
			return path;

		return path.substr(u + 1);
	}

	std::wstring extract_file_base(const std::wstring & path)
	{
		auto np = extract_file_name(path);

		if (np.empty())
			return np;

		unsigned u = np.rfind(L'.');
		if (u == np.npos)
			return np;

		return np.substr(0, u);
	}

	std::wstring extract_path(const std::wstring & path)
	{
		if (path.empty())
			return path;

		// szukamy ostatniego '\'
		unsigned u = path.rfind(L'\\');
		if (u == path.npos)
			return path;

		return path.substr(0, u);
	}
}

void show_help()
{
	std::wcout << L"usage:\n"
			L" fef [opts] command\n"
			L" where opts are:\n"
			L"  -f - run command only for files\n"
			L"  -d - run command only for directories\n"
			L"  -r - recursive\n"
			L"  -u - dry run (command is printed not executed)\n"
			L" where command can use this variables:\n"
			L"  ${fname} - file name\n"
			L"  ${fbase} - file name without ext\n"
			L"  ${fpath} - full file path\n"
			L"  ${path}  - file path without file name" << std::endl;
}

void run_command(const std::wstring & path)
{
	std::wstring estr = exec_str;

	if (estr.find(L"${fname}") != estr.npos)
		replace_all<wchar_t>(estr, L"${fname}", extract_file_name(path));
	if (estr.find(L"${fbase}") != estr.npos)
		replace_all<wchar_t>(estr, L"${fbase}", extract_file_base(path));
	if (estr.find(L"${fpath}") != estr.npos)
		replace_all<wchar_t>(estr, L"${fpath}", path);
	if (estr.find(L"${path}") != estr.npos)
		replace_all<wchar_t>(estr, L"${path}", extract_path(path));

	if (dry_run)
	{
		std::wcout << L"Command to run: " << estr << std::endl;
		return;
	}

	// for some reason CreateProcessW can modify second argument passed to fnc, so
	// we need to copy it to new non-const variable
	wchar_t args_[ 1 << 12 ];
	wcscpy_s(args_, estr.c_str());

	STARTUPINFOW siw = {sizeof (STARTUPINFOW)};
	PROCESS_INFORMATION pi = {};

	BOOL ret = CreateProcessW(nullptr, // application to run
							args_, // parameters
							nullptr, // process attributes
							nullptr, // thread attributes
							true, // inherit handles
							0, // creation flags
							nullptr, // enviroment
							nullptr, // current directory
							&siw, // startup info struct
							&pi // process information struct
							);
	if (!ret)
		std::wcout << L"Error creating process! (" << GetLastError() << ")" << std::endl;
	else
		WaitForSingleObject(pi.hProcess, INFINITE);
}

void run_command_recursively(const std::wstring &path, unsigned rec)
{
	if (!rec)
		return;

	WIN32_FIND_DATAW wfd = {};

	auto fn_path = L"\\\\?\\" + path + L"\\*";

	HANDLE fh = FindFirstFileW(fn_path.c_str(), &wfd);

	if (fh == INVALID_HANDLE_VALUE)
		return;

	do
	{
		if (std::wstring(wfd.cFileName) == L"." || std::wstring(wfd.cFileName) == L"..")
			continue;

		if (wfd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
		{
			// we have directory
			if (run_for_directories)
				run_command(path + L"\\" + wfd.cFileName);

			if (run_recursively)
				run_command_recursively(path + L"\\" + wfd.cFileName, rec - 1);
		} else
		{
			if (run_for_files)
				run_command(path + L"\\" + wfd.cFileName);
		}
	}
	while (FindNextFileW(fh, &wfd));

	FindClose(fh);
}

int wmain(int argc, const wchar_t ** argv)
{
	std::wcout << L"fef\n"
				  L"(c) 2015 Andrzej Budzanowski\n"
				  L"--" << std::endl;

	if (argc == 1)
		return show_help(), 1;

	for (auto it : make_iterable(argv + 1, argv + argc))
		if (it[0] == L'-')
		{
			switch (it[1])
			{
				case L'f':
					run_for_files = true;
					break;

				case L'd':
					run_for_directories = true;
					break;

				case L'r':
					run_recursively = true;
					break;

				case L'u':
					dry_run = true;
					break;
			}
		} else
		{
			if (!exec_str.empty())
				exec_str = exec_str + L" " + it;
			else
				exec_str = it;
		}

	if (!run_for_files && !run_for_directories)
		run_for_files = run_for_directories = true;

	{
		wchar_t buff[1 << 10];
		GetCurrentDirectoryW(1 << 10, buff);
		run_command_recursively(buff, run_recursively ? -1 : 1);
	}

	return 0;
}
