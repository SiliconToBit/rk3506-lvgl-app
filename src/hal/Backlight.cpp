//
// Created by gm on 2025/12/3.
//

#include "Backlight.h"

#include <iostream>
#include <ostream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>



Backlight::Backlight(std::string path) : brightness_path(path + "brightness"), \
                                max_brightness_path(path + "/max_brightness")
{
}

Backlight::~Backlight()
{
}

void Backlight::setBrightness(int brightness)
{
    // Implementation to set the backlight brightness
    FILE* file = fopen(brightness_path.c_str(), "w");
    if (!file)
    {
        std::cerr << "Failed to open file " << brightness_path.c_str() << std::endl;
    }

    fprintf(file, "%d\n", brightness);
    fclose(file);
}

int Backlight::getBrightness() const
{
    // Implementation to get the current backlight brightness
    FILE* file = fopen(brightness_path.c_str(), "w");
    if (!file)
    {
        std::cerr << "Failed to open file " << brightness_path.c_str()<< std::endl;
        return -1;
    }
    int value;
    fscanf(file, "%d", &value);
    fclose(file);
    return value;
}
