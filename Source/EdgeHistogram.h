#pragma once
#include "Feature.h"
class EdgeHistogram : public Feature {
private:
    cv::Mat histogram;

public:
    EdgeHistogram(cv::Mat histogram) {
        this->histogram = histogram.clone();
    }

    double distance(Feature* other) override;
    cv::Mat getHistogram() { return histogram; }
    void save(std::string filename) override;
};

