#pragma once
#include "Feature.h"
#include <fstream>

class ColorHistogram : public Feature
{
private:
    cv::Mat histogram;

public:
    ColorHistogram(cv::Mat histogram) {
        this->histogram = histogram.clone();
    }
    double distance(Feature* other) override;
    cv::Mat getHistogram() { return histogram; }
    void save(std::string filename) override;
};