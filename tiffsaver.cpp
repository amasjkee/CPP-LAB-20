#include "tiffsaver.h"
#include <tiffio.h>
#include <QDebug>

bool TiffSaver::saveTiff(const QString &filename, const QImage &image,
                         const TiffSaveOptions &options, QString *errorString) {
    if (image.isNull()) {
        if (errorString) *errorString = "Image is null";
        return false;
    }

    // Открываем TIFF файл
    TIFF *tif = TIFFOpen(filename.toLocal8Bit().constData(), "w");
    if (!tif) {
        if (errorString) *errorString = "Cannot open TIFF file for writing";
        return false;
    }

    // Конвертируем изображение в подходящий формат
    QImage processedImage = image;
    bool isMono = isMonochrome(image);
    bool isGray = isGrayscale(image);

    // Проверка совместимости сжатия с типом изображения
    if ((options.compression == TiffCompression::CCITT_Group3 ||
         options.compression == TiffCompression::CCITT_Group4) && !isMono) {
        if (errorString) *errorString = "CCITT compression requires monochrome image";
        TIFFClose(tif);
        return false;
    }

    if (options.compression == TiffCompression::JPEG && isMono) {
        if (errorString) *errorString = "JPEG compression not suitable for monochrome images";
        TIFFClose(tif);
        return false;
    }

    // Базовые теги
    TIFFSetField(tif, TIFFTAG_IMAGEWIDTH, image.width());
    TIFFSetField(tif, TIFFTAG_IMAGELENGTH, image.height());
    TIFFSetField(tif, TIFFTAG_PLANARCONFIG, PLANARCONFIG_CONTIG);
    TIFFSetField(tif, TIFFTAG_ORIENTATION, ORIENTATION_TOPLEFT);

    // Настройка по типу изображения
    if (isMono) {
        // Монохромное (1 бит)
        processedImage = image.convertToFormat(QImage::Format_Mono);
        TIFFSetField(tif, TIFFTAG_BITSPERSAMPLE, 1);
        TIFFSetField(tif, TIFFTAG_SAMPLESPERPIXEL, 1);
        TIFFSetField(tif, TIFFTAG_PHOTOMETRIC, PHOTOMETRIC_MINISBLACK);
    } else if (isGray) {
        // Градации серого (8 бит)
        processedImage = image.convertToFormat(QImage::Format_Grayscale8);
        TIFFSetField(tif, TIFFTAG_BITSPERSAMPLE, 8);
        TIFFSetField(tif, TIFFTAG_SAMPLESPERPIXEL, 1);
        TIFFSetField(tif, TIFFTAG_PHOTOMETRIC, PHOTOMETRIC_MINISBLACK);
    } else {
        // RGB (24 бит)
        processedImage = image.convertToFormat(QImage::Format_RGB888);
        TIFFSetField(tif, TIFFTAG_BITSPERSAMPLE, 8);
        TIFFSetField(tif, TIFFTAG_SAMPLESPERPIXEL, 3);
        TIFFSetField(tif, TIFFTAG_PHOTOMETRIC, PHOTOMETRIC_RGB);
    }

    // Настройка сжатия
    switch (options.compression) {
    case TiffCompression::None:
        TIFFSetField(tif, TIFFTAG_COMPRESSION, COMPRESSION_NONE);
        break;

    case TiffCompression::Deflate:
        TIFFSetField(tif, TIFFTAG_COMPRESSION, COMPRESSION_DEFLATE);
        TIFFSetField(tif, TIFFTAG_ZIPQUALITY, options.compressionLevel);
        break;

    case TiffCompression::LZMA:
        TIFFSetField(tif, TIFFTAG_COMPRESSION, COMPRESSION_LZMA);
        TIFFSetField(tif, TIFFTAG_LZMAPRESET, options.compressionLevel);
        break;

    case TiffCompression::PackBits:
        TIFFSetField(tif, TIFFTAG_COMPRESSION, COMPRESSION_PACKBITS);
        break;

    case TiffCompression::LZW:
        TIFFSetField(tif, TIFFTAG_COMPRESSION, COMPRESSION_LZW);
        break;

    case TiffCompression::CCITT_Group3:
        TIFFSetField(tif, TIFFTAG_COMPRESSION, COMPRESSION_CCITTFAX3);
        TIFFSetField(tif, TIFFTAG_GROUP3OPTIONS, GROUP3OPT_2DENCODING);
        break;

    case TiffCompression::CCITT_Group4:
        TIFFSetField(tif, TIFFTAG_COMPRESSION, COMPRESSION_CCITTFAX4);
        break;

    case TiffCompression::JPEG:
        TIFFSetField(tif, TIFFTAG_COMPRESSION, COMPRESSION_JPEG);
        TIFFSetField(tif, TIFFTAG_JPEGQUALITY, options.compressionLevel);
        break;
    }

    TIFFSetField(tif, TIFFTAG_ROWSPERSTRIP, TIFFDefaultStripSize(tif, 0));

    // Запись данных построчно
    // int bytesPerLine = processedImage.bytesPerLine();

    for (int y = 0; y < processedImage.height(); ++y) {
        const uchar *line = processedImage.constScanLine(y);
        if (TIFFWriteScanline(tif, (void*)line, y, 0) < 0) {
            if (errorString) *errorString = QString("Error writing line %1").arg(y);
            TIFFClose(tif);
            return false;
        }
    }

    TIFFClose(tif);
    return true;
}

bool TiffSaver::isMonochrome(const QImage &image) {
    for (int y = 0; y < image.height(); ++y) {
        for (int x = 0; x < image.width(); ++x) {
            QRgb pixel = image.pixel(x, y);
            int gray = qGray(pixel);
            if (gray != 0 && gray != 255) {
                return false;
            }
            if (qRed(pixel) != qGreen(pixel) || qGreen(pixel) != qBlue(pixel)) {
                return false;
            }
        }
    }
    return true;
}

bool TiffSaver::isGrayscale(const QImage &image) {
    for (int y = 0; y < image.height(); ++y) {
        for (int x = 0; x < image.width(); ++x) {
            QRgb pixel = image.pixel(x, y);
            if (qRed(pixel) != qGreen(pixel) || qGreen(pixel) != qBlue(pixel)) {
                return false;
            }
        }
    }
    return true;
}
