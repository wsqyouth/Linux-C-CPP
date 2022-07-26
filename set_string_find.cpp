
#include <iostream>
#include <set>
#include <string>
using namespace std;
int main() {
	std::set<string> allow_post_set = {
		"pre_auth_check/get",
		"inquiry/get",
		"keywords_lock_conflict/get",
    "inquiry_execute/get",
    "inquiry_record/get",
	};
	string method = "inquiry_execute/get";
	if(allow_post_set.find(method) !=allow_post_set.end()) {
		cout << " found " << endl;
	} else {
		cout << "not  found " << endl;
	}
	return 0;
}
