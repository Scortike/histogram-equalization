#include "Test.h"
#include "Image.h"

#include <cmath>
#include <cassert>
#include <algorithm>
#include <iostream>

void compareTwoImages(const std::string& imagePath_1, const std::string& imagePath_2) {
    Image image1(imagePath_1);
    Image image2(imagePath_2);

    assert(image1.width() == image2.width());
    assert(image1.height() == image2.height());

    auto pixDiff = [](const Pixel& p1, const Pixel& p2) {
        return std::max({std::abs(p1.r - p2.r), std::abs(p1.g - p2.g), std::abs(p1.b - p2.b)});
    };

    decltype(image1.getPixel(0, 0).r) maxDiff = 0.0;
    for (size_t y = 0; y < image1.height(); ++y) {
        for (size_t x = 0; x < image1.width(); ++x) {
            auto pix1 = image1.getPixel(x, y);
            auto pix2 = image2.getPixel(x, y);
            auto curDiff = pixDiff(pix1, pix2);
            maxDiff = std::max(curDiff, maxDiff);
        }
    }

    std::cout << "Max difference " << maxDiff << std::endl;
}
