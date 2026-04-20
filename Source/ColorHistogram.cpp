#include "stdafx.h"
#include "ColorHistogram.h"

double ColorHistogram::distance(Feature* other) {
	ColorHistogram* otherPointer = dynamic_cast<ColorHistogram*>(other);
	cv::Mat otherHistogram = otherPointer->getHistogram();
	return (1.0 - cv::compareHist(this->histogram, otherHistogram, cv::HISTCMP_CORREL)) / 2;
}

void ColorHistogram::save(std::string filename) {
	std::ofstream ofs(filename, std::ios::binary);
	int rows = histogram.rows;
	int cols = histogram.cols;
	int type = histogram.type();
	ofs.write((char*)&rows, sizeof(int));
	ofs.write((char*)&cols, sizeof(int));
	ofs.write((char*)&type, sizeof(int));
	size_t dataSize = histogram.total() * histogram.elemSize();
	ofs.write((char*)histogram.data, dataSize);
	ofs.close();
}