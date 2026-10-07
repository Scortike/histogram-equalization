#pragma once

#include <vector>
#include <string>

#define STANDARD BT601

namespace BT601 {
    static constexpr float kr = 0.299f;
    static constexpr float kg = 0.587f;
    static constexpr float kb = 0.114f;
}

constexpr float KR = STANDARD::kr;
constexpr float KG = STANDARD::kg;
constexpr float KB = STANDARD::kb;

struct Pixel {
    float r = 0.0f, g = 0.0f, b = 0.0f;
};

struct RGB {
    std::vector<float> r, g, b;

    RGB() = default;
    RGB(size_t size) : r(size), g(size), b(size) { }
    RGB(size_t width, size_t height) : r(width * height), g(width * height), b(width * height) { }
    Pixel operator[](size_t index) const { return (index < r.size() && index >= 0) ? Pixel{r[index], g[index], b[index]} : Pixel{}; }
};


class Image {
public:
    Image(const std::string& path);  // Support only jpeg format with baseline encoding and ppm p6 binary format
    Image(size_t width, size_t height)
        : _width(width)
        , _height(height)
        , _data(width * height) { }
    Pixel getPixel(size_t x, size_t y) const { return _data[_width * y + x]; }
    RGB& getRGB() { return _data; }
    const RGB& getRGB() const { return _data; }
    size_t width() const { return _width; }
    size_t height() const { return _height; }
    size_t area() const { return _width * _height; }

    void writeToFile(const std::string& path);  // Support only ppm p6 binary format

private:
    size_t _width = 0, _height = 0;
    RGB _data;
};
