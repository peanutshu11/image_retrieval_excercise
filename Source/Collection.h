#pragma once
#include "Creator.h"
#include "Kmeans.h"
#include <filesystem>

namespace fs = std::filesystem;

class Collection
{
private:
	std::string location;
	Creator* creator;
	Kmeans* clusters;
	std::vector<cv::Mat> images;
public:
	Collection(Creator* creator, std::string folderPath, std::string name, int k);
	void save();
};

