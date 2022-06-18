#include <iostream>
#include <vector>
#include <string>
#include <string.h>
#include <stdio.h>
#include <set>
#include <algorithm>
#include <sstream>
#include <sys/types.h>
#include <memory>
#include <unordered_map>
#include<json/json.h>
using namespace std;
vector<string> split(const string& str, const string& delim) {
	vector<string> res;
	if("" == str) return res;
	//先将要切割的字符串从string类型转换为char*类型
	char * strs = new char[str.length() + 1] ;
	//不要忘了
	strcpy(strs, str.c_str());
	char * d = new char[delim.length() + 1];
	strcpy(d, delim.c_str());
	char *p = strtok(strs, d);
	while(p) {
		string s = p;
		//分割得到的字符串转换为string类型
		res.push_back(s);
		//存入结果数组
		p = strtok(NULL, d);
	}
	return res;
}
// cgi与server映射配置
unordered_map<string,string > cgi_server_conf_table;
// meta透明传参,内部包含请求方法的注解
struct MetaParam {
	string method;
	string query_path;
};
// metaData根据meta透明传参获取对应server
struct MetaData {
	string server_name;
	string server_method;
};
// 暂时作为rpc返回的结果结构体
struct RpcData {
	int id;
	string content;
};

////////////////////////////////////////////////////////////////
class ProxyBase {
	public:
	        ProxyBase(string auth_info, string input_data, unordered_map<string,string > cgi_server_conf_table);
	~ProxyBase();
	virtual string Validate(const string& auth_info) {
		cout << "auth info: " << auth_info << endl;
		return "";
	}
	protected:
	        virtual int Process(const string& auth_info,const struct MetaParam& metaParam , const string& input_data);
	std::shared_ptr<RpcData> DoRPC(const string& auth_info, const struct MetaParam& metaParam , const string& input_data);
	int Output(std::shared_ptr<RpcData> pData);
	string m_auth_info;
	string m_input_data;
	unordered_map<string, string> m_cgi_server_conf_table;
};

ProxyBase::ProxyBase(string auth_info, string input_data,  unordered_map<string,string>  cgi_server_conf_table) {
	m_auth_info = auth_info;
	m_input_data = input_data;
	m_cgi_server_conf_table = cgi_server_conf_table;
	cout << "parent constructor" << endl;
}
ProxyBase::~ProxyBase() {
	cout << "parent destructor" << endl;
}
int ProxyBase::Process(const string& auth_info, const struct MetaParam& metaParam , const string& input_data) {
	string key = metaParam.method + " " + metaParam.query_path ;
	cout << key << endl;
	// 获取server实体
	auto it = this->m_cgi_server_conf_table.find(key);
	if (it == this->m_cgi_server_conf_table.end()) {
		cout << "can not find rpc client for key" << key << endl;
		throw ("can not find rpc client for key" + key );
	}
	//auto metatData
	cout << this->m_cgi_server_conf_table[key]<< endl;
	vector<string> rec = split(this->m_cgi_server_conf_table[key],".");
	cout << rec[0] << endl;
	//由此再查找map找到基类的client base
	cout << rec[1] << endl;
	//找到类方法
	auto r = this->DoRPC(auth_info,metaParam, input_data);
	this->Output(r);
}
std::shared_ptr<RpcData> ProxyBase::DoRPC(const string& auth_info,const struct MetaParam& metaParam ,const string& input_data) {
	// 在调用rcp前鉴权,因为虚继承，这里调用的是子类的校验方法
	string err_msg = this->Validate(auth_info);
	if (err_msg != "") {
		cout << "Validate fail, " << err_msg << endl;
	}
	// rpc调用 [这里的client本来是根据input_data获取注入的client,这里先调用父类的rpc]
	//auto r = client->DoRPC(uin, method, &args, timeout, rpc_ret);
	//auto r = this->DoRPC(auth_info,input_data);
	// 返回输出
	auto r = std::shared_ptr<struct RpcData>(new RpcData());
	r->content = input_data;
	return r;
}
int ProxyBase::Output(std::shared_ptr<RpcData> pData) {
	if (pData != NULL) {
		auto r = *pData;
		cout <<"rpc output: "<< r.content << endl;
	}
	return 0;
}
////////////////////////////////////////////////////////////////
class ChildProxy: public ProxyBase {
	public:
	    ChildProxy(string auth_info, string input_data, unordered_map<string,string> cgi_server_conf_table):ProxyBase(auth_info,input_data,cgi_server_conf_table) {
		cout << "child constructor" << endl;
	};
	~ChildProxy() {
		cout << "child destructor" << endl;
	}
	// 子类实现鉴权
	virtual string Validate(const string& auth_info) {
		cout << "child validate: " << auth_info << " pass" <<  endl;
		return "";
	}
	int Process(const string& auth_info,const struct MetaParam& metaParam ,  const string& input_data) {
		cout << "child Process" << endl;
		int ret = ProxyBase::Process(auth_info,metaParam, input_data);
		return ret;
	}
};
////////////////////////////////////////////////////////////////
int main() {
	string auth_info = "child proxy";
	string method = "GET";
	// meta透明传参
	string query_path = "rpc/get_data";
	string input_data = "rpc need process req data";
	struct MetaParam metaParam {
		method,query_path
	};
	cgi_server_conf_table["GET rpc/get_data"] = "greeter.GetData";
	cgi_server_conf_table["POST rpc/set_data"] = "greeter.SetData";
	ChildProxy child(auth_info,input_data,cgi_server_conf_table);
	child.Process(auth_info,metaParam,input_data);
	return 0;
}


//--------------------------
// 说明：
// 最近在学习rpcProxy,看到原作者将虚继承和map用得出神入化，就抽离个demo进行学习，核心思路：
// 1. 父代理类定义接口，子代理类实现方法其中的虚函数，进而运行时调用
// 2. 子类在初始化时将cgi与server映射配置传入，基类在底层根据请求的传参找到对应的client
// 3. client也可以为基类指针，此时业务实现子类，从而又实现了依赖注入
// 4. 可以使用json完成参数传递，这里为了不引人第三方库，写了简易的struct进行参数传递
// 5. 这个代码可以再完善，总体思路就是配置文件通过构造函数传递，其他业务数据+meta数据传参
// 命令：g++ rpc_proxy.cpp -std=c++11  && ./a.out
