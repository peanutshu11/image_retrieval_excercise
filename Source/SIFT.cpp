#include "stdafx.h"
#include "SIFT.h"

double SIFT::distance(Feature* other) {
    SIFT* s = dynamic_cast<SIFT*>(other);
    if (!s || descriptors.empty() || s->descriptors.empty()) return DBL_MAX;

    cv::Mat desc1 = descriptors;
    cv::Mat desc2 = s->descriptors;
    if (desc1.type() != CV_32F) desc1.convertTo(desc1, CV_32F);
    if (desc2.type() != CV_32F) desc2.convertTo(desc2, CV_32F);

    cv::BFMatcher matcher(cv::NORM_L2, false);
    std::vector<cv::DMatch> matches;
    matcher.match(desc1, desc2, matches);

    double sum = 0;
    for (const auto& m : matches)
        sum += m.distance;

    return matches.empty() ? DBL_MAX : sum / matches.size();
}

void SIFT::save(std::string filename) {
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
