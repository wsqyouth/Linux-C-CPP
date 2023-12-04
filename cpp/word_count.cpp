#include <iostream>
#include <math.h>
using namespace std;

size_t get_word_count(const string &s)
{
    std::size_t mb_len = 0, elen = 0;
    std::string::const_iterator begin = s.begin(), end = s.end();
    while (begin != end)
    {
        unsigned char c = *begin;
        int n;
        printf("%x", c);
        if ((c & 0x80) == 0)
            n = 1, ++elen;
        else if ((c & 0xE0) == 0xC0)
            n = 2;
        else if ((c & 0xF0) == 0xE0)
            n = 3;
        else if ((c & 0xF8) == 0xF0)
            n = 4;
        else
            throw std::runtime_error("utf8_length: invalid UTF-8");

        if (end - begin < n)
        {
            throw std::runtime_error("utf8_length: string too short");
        }
        for (int i = 1; i < n; ++i)
        {
            if ((begin[i] & 0xC0) != 0x80)
            {
                throw std::runtime_error("utf8_length: expected continuation byte");
            }
        }
        mb_len += 1;
        begin += n;
    }
    cout << elen << endl;
    mb_len -= floor(elen / 2);
    return mb_len;
}

int main()
{
    // string src_str = "严苛成品样板·艺术体验中心·景观示范区，全维实景敬呈，恭迎品鉴";
    string src_str = "··，，";
    cout << "str:" << src_str << endl;
    cout << "size: " << get_word_count(src_str) << endl;
    return 0;
}
