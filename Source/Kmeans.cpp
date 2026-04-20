#include "stdafx.h"
#include "Kmeans.h"
#define INF std::numeric_limits<double>::max()


Kmeans::Kmeans(int k, std::vector<Feature*> objects, Creator* creator) : objects(objects), creator(creator) {
	this->k = std::min((int)objects.size(), k);
	centroids.reserve(k);
	labels.resize(objects.size());

	std::vector<int> indices(objects.size());
	for (int i = 0; i < objects.size(); i++)
		indices[i] = i;

	std::random_device rd;
	std::mt19937 gen(rd());
	std::shuffle(indices.begin(), indices.end(), gen);

	for (int i = 0; i < this->k; ++i)
		centroids.push_back(objects[indices[i]]);
}

void Kmeans::run(int max_iters) {
	for (int iter = 0; iter < max_iters; iter++) {
		bool changed = false;
		for (int i = 0; i < objects.size(); i++) {
			double min_dist = INF;
			int best_cluster = 0;

			for (int c = 0; c < k; ++c) {
				double dist = objects[i]->distance(centroids[c]);
				if (dist < min_dist) {
					min_dist = dist;
					best_cluster = c;
				}
			}

			if (labels[i] != best_cluster) {
				labels[i] = best_cluster;
				changed = true;
			}
		}

		if (!changed) break;

		std::vector<std::vector<Feature*>> clusters(k);
		for (size_t i = 0; i < objects.size(); i++) {
			clusters[labels[i]].push_back(objects[i]);
		}

		for (int c = 0; c < k; c++) {
			if (!clusters[c].empty()) {
				centroids[c] = creator->createFromCluster(clusters[c]);
			}
		}
	}
}

std::vector<std::string> Kmeans::saveObjects(std::string folder) {
	if (!fs::exists(folder)) {
		fs::create_directory(folder);
	}
	std::vector<std::string> folders;
	for (int i = 0; i < centroids.size(); i++) {
		std::string newFolder = folder + "/" + std::to_string(i);
		fs::create_directory(newFolder);
		centroids[i]->save(newFolder + "/centroid.bin");
		folders.push_back(newFolder);
	}
	for (int i = 0; i < objects.size(); i++) {
		std::string path = folder + "/" + std::to_string(labels[i]) + "/" + std::to_string(i);
		objects[i]->save(path);
	}
	return folders;
}

std::vector<int> Kmeans::rankCentroid(cv::Mat& image) {
	std::vector<int> centroidIndex(centroids.size());
	for (int i = 0; i < centroids.size(); i++)
		centroidIndex[i] = i;
	Feature* imgFeature = creator->extract(image);
	std::sort(centroidIndex.begin(), centroidIndex.end(), [&](const int &a, const int &b) {
		return imgFeature->distance(centroids[a]) < imgFeature->distance(centroids[b]);
	});
	delete imgFeature;
	return centroidIndex;
}