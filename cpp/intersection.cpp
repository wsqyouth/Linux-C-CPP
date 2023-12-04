#include <cmath>
#include <iostream>
#include <algorithm>
#include <vector>
// #include <multiset>

using namespace std;

int main(void)
{
    std::vector<int> v1{1, 2, 3, 4, 5, 6, 7, 8};
    std::vector<int> v2{5, 7, 9, 10};

    std::vector<int> v_intersection;

    std::set_intersection(v1.begin(), v1.end(),
                          v2.begin(), v2.end(),
                          std::back_inserter(v_intersection));

    cout << v_intersection.size() << endl;
    return 0;
}