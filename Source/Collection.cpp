#include "stdafx.h"
#include "Collection.h"

Collection::Collection(Creator* creator, std::string folderPath, std::string name, int k) {
    this->creator = creator;
    std::vector<Feature*> features;
    for (const auto& entry : fs::directory_iterator(folderPath)) {
        if (entry.is_regular_file()) {
            std::string imagePath = entry.path().generic_string();
            cv::Mat image = cv::imread(imagePath, cv::IMREAD_COLOR);
            this->images.push_back(image);
            features.push_back(creator->extract(image));
        }
    }
    clusters = new Kmeans(k, features, creator);
    clusters->run(1000);
    this->location = name;
}

void Collection::save() {
    std::ofstream metafile("metadata.txt", std::ios::app);
    metafile << clusters->size() << "\n" << creator->getType() << "\n" << this->location << "\n";
    metafile.close();

    fs::create_directories(this->location);
    std::vector<std::string> paths = clusters->saveObjects(this->location);
    std::vector<int> labels = clusters->getLabels();

    for (size_t i = 0; i < images.size(); ++i) {
        std::string path = paths[labels[i]] + "/" + std::to_string(i) + ".png";
        if (!cv::imwrite(path, images[i])) {
            std::cerr << "Failed to save image " << path << std::endl;
        }
    }
}