#include <cmath>
#include <iostream>
#include <algorithm>
#include <vector>

using namespace std;

void printVec(const vector<int> vec)
{
    for (int i = 0; i < vec.size(); i++)
    {
        cout << vec[i] << " ";
    }
    cout << endl;
}

int main(int argc, char **argv)
{

    size_t case_count;
    cin >> case_count;
    size_t num_count, rank_limit;
    size_t num;

    while (case_count--)
    {
        std::vector<int> vec;
        cin >> num_count >> rank_limit;

        size_t n = num_count;
        while (n--)
        {
            cin >> num;
            vec.push_back(num);
        }
        vector<int> score(num_count);
        int count = 0;
        int last_index = 0;
        bool found = false;
        do
        {

            int index = count % vec.size();
            for (int i = 0; i < score.size(); i++)
            {
                if (i == vec[index] - 1)
                {
                    score[i]++;
                }
                else
                {
                    score[i] = 0;
                }
                if (score[i] > rank_limit)
                {
                    found = true;
                    last_index = vec[index];
                    break;
                }
            }
            ++count;
        } while (!found);
        cout << (count - 1) << " " << last_index << endl;

        //  cout << rank_limit;
        //  printVec(vecA);

        // std::sort (vec.begin(), vec.begin()+num_count,myfunction);

        // if (tencentVal  >= vec[rank_limit-1]){
        //     cout << "Yes" << endl;
        // }else{
        //      cout << "No" << endl;
        // }
    }
    return 0;
}