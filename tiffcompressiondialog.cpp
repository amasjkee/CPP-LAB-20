#include "tiffcompressiondialog.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFormLayout>
#include <QPushButton>
#include <QGroupBox>

TiffCompressionDialog::TiffCompressionDialog(bool isMonochrome, bool isGrayscale, QWidget *parent)
    : QDialog(parent), mono(isMonochrome), gray(isGrayscale) {
    setupUI();
}

void TiffCompressionDialog::setupUI() {
    setWindowTitle("TIFF Compression Settings");
    setMinimumWidth(400);

    QVBoxLayout *mainLayout = new QVBoxLayout(this);

    // Группа выбора сжатия
    QGroupBox *compressionGroup = new QGroupBox("Compression Method");
    QFormLayout *formLayout = new QFormLayout(compressionGroup);

    compressionCombo = new QComboBox();
    compressionCombo->addItem("None (Uncompressed)", (int)TiffCompression::None);
    compressionCombo->addItem("Deflate (ZIP)", (int)TiffCompression::Deflate);
    compressionCombo->addItem("LZMA", (int)TiffCompression::LZMA);
    compressionCombo->addItem("PackBits (RLE)", (int)TiffCompression::PackBits);
    compressionCombo->addItem("LZW", (int)TiffCompression::LZW);

    if (mono) {
        compressionCombo->addItem("CCITT Group 3 (Fax)", (int)TiffCompression::CCITT_Group3);
        compressionCombo->addItem("CCITT Group 4 (Fax)", (int)TiffCompression::CCITT_Group4);
    }

    if (!mono) {
        compressionCombo->addItem("JPEG", (int)TiffCompression::JPEG);
    }

    connect(compressionCombo, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &TiffCompressionDialog::onCompressionChanged);

    levelLabel = new QLabel("Compression Level:");
    levelSpinBox = new QSpinBox();
    levelSpinBox->setRange(1, 9);
    levelSpinBox->setValue(6);

    formLayout->addRow("Method:", compressionCombo);
    formLayout->addRow(levelLabel, levelSpinBox);

    // Описание
    descriptionLabel = new QLabel();
    descriptionLabel->setWordWrap(true);
    descriptionLabel->setStyleSheet("color: #808080; padding: 10px; background: #f5f5f5;");

    // Кнопки
    QHBoxLayout *buttonLayout = new QHBoxLayout();
    QPushButton *okButton = new QPushButton("OK");
    QPushButton *cancelButton = new QPushButton("Cancel");

    connect(okButton, &QPushButton::clicked, this, &QDialog::accept);
    connect(cancelButton, &QPushButton::clicked, this, &QDialog::reject);

    buttonLayout->addStretch();
    buttonLayout->addWidget(okButton);
    buttonLayout->addWidget(cancelButton);

    mainLayout->addWidget(compressionGroup);
    mainLayout->addWidget(descriptionLabel);
    mainLayout->addLayout(buttonLayout);

    onCompressionChanged(0);
}

void TiffCompressionDialog::onCompressionChanged(int index) {
    TiffCompression comp = (TiffCompression)compressionCombo->currentData().toInt();

    bool hasLevel = false;
    QString desc;

    switch (comp) {
    case TiffCompression::None:
        desc = "No compression. Largest file size, fastest processing.";
        break;

    case TiffCompression::Deflate:
        hasLevel = true;
        levelSpinBox->setRange(1, 9);
        levelSpinBox->setValue(6);
        levelLabel->setText("Deflate Level (1-9):");
        desc = "ZIP/Deflate compression. Level 1 = fast, Level 9 = best compression. Good balance of speed and compression.";
        break;

    case TiffCompression::LZMA:
        hasLevel = true;
        levelSpinBox->setRange(1, 9);
        levelSpinBox->setValue(6);
        levelLabel->setText("LZMA Preset (1-9):");
        desc = "LZMA compression. Better compression than Deflate but slower. Level 9 gives best compression.";
        break;

    case TiffCompression::PackBits:
        desc = "PackBits RLE compression. Simple run-length encoding. Fast but moderate compression.";
        break;

    case TiffCompression::LZW:
        desc = "LZW compression. Good general-purpose compression. No quality loss.";
        break;

    case TiffCompression::CCITT_Group3:
        desc = "CCITT Group 3 (Fax). For monochrome images only. Used in fax machines.";
        break;

    case TiffCompression::CCITT_Group4:
        desc = "CCITT Group 4 (Fax). For monochrome images only. Better compression than Group 3.";
        break;

    case TiffCompression::JPEG:
        hasLevel = true;
        levelSpinBox->setRange(0, 100);
        levelSpinBox->setValue(85);
        levelLabel->setText("JPEG Quality (0-100):");
        desc = "JPEG compression. Lossy compression for color and grayscale images. 100 = best quality, 0 = worst quality.";
        break;
    }

    levelLabel->setVisible(hasLevel);
    levelSpinBox->setVisible(hasLevel);
    descriptionLabel->setText(desc);
}

TiffSaveOptions TiffCompressionDialog::getOptions() const {
    TiffCompression comp = (TiffCompression)compressionCombo->currentData().toInt();
    int level = levelSpinBox->value();
    return TiffSaveOptions(comp, level);
}
