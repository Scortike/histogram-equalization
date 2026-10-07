#include "Image.h"
#include "ImageProcessing.h"
#include "jpeg_decoder.h"

#include <fstream>
#include <iostream>
#include <algorithm>
#include <cstdint>

constexpr float normalize_factor = 1.0f / 255.0f;

Image::Image(const std::string& path) {
    auto getFileExtension = [](const std::string& path) -> std::string {
        auto dotPos = path.rfind(".");
        std::string result = dotPos == std::string::npos ? "" : path.substr(dotPos + 1);
        std::transform(result.begin(), result.end(), result.begin(), ::tolower);
        return result;
    };
    
    std::string extension = getFileExtension(path);

    if (extension == "jpg" || extension == "jpeg") {
        std::ifstream f(path, std::ifstream::binary);
        if (!f.is_open()) {
            printf("Error opening the input file.\n");
            return;
        }
        f.seekg (0, std::ios::end);
        size_t size = f.tellg();

        std::vector<unsigned char> buf(size);

        f.seekg (0, std::ios::beg);
        f.read(reinterpret_cast<char*>(&buf[0]), size);

        Jpeg::Decoder decoder(&buf[0], size);
        if (decoder.GetResult() != Jpeg::Decoder::OK) {
            printf("Error decoding the input file\n");
            return;
        }

        if (decoder.IsColor()) {
            _width = decoder.GetWidth();
            _height = decoder.GetHeight();
            _data.r.resize(area());
            _data.g.resize(area());
            _data.b.resize(area());
            std::fill_n(_data.r.begin(), area(), 0.0f);
            std::fill_n(_data.g.begin(), area(), 0.0f);
            std::fill_n(_data.b.begin(), area(), 0.0f);

            for (size_t y = 0; y < height(); ++y) {
                for (size_t x = 0; x < width(); ++x) {
                    auto jpgPixel = decoder.GetImage() + (x + width() * y) * 3;
                    _data.r[_width * y + x] = jpgPixel[0] * normalize_factor;
                    _data.g[_width * y + x] = jpgPixel[1] * normalize_factor;
                    _data.b[_width * y + x] = jpgPixel[2] * normalize_factor;
                }
            }
        }
    }
    else if (extension == "ppm") {
        std::ifstream file(path, std::ifstream::binary);
        if (!file.is_open()) {
            std::cerr << "Error opening the input file \"" << path << "\"" << std::endl;
            return;
        }
        char buf[8]{};
        file.read(buf, 3);
        if (std::string(buf) == "P6\n") {
            std::fill(std::begin(buf), std::end(buf), 0);
            size_t w{}, h{};
            file >> w;
            file.ignore(1);
            file >> h;
            file.ignore(2);

            _width = w;
            _height = h;
            _data.r.resize(area());
            _data.g.resize(area());
            _data.b.resize(area());
            std::fill_n(_data.r.begin(), area(), 0.0f);
            std::fill_n(_data.g.begin(), area(), 0.0f);
            std::fill_n(_data.b.begin(), area(), 0.0f);

            for (size_t y = 0; y < height(); ++y) {
                for (size_t x = 0; x < width(); ++x) {
                    file.read(buf, 3);
                    _data.r[_width * y + x] = buf[0] * normalize_factor;
                    _data.g[_width * y + x] = buf[1] * normalize_factor;
                    _data.b[_width * y + x] = buf[2] * normalize_factor;
                }
            }
        }
    }
}

void Image::writeToFile(const std::string& path) {
    size_t data_size = width() * height() * 3;
    std::vector<uint8_t> pixel_data(data_size);

    for (size_t y = 0; y < height(); ++y) {
        for (size_t x = 0; x < width(); ++x) {
            auto pixel = getPixel(x, y);
            size_t offset = (x + width() * y) * 3;
            pixel_data[offset + 0] = pixel.r * 255;
            pixel_data[offset + 1] = pixel.g * 255;
            pixel_data[offset + 2] = pixel.b * 255;
        }
    }

    std::ofstream file(path, std::ofstream::binary | std::ofstream::trunc);
    if (!file.is_open()) {
        std::cerr << "Error opening the output file \"" << path << "\"" << std::endl;
        return;
    }

    file << "P6\n"
         << static_cast<int>(width()) << " " << static_cast<int>(height()) << "\n"
         << "255\n";
    file.write(reinterpret_cast<const char*>(pixel_data.data()), data_size);
}
