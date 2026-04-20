#pragma once
#include "Creator.h"
#include <filesystem>
#include <fstream>
#include <algorithm>
#include <random>

namespace fs = std::filesystem;

class Kmeans
{
private:
	int k;
	std::vector<Feature*> objects;
	std::vector<Feature*> centroids;
	std::vector<int> labels;
	Creator* creator = nullptr;
public:
	Kmeans(int k, std::vector<Feature*> objects, Creator* creator);
	void run(int max_iters);
	std::vector<std::string> saveObjects(std::string folder);
	std::vector<int> getLabels() { return labels; }
	std::vector<int> rankCentroid(cv::Mat& image);
	int size() { return centroids.size(); }
};

