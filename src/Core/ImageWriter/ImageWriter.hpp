#pragma once

#include "../../Config/MainConfig.hpp"

class ImageWriter {
public:
    static void savePNG(const std::string& name, int w, int h);
};