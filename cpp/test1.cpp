#include <iostream>
#include <cstring>
using namespace std;
int getStrLenUtf8(const char *str)
{
	if (!str)
		return 0;
	int len = (int)strlen(str);
	int ret = 0;

	for (const char *sptr = str; (sptr - str) < len && *sptr;)
	{
		unsigned char ch = (unsigned char)(*sptr);

		if (ch < 0x80)
		{
			sptr++; // ascii
			ret++;
		}
		else if (ch < 0xc0)
		{
			sptr++; // invalid char
		}
		else if (ch < 0xe0)
		{
			sptr += 2;
			ret++;
		}
		else if (ch < 0xf0)
		{
			sptr += 3;
			ret++;
		}
		else
		{
			// 统一4个字节
			sptr += 4;
			ret++;
		}
	}

	return ret;
}

int main()
{
	string str = "我司12";
	cout << getStrLenUtf8(str.c_str()) << endl;
}