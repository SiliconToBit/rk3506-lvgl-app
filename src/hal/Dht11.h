#ifndef DHT11_HPP
#define DHT11_HPP

#include <ctime>
#include <string>

class Dht11
{
  private:
    std::string devPath;
    int fd;
    int lastTemp;
    int lastHumi;
    time_t lastReadTime;

    void updateData();

  public:
    Dht11(std::string path);
    ~Dht11();

    bool openDevice();
    void closeDevice();

    // 读取温度
    int readTemperature();
    // 读取湿度
    int readHumidity();
};

#endif // DHT11_HPP
