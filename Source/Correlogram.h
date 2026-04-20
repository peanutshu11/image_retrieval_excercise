#pragma once
#include "Feature.h"

class Correlogram : public Feature {
private:
    cv::Mat correlogram;

public:
    Correlogram(cv::Mat correlogram) {
        this->correlogram = correlogram.clone();
    }

    double distance(Feature* other) override;
    cv::Mat getCorrelogram() { return correlogram; }
    void save(std::string filename) override;
};

