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
    size_t num_count, rank_limit;
    size_t num;

    while (case_count--)
    {
        std::vector<int> vecA;
        std::vector<int> vecB;
        cin >> num_count >> rank_limit;

        size_t n = num_count;
        while (n--)
        {
            cin >> num;
            vecA.push_back(num);
        }
        n = num_count;
        while (n--)
        {
            cin >> num;
            vecB.push_back(num);
        }

        vector<int> vec(vecA);
        vec[0] = vec[0] + vecB[0];
        size_t tencentVal = vec[0];

        std::sort(vec.begin(), vec.begin() + num_count, myfunction);

        if (tencentVal >= vec[rank_limit - 1])
        {
            cout << "Yes" << endl;
        }
        else
        {
            cout << "No" << endl;
        }
    }
    return 0;
}