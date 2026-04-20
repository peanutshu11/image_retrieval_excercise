#include "stdafx.h"
#include "ORB.h"

double ORB::distance(Feature* other) {
	ORB* o = dynamic_cast<ORB*>(other);
	if (!o || descriptors.empty() || o->descriptors.empty()) return DBL_MAX;

	// hamming distance
	cv::BFMatcher matcher(cv::NORM_HAMMING);
	std::vector<cv::DMatch> matches;
	matcher.match(descriptors, o->descriptors, matches);

	double sum = 0;
	for (const auto& m : matches)
		sum += m.distance;

	return matches.empty() ? DBL_MAX : sum / matches.size();
}

void ORB::save(std::string filename) {
	std::ofstream ofs(filename, std::ios::binary);
	if (!ofs) throw std::runtime_error("Failed to open file for writing: " + filename);

	int rows = descriptors.rows;
	int cols = descriptors.cols;
	int type = descriptors.type();

	ofs.write((char*)&rows, sizeof(int));
	ofs.write((char*)&cols, sizeof(int));
	ofs.write((char*)&type, sizeof(int));

	size_t dataSize = descriptors.total() * descriptors.elemSize();
	ofs.write((char*)descriptors.data, dataSize);

	ofs.close();
}