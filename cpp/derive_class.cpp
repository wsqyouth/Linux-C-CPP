#include <iostream>
#include <cstdint>
using namespace std;
// Mocking your CgiInput and CgiOutput classes
class CgiInput
{
};
class CgiOutput
{
};

class SkeyCgi
{
public:
    SkeyCgi(int argc, char *argv[], const uint32_t plugin_id = 0) {}
    virtual ~SkeyCgi() {}

    int CheckMPAccountOpenAdqByAppid(const string &appid, uint32_t &adq_type, uint32_t &adq_uid)
    {
        // print InitCgiConfig
        cout << m_gdt_cmdid << m_gdt_modid << m_gdt_key << endl;
        
        // Add your implementation here
        std::cout << "SkeyCgi::genTypeAndStr called with appid: " << appid << std::endl;
        adq_type = 123; // example value
        adq_uid = 456;  // example
        return 0;
    }

    void InitCgiConfig(const uint32_t gdt_modid, const uint32_t gdt_cmdid, const std::string& gdt_key) {
        m_gdt_modid = gdt_modid;
        m_gdt_cmdid = gdt_cmdid;
        m_gdt_key = gdt_key;
    }
protected:
    virtual int BeforeProcess(CgiInput *poInput, CgiOutput *poOutput)
    {
        std::cout << "SkeyCgi::BeforeProcess called" << std::endl;
        return 0;
    }

private:
   private:
    uint32_t m_gdt_modid = 0;
    uint32_t m_gdt_cmdid = 0;
    string m_gdt_key;
};

class AdvertiserIndex : public SkeyCgi
{
public:
    AdvertiserIndex(int argc, char *argv[]) : SkeyCgi(argc, argv) {
        InitCgiConfig(1, 2, "example_key"); // Initialize the base class members with the desired values
    }
    ~AdvertiserIndex(void) {}

    // 不能在类的外部直接调用它的保护（protected）或私有（private）方法
    // 只能在类的内部或在派生类中调用这些方法。
    int Process(CgiInput *poInput, CgiOutput *poOutput)
    {
        
        BeforeProcess(poInput, poOutput);
        std::string appid = "example_appid";
        uint32_t adq_type, adq_uid;
        int iRet = CheckMPAccountOpenAdqByAppid(appid, adq_type, adq_uid);
        if (iRet != 0)
        {
            return iRet;
        }
        std::cout << "In AdvertiserIndex::Process, adq_type: " << adq_type << ", adq_uid: " << adq_uid << std::endl;
        return 0;
    }

protected:
    virtual int BeforeProcess(CgiInput *poInput, CgiOutput *poOutput) override
    {
        int iRet = SkeyCgi::BeforeProcess(poInput, poOutput);

        if (iRet != 0)
        {
            return iRet;
        }

        std::cout << "AdvertiserIndex::BeforeProcess called" << std::endl;

        return 0;
    }

    // Other methods...
};

int main(int argc, char *argv[])
{
    AdvertiserIndex ai(argc, argv);
    CgiInput input;
    CgiOutput output;
    ai.Process(&input, &output);
    return 0;
}