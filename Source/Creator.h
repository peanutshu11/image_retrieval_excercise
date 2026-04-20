#pragma once
#include "Feature.h"
#include "ColorHistogram.h"
#include "EdgeHistogram.h"
#include "SIFT.h"
#include "ORB.h"
#include "Correlogram.h"
#include <vector>
#include <filesystem>

namespace fs = std::filesystem;

class Creator
{
public:
	virtual Feature* createFromCluster(const std::vector<Feature*>& cluster) const = 0;
	virtual Feature* readFile(const std::string path) = 0;
	virtual Feature* extract(cv::Mat& image) = 0;
	virtual std::string getType() = 0;
};

class ColorHistogramCreator : public Creator {
public:
	ColorHistogramCreator() {}
	Feature* extract(cv::Mat& image) override;
	Feature* createFromCluster(const std::vector<Feature*>& cluster) const override;
	std::string getType() override { return "Color Histogram"; }
	Feature* readFile(const std::string path) override;
};

class EdgeHistogramCreator : public Creator {
public:
	EdgeHistogramCreator() {}
	Feature* extract(cv::Mat& image) override;
	Feature* createFromCluster(const std::vector<Feature*>& cluster) const override;
	std::string getType() override { return "Edge Histogram"; }
	Feature* readFile(const std::string path) override;
};

class CorrelogramCreator : public Creator {
public:
	CorrelogramCreator() {}
	Feature* extract(cv::Mat& image) override;
	Feature* createFromCluster(const std::vector<Feature*>& cluster) const override;
	std::string getType() override { return "Correlogram"; }
	Feature* readFile(const std::string path) override;
};

class ORBCreator : public Creator {
public:
	ORBCreator() {}
	Feature* extract(cv::Mat& image) override;
	Feature* createFromCluster(const std::vector<Feature*>& cluster) const override;
	std::string getType() override { return "ORB"; }
	Feature* readFile(const std::string path) override;
};

class SIFTCreator : public Creator {
private:
	cv::Mat vocabulary;
public:
	SIFTCreator() {}
	Feature* extract(cv::Mat& image) override;
	Feature* createFromCluster(const std::vector<Feature*>& cluster) const override;
	std::string getType() override { return "SIFT"; }
	Feature* readFile(const std::string path) override;
};