#pragma once
#include <opencv2/opencv.hpp>
#include <Feature.h>
#include <Creator.h>
#include <filesystem>
#include <string>

namespace fs = std::filesystem;

class Retriever
{
private:
	std::string path;
	Creator* creator;
	std::vector<Feature*> centroids;
	int size;
public:
	Retriever(std::string path, Creator* creator, int size);
	std::vector<cv::Mat> retrieve(cv::Mat &image, int n = 3);
	~Retriever() {
		for (auto& i : centroids) {
			delete i;
		}
		delete creator;
	}
};

