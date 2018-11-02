//C++11新特性：enum class 解决全局冲突问题
#include <iostream>
using namespace std;

enum class OPERATION_CODE : uint16_t{
  SET_CMD_REQ = 0x00,
  QUERY_CMD_REQ = 0x01,
  SET_CMD_RSP = 0x80,
  QUERY_CMD_RSP = 0x81,
  STOP_CTRL_CMD=0xf0,
  FILE_DOWNLOAD_CMD=0xf1,
};




int main() {
	//OPERATION_CODE res = OPERATION_CODE::QUERY_CMD_REQ;
	uint16_t cmd_num = 0xf0;
	OPERATION_CODE res = static_cast<OPERATION_CODE>(cmd_num);
	
 	switch(res) {
		case OPERATION_CODE::SET_CMD_REQ:
			 cout << "set" << endl;
			 break;
		case OPERATION_CODE::QUERY_CMD_REQ:
			 cout << "query" << endl;
			 break;
		case OPERATION_CODE::STOP_CTRL_CMD:
			 cout << "stop" << endl;
			 break;
		default:
			 break;
	}
	return 0;
}
