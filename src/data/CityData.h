/**
 * @file CityData.h
 * @brief 中国城市数据
 * @details 包含全国省市区三级联动数据
 */

#ifndef LVGL_APP_DATA_CITY_DATA_H
#define LVGL_APP_DATA_CITY_DATA_H

#include <string>
#include <vector>

namespace CityData
{

    /**
     * @brief 设置CSV文件路径
     * @param path CSV文件路径
     */
    void setCSVPath(const std::string& path);

    /**
     * @brief 区县
     */
    struct County
    {
        std::string name;
        std::string code; // 天气API用的城市代码
    };

    /**
     * @brief 城市
     */
    struct City
    {
        std::string name;
        std::vector<County> counties;
    };

    /**
     * @brief 省份
     */
    struct Province
    {
        std::string name;
        std::vector<City> cities;
    };

    /**
     * @brief 获取所有省份数据
     * @return const std::vector<Province>& 省份列表
     */
    const std::vector<Province>& getProvinces();

    /**
     * @brief 获取省份名称列表（用于滚轮）
     * @return std::string 以\n分隔的省份名称
     */
    std::string getProvinceNames();

    /**
     * @brief 根据省份索引获取城市名称列表
     * @param provIndex 省份索引
     * @return std::string 以\n分隔的城市名称
     */
    std::string getCityNames(int provIndex);

    /**
     * @brief 根据省份和城市索引获取区县名称列表
     * @param provIndex 省份索引
     * @param cityIndex 城市索引
     * @return std::string 以\n分隔的区县名称
     */
    std::string getCountyNames(int provIndex, int cityIndex);

    /**
     * @brief 获取选中的城市完整信息
     * @param provIndex 省份索引
     * @param cityIndex 城市索引
     * @param countyIndex 区县索引
     * @return std::string 格式：省/市/区
     */
    std::string getSelectedCityName(int provIndex, int cityIndex, int countyIndex);

    /**
     * @brief 获取选中的城市代码（用于天气API）
     * @param provIndex 省份索引
     * @param cityIndex 城市索引
     * @param countyIndex 区县索引
     * @return std::string 城市代码
     */
    std::string getSelectedCityCode(int provIndex, int cityIndex, int countyIndex);

} // namespace CityData

#endif // LVGL_APP_DATA_CITY_DATA_H
