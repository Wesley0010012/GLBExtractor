#include "./ImageWriter.hpp"

void ImageWriter::savePNG(const std::string &name, int w, int h)
{
    std::vector<unsigned char> px(w * h * 3);
    glReadPixels(0, 0, w, h, GL_RGB, GL_UNSIGNED_BYTE, px.data());

    std::vector<unsigned char> flip(w * h * 3);
    for (int y = 0; y < h; y++)
        memcpy(&flip[y * w * 3], &px[(h - 1 - y) * w * 3], w * 3);

    stbi_write_png(name.c_str(), w, h, 3, flip.data(), w * 3);
}