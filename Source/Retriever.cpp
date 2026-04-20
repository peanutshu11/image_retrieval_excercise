#include "stdafx.h"
#include "Retriever.h"

Retriever::Retriever(std::string path, Creator* creator, int size) : path(path), creator(creator), size(size) {
    centroids = std::vector<Feature*>(size);
    for (const auto& entry : fs::directory_iterator(path)) {
        if (entry.is_directory()) {
            std::string folderName = entry.path().filename().string();
            int folderIndex = std::stoi(folderName);
            fs::path centroidPath = entry.path() / "centroid.bin";
            if (fs::exists(centroidPath)) {
                Feature* centroid = creator->readFile(centroidPath.string());
                centroids[folderIndex] = centroid;
            }
        }
    }
}

std::vector<cv::Mat> Retriever::retrieve(cv::Mat& image, int n) {
    std::vector<int> centroidIndex(size);
    for (int i = 0; i < size; i++) {
        centroidIndex[i] = i;
    }
    Feature* imgFeature = creator->extract(image);
    // rank clusters
    std::sort(centroidIndex.begin(), centroidIndex.end(), [&](const int& a, const int& b) {
        return imgFeature->distance(centroids[a]) < imgFeature->distance(centroids[b]);
    });

    std::vector<cv::Mat> results;
    std::vector<Feature*> resultsFeatures;

    for (auto& i : centroidIndex) {
        std::vector<cv::Mat> images;
        std::vector<Feature*> features;
        // open folder
        for (auto& entry : fs::directory_iterator(path + "/" + std::to_string(i))) {
            std::string ext = entry.path().extension().string();
            std::string imgPath = entry.path().string();
            fs::path name = entry.path().stem();
            if (ext == ".jpg" || ext == ".png" || ext == ".jpeg") {
                cv::Mat img = cv::imread(imgPath, cv::IMREAD_COLOR);
                if (!img.empty()) {
                    images.push_back(img);
                    features.push_back(creator->readFile((entry.path().parent_path() / name).string()));
                }
            }
        }

        // take the whole cluster if possible
        if (results.size() + images.size() > n) {
            std::vector<int> indices(images.size());
            for (int i = 0; i < indices.size(); ++i) {
                indices[i] = i;
            }
            std::sort(indices.begin(), indices.end(), [&](int a, int b) {
                return imgFeature->distance(features[a]) < imgFeature->distance(features[b]);
            });
            int num = n - results.size();
            for (int i = 0; i < num; ++i) {
                results.push_back(images[indices[i]]);
                resultsFeatures.push_back(features[indices[i]]);
            }
            break;
        }
        results.insert(results.end(), images.begin(), images.end());
        resultsFeatures.insert(resultsFeatures.end(), features.begin(), features.end());
        if (results.size() == n) {
            break;
        }
    }

    // rank result
    std::vector<int> indices(results.size());
    for (int i = 0; i < indices.size(); i++) {
        indices[i] = i;
    }
    std::sort(indices.begin(), indices.end(), [&](int a, int b) {
        return imgFeature->distance(resultsFeatures[a]) < imgFeature->distance(resultsFeatures[b]);
    });
    std::vector<cv::Mat> rankedResult;
    for (auto& i : indices) {
        rankedResult.push_back(results[i]);
    }
    for (auto* f : resultsFeatures) {
        delete f;
    }
    delete imgFeature;
    return rankedResult;
}