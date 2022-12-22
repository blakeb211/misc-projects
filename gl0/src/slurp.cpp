#include "headers.h" 
#include "log.h"
#include <streambuf>
using namespace std;

namespace slurp
{

stringstream GetFileContents(const std::string filename)
{
	try
	{
		ifstream t(filename);
		string* str_ptr = new string;
		t.seekg(0, ios::end);
		str_ptr->reserve(t.tellg());
		t.seekg(0, ios::beg);

		str_ptr->assign((istreambuf_iterator<char>(t)), istreambuf_iterator<char>());
		stringstream *ss = new stringstream;
		ss->str(*str_ptr);
		delete str_ptr;
		return stringstream(std::move(*ss));
	}
	catch (...)
	{
		Log::PrintLn("Error: GetFileContents() failed with argument:", filename);
	}
	return stringstream{""};
}

// takes a and a name and returns true if it exists
bool CheckFileExist(const string path, const string fname, const string ext)
{
	return filesystem::exists(path + fname + "." + ext);
}

} // namespace slurp
