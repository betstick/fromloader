#include <wchar.h>

namespace cfr
{
	//a simpler string.
	class b_str
	{
		wchar_t* str;
		int len;

		b_str(char* c, int l);
		b_str(wchar_t* c, int l);
	};
};