// AddCollectionDialog.h
#include <QDialog>
#include "ui_add_collection.h"
#include "Collection.h"

class AddCollectionDialog : public QDialog {
    Q_OBJECT
public:
    AddCollectionDialog(QWidget* parent = nullptr);
    void selectFolder();
    std::string getFolderPath() { return ui.folder_path->toPlainText().toStdString(); }
    std::string getFeature() { return ui.feature_select->currentText().toStdString(); }
    std::string getName() { return ui.collection_name->toPlainText().toStdString(); }
    int getK() { return ui.num_k->value(); }
private:
    Ui::Dialog ui;
};
