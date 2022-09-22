#include <iostream>
#include <vector>
#include<algorithm>
using namespace std;

template<typename T>
int StrSplit(T& dst, const string& src, const string& spt)
{
    string::size_type bpos=0, epos=0;
    while( (epos=src.find(spt,bpos)) != string::npos )
    {
        dst.insert(dst.end(), src.substr(bpos,epos-bpos));
        if(src.size() == epos+spt.size())
        {
            bpos=epos;
            break;
        }
        bpos=epos+spt.size();
    }
    dst.insert(dst.end(), src.substr(bpos,epos-bpos));
    return 0;
}

int main()
{
  //string white_list_str=""; // test empty case
  string white_list_str="a,b,c,d";
  vector<string> white_list_vec;
  StrSplit(white_list_vec,white_list_str,",");
  cout << white_list_vec.size() << endl;
  cout << white_list_vec[0] << endl;
  string field="c";
  if (std::find(white_list_vec.begin(), white_list_vec.end(), field) != white_list_vec.end())
   {
     cout << "found " << endl;
   }
  return 0;
}
