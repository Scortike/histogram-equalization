#pragma once

#include "Image.h"

struct Rect {
    size_t x, y, width, height;
};

void doHistogramEqualization(Image& image, const Rect& roi);

