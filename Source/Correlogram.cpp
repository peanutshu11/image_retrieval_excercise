#include "stdafx.h"
#include "Correlogram.h"

double Correlogram::distance(Feature* other) {
    auto* o = dynamic_cast<Correlogram*>(other);
    if (!o) return DBL_MAX;
    return cv::norm(correlogram, o->getCorrelogram(), cv::NORM_L2);
}
void Correlogram::save(std::string filename) {
    std::ofstream ofs(filename, std::ios::binary);
    int rows = correlogram.rows;
    int cols = correlogram.cols;
    int type = correlogram.type();
    ofs.write((char*)&rows, sizeof(int));
    ofs.write((char*)&cols, sizeof(int));
    ofs.write((char*)&type, sizeof(int));
    size_t dataSize = correlogram.total() * correlogram.elemSize();
    ofs.write((char*)correlogram.data, dataSize);
    ofs.close();
}
