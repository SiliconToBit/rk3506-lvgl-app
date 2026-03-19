/**
 * @file CityData.cpp
 * @brief 中国城市数据实现
 * @details 运行时解析CSV文件
 */

#include "CityData.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>

namespace CityData
{

    // 静态数据存储
    static std::vector<Province> s_provinces{};
    static bool s_initialized{false};
    static std::string s_csvPath{"/root/China-City-List-latest.csv"};

    static void parseCSV()
    {
        std::ifstream file(s_csvPath);
        if (!file.is_open())
        {
            std::cerr << "[CityData] Failed to open CSV file: " << s_csvPath << '\n';
            return;
        }

        std::string line;
        // 跳过第一行版本信息
        std::getline(file, line);
        // 读取表头
        std::getline(file, line);

        std::cout << "[CityData] Parsing CSV file..." << '\n';

        while (std::getline(file, line))
        {
            std::stringstream ss(line);
            std::string cell;
            std::vector<std::string> cells;

            while (std::getline(ss, cell, ','))
            {
                cells.push_back(cell);
            }

            if (cells.size() < 10)
                continue;

            std::string loc_id{cells[0]};
            std::string loc_name{cells[2]};
            std::string adm1_name{cells[7]}; // 省
            std::string adm2_name{cells[9]}; // 市

            // 跳过无效数据
            if (loc_id.empty() || loc_id.find("101") != 0)
                continue;

            // 查找或创建省份
            auto provIt = std::find_if(s_provinces.begin(), s_provinces.end(),
                                       [&adm1_name](const Province& p) { return p.name == adm1_name; });

            if (provIt == s_provinces.end())
            {
                Province prov;
                prov.name = adm1_name;
                s_provinces.push_back(prov);
                provIt = s_provinces.end() - 1;
            }

            // 查找或创建城市
            auto cityIt = std::find_if(provIt->cities.begin(), provIt->cities.end(),
                                       [&adm2_name](const City& c) { return c.name == adm2_name; });

            if (cityIt == provIt->cities.end())
            {
                City city;
                city.name = adm2_name;
                provIt->cities.push_back(city);
                cityIt = provIt->cities.end() - 1;
            }

            // 添加区县
            County county;
            county.name = loc_name;
            county.code = loc_id;
            cityIt->counties.push_back(county);
        }

        file.close();
        std::cout << "[CityData] Parsed " << s_provinces.size() << " provinces" << '\n';
    }

    static void initData()
    {
        if (s_initialized)
            return;

        parseCSV();
        s_initialized = true;
    }

    void setCSVPath(const std::string& path)
    {
        s_csvPath = path;
    }

    const std::vector<Province>& getProvinces()
    {
        initData();
        return s_provinces;
    }

    std::string getProvinceNames()
    {
        initData();
        std::string names;
        for (size_t i = 0; i < s_provinces.size(); ++i)
        {
            if (i > 0)
                names += "\n";
            names += s_provinces[i].name;
        }
        return names;
    }

    std::string getCityNames(int provIndex)
    {
        initData();
        if (provIndex < 0 || provIndex >= (int) s_provinces.size())
            return "";

        const auto& cities = s_provinces[provIndex].cities;
        std::string names;
        for (size_t i = 0; i < cities.size(); ++i)
        {
            if (i > 0)
                names += "\n";
            names += cities[i].name;
        }
        return names;
    }

    std::string getCountyNames(int provIndex, int cityIndex)
    {
        initData();
        if (provIndex < 0 || provIndex >= (int) s_provinces.size())
            return "";

        const auto& cities = s_provinces[provIndex].cities;
        if (cityIndex < 0 || cityIndex >= (int) cities.size())
            return "";

        const auto& counties = cities[cityIndex].counties;
        std::string names;
        for (size_t i = 0; i < counties.size(); ++i)
        {
            if (i > 0)
                names += "\n";
            names += counties[i].name;
        }
        return names;
    }

    std::string getSelectedCityName(int provIndex, int cityIndex, int countyIndex)
    {
        initData();
        if (provIndex < 0 || provIndex >= (int) s_provinces.size())
            return "";

        const auto& prov = s_provinces[provIndex];
        if (cityIndex < 0 || cityIndex >= (int) prov.cities.size())
            return prov.name;

        const auto& city = prov.cities[cityIndex];
        if (countyIndex < 0 || countyIndex >= (int) city.counties.size())
            return prov.name + "/" + city.name;

        return prov.name + "/" + city.name + "/" + city.counties[countyIndex].name;
    }

    std::string getSelectedCityCode(int provIndex, int cityIndex, int countyIndex)
    {
        initData();
        if (provIndex < 0 || provIndex >= (int) s_provinces.size())
            return "";

        const auto& prov = s_provinces[provIndex];
        if (cityIndex < 0 || cityIndex >= (int) prov.cities.size())
            return "";

        const auto& city = prov.cities[cityIndex];
        if (countyIndex < 0 || countyIndex >= (int) city.counties.size())
            return "";

        return city.counties[countyIndex].code;
    }

} // namespace CityData
