#ifndef TIFFCOMPRESSIONDIALOG_H
#define TIFFCOMPRESSIONDIALOG_H

#include <QDialog>
#include <QComboBox>
#include <QSpinBox>
#include <QLabel>
#include "tiffsaver.h"

class TiffCompressionDialog : public QDialog {
    Q_OBJECT

public:
    explicit TiffCompressionDialog(bool isMonochrome, bool isGrayscale, QWidget *parent = nullptr);

    TiffSaveOptions getOptions() const;

private slots:
    void onCompressionChanged(int index);

private:
    void setupUI();

    QComboBox *compressionCombo;
    QSpinBox *levelSpinBox;
    QLabel *levelLabel;
    QLabel *descriptionLabel;

    bool mono;
    bool gray;
};

#endif // TIFFCOMPRESSIONDIALOG_H
