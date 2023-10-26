#include <iostream>
#include <cstring>
#include <stdint.h>
#include <stdlib.h>
#include <set>
#include <vector>
#include <string>
#include <cstring>
using namespace std;

int Str2Vector(string str, vector<string> &v, const char *p)
{
  v.clear();
  char *pTmp = strtok(const_cast<char *>(str.c_str()), p);
  while (NULL != pTmp)
  {
    v.push_back(pTmp);
    pTmp = strtok(NULL, p);
  }
  return v.size();
}

int main()
{
  /*
    string data_bizuin = "224324";
    int32_t operate_uid = strtoul(data_bizuin.c_str(), NULL, 10);
    cout<<"mysmooth:  "<< operate_uid << endl;
  */

  /*
  std::set<uint64_t> cid_set;
  cout << cid_set.size() << endl;
  uint64_t cid = 12345;
  if (cid_set.find(cid) == cid_set.end())
  {
    cout << "not found" << endl;
  }
  */
  string str = "Hello,World,How,Are,You";
  vector<string> v;
  const char *p = ",";
  int count = Str2Vector(str, v, p);
  cout << "Total " << count << " substrings:" << endl;
  for (int i = 0; i < count; i++)
  {
    cout << v[i] << endl;
  }
  return 0;
}
