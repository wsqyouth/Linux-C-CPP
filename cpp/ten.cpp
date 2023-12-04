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

    vector<int> vec{1, 1, 3, 2, 1};
    int rank_limit = vec.size();
    int limit = 3;
    // printVec(vec);

    vector<int> score(rank_limit);
    size_t count = 0;
    int last_index = 0;
    bool found = false;
    bool no_found = false;
    ;
    do
    {

        size_t index = count % rank_limit;
        for (int i = 0; i < rank_limit; i++)
        {
            if (i == vec[index] - 1)
            {
                score[i]++;
            }
            else
            {
                score[i] = 0;
            }
            if (score[i] > limit)
            {
                found = true;
                last_index = vec[index];
                break;
            }
        }
        ++count;
        if (count > 10000)
        {
            no_found = true;
            break;
        }
    } while (!found && !no_found);

    if (found)
    {
        cout << count - 1 << endl;
        cout << last_index << endl;
    }
    else
    {
        cout << "INF" << endl;
    }

    return 0;
}