#pragma once
#include "Feature.h"
class ORB : public Feature
{
private:
    cv::Mat descriptors;
public:
    ORB(const cv::Mat& desc) {
        desc.copyTo(descriptors);
    }
    const cv::Mat& getDescriptors() const { return descriptors; }
    double distance(Feature* other) override;
    void save(std::string filename) override;
};

