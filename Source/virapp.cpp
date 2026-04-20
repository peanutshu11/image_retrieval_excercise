#include "stdafx.h"
#include "virapp.h"

VIRApp::VIRApp(QWidget *parent)
    : QWidget(parent)
{
    ui.setupUi(this);
    meta = readMetadata("metadata.txt");
    connect(ui.select_image, &QPushButton::clicked, this, &VIRApp::selectImage);
    connect(ui.add_collection, &QPushButton::clicked, this, &VIRApp::addCollection);
    connect(ui.collections, &QListWidget::itemClicked, this, &VIRApp::onCollectionClicked);
    connect(ui.retrieve_button, &QPushButton::clicked, this, &VIRApp::retrieve);
    connect(ui.measure, &QPushButton::clicked, this, &VIRApp::measuremAP);
}

std::vector<Meta> VIRApp::readMetadata(const std::string& filename) {
    std::ofstream metafile(filename, std::ios::app);
    metafile.close();
    std::ifstream infile(filename);
    std::vector<Meta> metadata;
    std::string line;
    ui.collections->clear();
    while (true) {
        Meta meta;
        if (!std::getline(infile, line)) break;
        meta.size = std::stoi(line);
        if (!std::getline(infile, meta.type)) break;
        if (!std::getline(infile, meta.path)) break;
        metadata.push_back(meta);
        std::string title = meta.path;
        ui.collections->addItem(QString::fromStdString(title));
    }
    return metadata;
}

void VIRApp::selectImage()
{
    QString fileName = QFileDialog::getOpenFileName(this, "Open Image", "",
        "Images (*.png *.jpg *.jpeg *.bmp *.gif)");
    if (!fileName.isEmpty()) {
        QPixmap pix(fileName);
        ui.image_show->setPixmap(pix.scaled(300, 250, Qt::KeepAspectRatio));
        currentImg = cv::imread(fileName.toStdString(), cv::IMREAD_COLOR);
    }
}

void VIRApp::onCollectionClicked(QListWidgetItem* item) {
    ui.log_info->setText("Switching collection....");
    int index = ui.collections->row(item);
    Creator* creator = nullptr;
    if (meta[index].type == "Color Histogram") {
        creator = new ColorHistogramCreator();
    }
    else if (meta[index].type == "ORB") {
        creator = new ORBCreator();
    }
    else if (meta[index].type == "SIFT") {
        creator = new SIFTCreator();
    }
    else if (meta[index].type == "Edge Histogram") {
        creator = new EdgeHistogramCreator();
    }
    else if (meta[index].type == "Correlogram") {
        creator = new CorrelogramCreator();
    }
    else {
        ui.log_info->setText("Collection type error (check metadata.txt)....");
        return;
    }
    if (retriever) 
        delete retriever;
    retriever = new Retriever(meta[index].path, creator, meta[index].size);
    ui.log_info->setText("Collection selected!");
}

void VIRApp::retrieve() {
    auto start = std::chrono::high_resolution_clock::now();
    retrieved = retriever->retrieve(currentImg, ui.retrieve_num->value());
    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double, std::milli> duration = end - start;
    ui.log_info->setText(QString::fromStdString(std::string("Images retrieved in ") + std::to_string(duration.count()) + std::string("ms!")));
    loadImages(retrieved);
}

void VIRApp::loadImages(const std::vector<cv::Mat>& images) 
{
    // clear any previous images
    QLayoutItem* item;
    while ((item = ui.retrived_image->takeAt(0)) != nullptr) {
        delete item->widget();
        delete item;
    }
    // covert to rgb
    auto matToQImage = [](const cv::Mat& mat) -> QImage {
        cv::Mat rgb;
        cv::cvtColor(mat, rgb, cv::COLOR_BGR2RGB);
        return QImage(rgb.data, rgb.cols, rgb.rows, rgb.step, QImage::Format_RGB888).copy();
    };
    // Qlable
    for (const cv::Mat& mat : images) {
        QLabel* label = new QLabel;
        QPixmap pixmap = QPixmap::fromImage(matToQImage(mat));
        QPixmap scaledPixmap = pixmap.scaled(100, 100, Qt::KeepAspectRatio, Qt::SmoothTransformation);
        label->setPixmap(scaledPixmap);
        ui.retrived_image->addWidget(label);
        label->setAlignment(Qt::AlignCenter);
    }
}

void VIRApp::addCollection() {
    ui.log_info->setText("Creating collection....");
    AddCollectionDialog* dialog = new AddCollectionDialog(this);
    if (dialog->exec() == QDialog::Accepted) {
        
        Creator* creator = nullptr;
        if (dialog->getFeature() == "Color Histogram") {
            creator = new ColorHistogramCreator();
        }
        else if (dialog->getFeature() == "ORB") {
            creator = new ORBCreator();
        }
        else if (dialog->getFeature() == "SIFT") {
            creator = new SIFTCreator();
        }
        else if (dialog->getFeature() == "Edge Histogram") {
            creator = new EdgeHistogramCreator();
        }
        else if (dialog->getFeature() == "Correlogram") {
            creator = new CorrelogramCreator();
        }
        // Create collection (extract feature from images, run kmeans and save to file)
        if (creator != nullptr) {
            Collection collection = Collection(creator, dialog->getFolderPath(), dialog->getName(), dialog->getK());
            collection.save();
            meta.push_back(Meta(dialog->getName(), dialog->getFeature(), dialog->getK()));
            ui.collections->addItem(QString::fromStdString(dialog->getName()));
            ui.log_info->setText("Collection saved!");
        }
        else {
            ui.log_info->setText("Unidentified feature");
        }
    }
}

double VIRApp::computeAP(const std::vector<cv::Mat>& retrieved, const std::vector<cv::Mat>& groundTruth) {
    int correct = 0;
    double ap = 0.0;
    std::vector<bool> matched(groundTruth.size(), false);  // prevent duplicate matches

    auto isMatch = [](const cv::Mat& a, const cv::Mat& b, double threshold = 1e-6) -> bool {
        if (a.size() != b.size() || a.type() != b.type()) return false;
        double diff = cv::norm(a, b);
        return diff < threshold;
    };

    for (size_t i = 0; i < retrieved.size(); i++) {
        for (size_t j = 0; j < groundTruth.size(); j++) {
            if (!matched[j] && isMatch(retrieved[i], groundTruth[j])) {
                matched[j] = true;
                correct++;
                ap += static_cast<double>(correct) / (i + 1);
                break;
            }
        }
    }

    if (groundTruth.empty()) return 0.0;
    return ap / groundTruth.size();
}

void VIRApp::measuremAP() {
    ui.log_info->setText("Measuring mAP...");
    int numQ = 0;
    double sAP = 0.0;
    double totTime = 0.0;
    std::string queryPath = ui.dataset->text().toStdString();
    for (const auto& entry : fs::directory_iterator(queryPath)) {
        std::string ext = entry.path().extension().string();
        std::string imgPath = entry.path().string();
        std::string name = entry.path().stem().string();
        if (ext == ".png") {
            // query
            cv::Mat query = cv::imread(imgPath, cv::IMREAD_COLOR);
            // retrieve + measure time
            auto start = std::chrono::high_resolution_clock::now();
            std::vector<cv::Mat> retrieved = retriever->retrieve(query, ui.retrieve_num->value());
            auto end = std::chrono::high_resolution_clock::now();
            std::chrono::duration<double, std::milli> duration = end - start;
            totTime += duration.count();

            //ground truth
            std::vector<cv::Mat> groundTruth;
            for (const auto& gtEntry : fs::directory_iterator(queryPath + "/" + name)) {
                std::string gtPath = gtEntry.path().string();
                cv::Mat gt = cv::imread(gtPath, cv::IMREAD_COLOR);
                groundTruth.push_back(gt);
            }
            numQ++;
            sAP += computeAP(retrieved, groundTruth);
        }
    }

    double mAP = sAP / numQ;
    ui.log_info->setText(QString::fromStdString("measured mAP: " + std::to_string(mAP) + " in " + std::to_string(totTime) + "ms"));
}
 
VIRApp::~VIRApp() {
    QLayoutItem* item;
    while ((item = ui.retrived_image->takeAt(0)) != nullptr) {
        delete item->widget();
        delete item;
    }
}

