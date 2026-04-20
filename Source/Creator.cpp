#include "stdafx.h"
#include "Creator.h"

Feature* ColorHistogramCreator::extract(cv::Mat& image) {
	cv::Mat hsvImg;
	cv::cvtColor(image, hsvImg, cv::COLOR_BGR2HSV);
	int hBins = 12;
	int sBins = 6;
	int histSize[] = { hBins, sBins };
	float hRanges[] = { 0, 180 };
	float sRanges[] = { 0, 256 };
	const float* ranges[] = { hRanges, sRanges };
	int channels[] = { 0, 1 };
	cv::Mat hist;
	cv::calcHist(&hsvImg, 1, channels, cv::Mat(), hist, 2, histSize, ranges, true, false);
	cv::normalize(hist, hist, 0, 1, cv::NORM_MINMAX, -1, cv::Mat());
	return new ColorHistogram(hist);
}

Feature* ColorHistogramCreator::createFromCluster(const std::vector<Feature*>& cluster) const {
	cv::Mat first = static_cast<ColorHistogram*>(cluster[0])->getHistogram();
	cv::Mat sum = cv::Mat::zeros(first.size(), first.type());
	for (const auto& feat : cluster) {
		cv::Mat hist = static_cast<ColorHistogram*>(feat)->getHistogram();
		sum += hist;
	}
	sum /= static_cast<float>(cluster.size());
	return new ColorHistogram(sum);
}

Feature* ColorHistogramCreator::readFile(const std::string path) {
	std::ifstream ifs(path, std::ios::binary);
	if (!ifs.is_open()) {
		return nullptr;
	}
	int rows, cols, type;
	ifs.read((char*)&rows, sizeof(int));
	ifs.read((char*)&cols, sizeof(int));
	ifs.read((char*)&type, sizeof(int));
	cv::Mat histogram(rows, cols, type);
	size_t dataSize = histogram.total() * histogram.elemSize();
	ifs.read((char*)histogram.data, dataSize);
	ifs.close();
	return new ColorHistogram(histogram);
}

Feature* ORBCreator::createFromCluster(const std::vector<Feature*>& cluster) const {
	std::vector<cv::Mat> all_descs;
	all_descs.reserve(cluster.size());
	for (auto* f : cluster) {
		auto* orb = dynamic_cast<ORB*>(f);
		if (orb && !orb->getDescriptors().empty())
			all_descs.push_back(orb->getDescriptors());
	}

	if (all_descs.empty()) return nullptr;

	cv::Mat all;
	cv::vconcat(all_descs, all);
	
	cv::Mat all_32f;
	all.convertTo(all_32f, CV_32F);
	const int num_words = 10;
	cv::Mat labels, centers;
	
	cv::kmeans(all_32f, num_words, labels,
		cv::TermCriteria(cv::TermCriteria::EPS + cv::TermCriteria::COUNT, 50, 1.0),
		1, cv::KMEANS_PP_CENTERS, centers);

	centers.convertTo(centers, CV_8U);
	return new ORB(centers);
}

Feature* ORBCreator::readFile(const std::string path) {
	std::ifstream ifs(path, std::ios::binary);
	if (!ifs.is_open()) {
		return nullptr;
	}

	int rows, cols, type;
	ifs.read((char*)&rows, sizeof(int));
	ifs.read((char*)&cols, sizeof(int));
	ifs.read((char*)&type, sizeof(int));

	cv::Mat desc(rows, cols, type);
	size_t dataSize = desc.total() * desc.elemSize();
	ifs.read((char*)desc.data, dataSize);

	ifs.close();
	return new ORB(desc);
}

Feature* ORBCreator::extract(cv::Mat& image) {
	cv::Ptr<cv::ORB> orb = cv::ORB::create();
	std::vector<cv::KeyPoint> keypoints;
	cv::Mat descriptors;
	orb->detectAndCompute(image, cv::noArray(), keypoints, descriptors);

	if (descriptors.empty()) {
		return new ORB(cv::Mat());
	}
	return new ORB(descriptors);
}

Feature* EdgeHistogramCreator::extract(cv::Mat& image) {
	cv::Mat gray;
	if (image.channels() == 3) {
		cv::cvtColor(image, gray, cv::COLOR_BGR2GRAY);
	}
	else {
		gray = image.clone();
	}

	cv::Mat gradX, gradY;
	cv::Sobel(gray, gradX, CV_32F, 1, 0, 3);
	cv::Sobel(gray, gradY, CV_32F, 0, 1, 3);

	cv::Mat magnitude, angle;
	cv::cartToPolar(gradX, gradY, magnitude, angle, true); 

	int numBins = 18;
	cv::Mat hist = cv::Mat::zeros(1, numBins, CV_32F);
	for (int i = 0; i < angle.rows; ++i) {
		for (int j = 0; j < angle.cols; ++j) {
			float mag = magnitude.at<float>(i, j);
			float ang = angle.at<float>(i, j);
			int bin = static_cast<int>(ang / (360.0f / numBins)) % numBins;
			hist.at<float>(0, bin) += mag;
		}
	}

	cv::normalize(hist, hist, 0, 1, cv::NORM_MINMAX);

	return new EdgeHistogram(hist);
}


Feature* EdgeHistogramCreator::createFromCluster(const std::vector<Feature*>& cluster) const {
	cv::Mat first = static_cast<EdgeHistogram*>(cluster[0])->getHistogram();
	cv::Mat sum = cv::Mat::zeros(first.size(), first.type());

	for (const auto& feat : cluster) {
		cv::Mat hist = static_cast<EdgeHistogram*>(feat)->getHistogram();
		sum += hist;
	}

	sum /= static_cast<float>(cluster.size());
	return new EdgeHistogram(sum);
}

Feature* EdgeHistogramCreator::readFile(const std::string path) {
	std::ifstream ifs(path, std::ios::binary);
	if (!ifs.is_open()) return nullptr;

	int rows, cols, type;
	ifs.read((char*)&rows, sizeof(int));
	ifs.read((char*)&cols, sizeof(int));
	ifs.read((char*)&type, sizeof(int));

	cv::Mat histogram(rows, cols, type);
	size_t dataSize = histogram.total() * histogram.elemSize();
	ifs.read((char*)histogram.data, dataSize);
	ifs.close();

	return new EdgeHistogram(histogram);
}

Feature* SIFTCreator::extract(cv::Mat& image) {
	cv::Ptr<cv::SIFT> sift = cv::SIFT::create();
	std::vector<cv::KeyPoint> keypoints;
	cv::Mat descriptors;
	sift->detectAndCompute(image, cv::noArray(), keypoints, descriptors);

	if (descriptors.empty()) {
		return new SIFT(cv::Mat());
	}
	return new SIFT(descriptors);
}


Feature* SIFTCreator::createFromCluster(const std::vector<Feature*>& cluster) const {
	std::vector<cv::Mat> all_descs;
	all_descs.reserve(cluster.size());

	for (auto* f : cluster) {
		auto* sift = dynamic_cast<SIFT*>(f);
		if (sift && !sift->getDescriptors().empty())
			all_descs.push_back(sift->getDescriptors());
	}

	if (all_descs.empty()) return nullptr;

	cv::Mat all;
	cv::vconcat(all_descs, all);

	cv::Mat all_32f;
	all.convertTo(all_32f, CV_32F);
	const int num_words = 10;
	cv::Mat labels, centers;

	cv::kmeans(all_32f, num_words, labels,
		cv::TermCriteria(cv::TermCriteria::EPS + cv::TermCriteria::COUNT, 50, 1.0),
		1, cv::KMEANS_PP_CENTERS, centers);

	centers.convertTo(centers, CV_32F);
	return new SIFT(centers);
}

Feature* SIFTCreator::readFile(const std::string path) {
	std::ifstream ifs(path, std::ios::binary);
	if (!ifs.is_open()) return nullptr;

	int rows, cols, type;
	ifs.read((char*)&rows, sizeof(int));
	ifs.read((char*)&cols, sizeof(int));
	ifs.read((char*)&type, sizeof(int));

	cv::Mat desc(rows, cols, type);
	size_t dataSize = desc.total() * desc.elemSize();
	ifs.read((char*)desc.data, dataSize);
	ifs.close();

	return new SIFT(desc);
}

Feature* CorrelogramCreator::extract(cv::Mat& image) {
	cv::Mat hsv;
	cv::cvtColor(image, hsv, cv::COLOR_BGR2HSV);

	int hBins = 8, sBins = 3, vBins = 3;
	int totalBins = hBins * sBins * vBins;

	cv::Mat quantized(hsv.rows, hsv.cols, CV_8U);
	for (int i = 0; i < hsv.rows; i++) {
		for (int j = 0; j < hsv.cols; j++) {
			cv::Vec3b pixel = hsv.at<cv::Vec3b>(i, j);
			int h = pixel[0] * hBins / 180;
			int s = pixel[1] * sBins / 256;
			int v = pixel[2] * vBins / 256;
			quantized.at<uchar>(i, j) = h * sBins * vBins + s * vBins + v;
		}
	}

	int d = 1;
	cv::Mat correlogram = cv::Mat::zeros(1, totalBins, CV_32F);

	for (int i = d; i < quantized.rows - d; i++) {
		for (int j = d; j < quantized.cols - d; j++) {
			int c = quantized.at<uchar>(i, j);
			int count = 0;

			std::vector<cv::Point> neighbors = {
				{i - d, j}, {i + d, j}, {i, j - d}, {i, j + d}
			};

			for (auto& n : neighbors) {
				if (n.x >= 0 && n.x < quantized.rows &&
					n.y >= 0 && n.y < quantized.cols &&
					quantized.at<uchar>(n.x, n.y) == c)
					count++;
			}

			correlogram.at<float>(0, c) += count;
		}
	}

	cv::normalize(correlogram, correlogram, 0, 1, cv::NORM_MINMAX);
	return new Correlogram(correlogram);
}

Feature* CorrelogramCreator::createFromCluster(const std::vector<Feature*>& cluster) const {
	cv::Mat first = static_cast<Correlogram*>(cluster[0])->getCorrelogram();
	cv::Mat sum = cv::Mat::zeros(first.size(), first.type());

	for (auto* f : cluster) {
		sum += static_cast<Correlogram*>(f)->getCorrelogram();
	}

	sum /= static_cast<float>(cluster.size());
	return new Correlogram(sum);
}

Feature* CorrelogramCreator::readFile(const std::string path) {
	std::ifstream ifs(path, std::ios::binary);
	if (!ifs.is_open()) return nullptr;

	int rows, cols, type;
	ifs.read((char*)&rows, sizeof(int));
	ifs.read((char*)&cols, sizeof(int));
	ifs.read((char*)&type, sizeof(int));
	cv::Mat corr(rows, cols, type);
	size_t dataSize = corr.total() * corr.elemSize();
	ifs.read((char*)corr.data, dataSize);
	ifs.close();

	return new Correlogram(corr);
}
