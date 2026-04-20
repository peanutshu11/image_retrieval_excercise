#pragma once
#include <opencv2/opencv.hpp>
#include <fstream>

class Feature
{
public:
	virtual double distance(Feature* other) = 0;
	virtual void save(std::string path) = 0;
};

