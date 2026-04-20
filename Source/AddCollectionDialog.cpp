#include "stdafx.h"
#include "AddCollectionDialog.h"

AddCollectionDialog::AddCollectionDialog(QWidget* parent)    : QDialog(parent) {
    ui.setupUi(this);
    connect(ui.select_folder, &QPushButton::clicked, this, &AddCollectionDialog::selectFolder);
}

void AddCollectionDialog::selectFolder() {
    QString folderPath = QFileDialog::getExistingDirectory(this, "Select Folder");

    if (!folderPath.isEmpty()) {
        ui.folder_path->setPlainText(folderPath);
    }
}