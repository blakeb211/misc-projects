#pragma once
#include "flags.h"
#include <any>
#include <array>
#include <cassert>
#include <cstdio>
#include <filesystem>
#include <fmt\core.h>
#include <fmt\format.h>
#include <fmt\os.h>
#include <string>
//
// GLOBALS
//
//
//
namespace Log
{

namespace Fs = std::filesystem;
inline FILE *fptr;
inline Fs::path log_path{};
/*******************************************************/

//@TODO: make log file save to AppRoot instead of the dir the program gets run from

//@TODO: rewrite using ostream and ostreambuf
//	std::ofstream fout("vector.txt");
//	fout.precision(10);
//
//	std::copy(numbers.begin(), numbers.end(),
//		std::ostream_iterator<double>(fout, "\n"));

// @TODO: remove the need to make a std::any copy of everything
// print arguments to log file and screen
template <class T> inline void write_to_screen_and_disk(const std::string fmt, std::any val)
{
	const auto fmt_ = fmt;
	const auto arg_ = std::any_cast<T>(val);
	if constexpr (Flags::LOG_PRINTS_TO_SCREEN)
		fmt::print(fmt_, arg_);
	fmt::print(fptr, fmt_, arg_);
}

template <typename T> inline void LogPrintOneItem(const T &item)
{
	if constexpr (Flags::USE_ASSERTIONS)
		assert(fptr != nullptr);

	if (typeid(0.1f) == typeid(item))
	{
		write_to_screen_and_disk<float>("{:<8.6f} ", item);
		return;
	}
	if (typeid(const char *) == typeid(std::decay<decltype(item)>::type))
	{
		write_to_screen_and_disk<const char *>("{} ", item);
		return;
	}
	if (typeid(char *) == typeid(item))
	{
		write_to_screen_and_disk<char *>("{} ", item);
		return;
	}
	if (typeid(1) == typeid(item))
	{
		write_to_screen_and_disk<int>("{:<8d} ", item);
		return;
	}
	if (typeid(1.0) == typeid(item))
	{
		write_to_screen_and_disk<double>("{:<8.3f} ", item);
		return;
	}
	if (typeid(1u) == typeid(item))
	{
		write_to_screen_and_disk<unsigned int>("{:<8d} ", item);
		return;
	}
	if (typeid(std::string("a")) == typeid(item))
	{
		write_to_screen_and_disk<std::string>("{} ", item);
		return;
	}
	if (typeid(size_t) == typeid(item))
	{
		write_to_screen_and_disk<size_t>("{:<8d} ", item);
		return;
	}
	//****************************************************************
	// add handling for next type here
	//****************************************************************
	write_to_screen_and_disk<const std::string>("ERROR: add type <{}> to PrintLn", typeid(item).name());
}

// Base case for variadic template
inline void PrintLn()
{
	LogPrintOneItem("\n");
}
// This creates a lambda function and processes each arg of a vararg with it.
template <typename T, typename... Tail> inline void PrintLn(T head, Tail... tail)
{
	LogPrintOneItem(head);
	PrintLn(tail...);
}

inline void LogErr(const std::string fname, const int lineNum, const std::string msg)
{
	PrintLn("ERROR: file <", fname, "> line <", lineNum, "> ==", msg);
}

inline bool CloseLog()
{
	if (fptr != NULL)
	{
		int result = fclose(fptr);
		if (result == 0)
		{
			return true;
		}
		return false;
	}
	return false;
}

inline bool SetLogFile(const Fs::path path)
{
	if (path.string().empty() || Fs::is_directory(path))
	{
		return false;
	}

	fopen_s(&fptr, path.string().c_str(), "w+");

	if (fptr == NULL)
	{
		return false;
	}
	return true;
}

} // namespace Log
