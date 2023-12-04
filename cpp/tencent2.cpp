#include <cmath>
#include <iostream>
#include <algorithm>
#include <vector>

using namespace std;

bool myfunction(size_t i, size_t j) { return (i > j); }

int main(int argc, char **argv)
{

     size_t case_count;
     cin >> case_count;
     size_t col_count, rank_count;
     size_t num;

     while (case_count--)
     {
          std::vector<int> vecA;
          std::vector<int> vecB;
          cin >> col_count >> rank_count;
          vector<vector<int>> vec;
          vector<int> vecRank;
          while (rank_count--)
          {

               size_t first_count;
               cin >> first_count;
               while (first_count--)
               {
                    cin >> num;
                    vecRank.push_back(num);
               }
          }
          vec.push_back(vecRank);
          std::vector<int> v;
          std::vector<int> v_intersection = vec[0];
          for (int i = 1; i < vec.size(); i++)
          {
               v = v_intersection;
               std::set_intersection(vec[i + 1].begin(), vec[i + 1].end(),
                                     v.begin(), v.end(),
                                     std::back_inserter(v_intersection));
          }
          //  cout <<v_intersection.size() <<  endl;
          // vector<int> vec(vecA);
          // vec[0] = vec[0] + vecB[0];
          // size_t tencentVal = vec[0];

          // std::sort (vec.begin(), vec.begin()+num_count,myfunction);

          // if (tencentVal  >= vec[rank_limit-1]){
          //     cout << "Yes" << endl;
          // }else{
          //      cout << "No" << endl;
          // }
          if (v_intersection.size() > 0)
          {
               cout << "Kelly" << endl;
          }
          else
          {
               cout << "Nacho" << endl;
          }
     }

     return 0;
}