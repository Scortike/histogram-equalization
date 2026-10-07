#include "ImageProcessing.h"

#include <vector>
#include <thread>
#include <cassert>
#include <iostream>

inline float getLuminance(const Pixel& p)
{
    return KR * p.r + KG * p.g + KB * p.b;
}   

inline float clamp(float x, float min = 0.0f, float max = 1.0f)
{
    return std::max(min, std::min(max, x));
}

void calculateEqualizImageThreaded(Image& image, const float* equaliz_lum, const Rect& thread_roi)
{
    for (size_t y = thread_roi.y; y < thread_roi.y + thread_roi.height; ++y) {
        for (size_t x = thread_roi.x; x < thread_roi.x + thread_roi.width; ++x) {
            RGB& data = image.getRGB();
            float r = data.r[image.width() * y + x];
            float g = data.g[image.width() * y + x];
            float b = data.b[image.width() * y + x];
            float luminance = getLuminance({r, g, b});
            int bin = luminance * 255;
            float delta = equaliz_lum[bin] - luminance;

            data.r[image.width() * y + x] = clamp(r + delta);
            data.g[image.width() * y + x] = clamp(g + delta);
            data.b[image.width() * y + x] = clamp(b + delta);
        }
    }
}

void calculateHistogramThreaded(Image& image, std::vector<size_t>& local_histogram, const Rect& thread_roi)
{
    for (size_t y = thread_roi.y; y < thread_roi.y + thread_roi.height; ++y) {
        for (size_t x = thread_roi.x; x < thread_roi.x + thread_roi.width; ++x) {
            RGB& data = image.getRGB();
            float r = data.r[image.width() * y + x];
            float g = data.g[image.width() * y + x];
            float b = data.b[image.width() * y + x];
            float luminance = getLuminance({r, g, b});
            int bin = luminance * 255;

            local_histogram[bin]++;
        }
    }
}

void doHistogramEqualization(Image& image, const Rect& roi)
{
    assert(roi.x >= 0);
    assert(roi.x + roi.width <= image.width());
    assert(roi.width > 0);
    assert(roi.y >= 0);
    assert(roi.y + roi.height <= image.height());
    assert(roi.height > 0);

    size_t g_histogram[256] = {0};

    unsigned int numThreads = std::thread::hardware_concurrency();
    if (numThreads == 0) numThreads = 1;

    std::vector<std::thread> threads;
    size_t chunk = roi.height / numThreads;
    std::vector<std::vector<size_t>> local_histograms(numThreads, std::vector<size_t>(256, 0));
    std::vector<Rect> thread_rois(numThreads);

    // Calculate local chunk histograms in parallel
    for (int i = 0; i < numThreads; ++i) {
            size_t startY = roi.y + i * chunk;
            size_t endY = (i == numThreads - 1) ? roi.y + roi.height : startY + chunk;
            thread_rois[i] = {roi.x, startY, roi.width, endY - startY};
            threads.emplace_back(calculateHistogramThreaded, 
                std::ref(image), 
                std::ref(local_histograms[i]), 
                thread_rois[i]);
    }
    for (auto& t : threads) {
            t.join();
    }

    // Combine local histograms into global histogram
    for (const auto& histogram : local_histograms) {
        for (int i = 0; i < 256; ++i) {
            g_histogram[i] += histogram[i];
        }
    }

    // Calculate equalization mapping
    float equaliz_lum[256] = {0.0f};
    float equaliz_factor = 1.0f / (roi.width * roi.height);
    size_t sum = 0;
    for (int i = 0; i < 256; ++i) {
        sum += g_histogram[i];
        equaliz_lum[i] = static_cast<float>(sum) * equaliz_factor;
    }

    // Apply equalization
    threads.clear();
    for (int i = 0; i < numThreads; ++i) {
        threads.emplace_back(calculateEqualizImageThreaded, 
            std::ref(image), 
            equaliz_lum, 
            thread_rois[i]);
    }
    for (auto& t : threads) {
        t.join();
    }
}
