#include <chrono>
#include <iostream>
int main()
{
    // 根据时钟得到现在时间
    std::chrono::system_clock::time_point today = std::chrono::system_clock::now();
    std::time_t time_t_today = std::chrono::system_clock::to_time_t(today);
    std::cout <<  "now time stamp is " << time_t_today << std::endl;
    std::cout <<  "now time is " << ctime(&time_t_today) << std::endl;

    // 时间戳转换为time_point
    std::chrono::system_clock::time_point time_point_today = std::chrono::system_clock::from_time_t(1662281926);
    std::time_t time_t_today_new  = std::chrono::system_clock::to_time_t(today);
    std::cout <<  "now time stamp is " << time_t_today_new << std::endl;

    return 0;
}


/*
 *system_clock这个类被称为系统内时钟，当修改系统时钟时可能会改变其单调递增的性质，静态成员函数有 now()、to_time_t()、from_time_t() 三个
 */
