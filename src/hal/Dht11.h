#ifndef LVGL_APP_HAL_DHT11_H
#define LVGL_APP_HAL_DHT11_H

#include <ctime>
#include <string>

class Dht11
{
  private:
    std::string m_devPath;
    int m_fd;
    int m_lastTemp;
    int m_lastHumi;
    time_t m_lastReadTime;

    void updateData();

  public:
    explicit Dht11(const std::string &path);
    ~Dht11();

    bool open();
    void close();
    int readTemperature();
    int readHumidity();
};

#endif // LVGL_APP_HAL_DHT11_H
