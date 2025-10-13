#ifndef TIFFSAVER_H
#define TIFFSAVER_H

#include <QImage>
#include <QString>

enum class TiffCompression {
    None,           // Без сжатия
    Deflate,        // zlib/deflate (1-9)
    LZMA,           // LZMA (1-9)
    PackBits,       // PackBits (RLE)
    LZW,            // LZW
    CCITT_Group3,   // CCITT Group 3 (факс)
    CCITT_Group4,   // CCITT Group 4 (факс)
    JPEG            // JPEG (0-100)
};

struct TiffSaveOptions {
    TiffCompression compression = TiffCompression::None;
    int compressionLevel = 6;  // Для Deflate/LZMA: 1-9, для JPEG: 0-100

    TiffSaveOptions() = default;
    TiffSaveOptions(TiffCompression comp, int level = 6)
        : compression(comp), compressionLevel(level) {}
};

class TiffSaver {
public:
    static bool saveTiff(const QString &filename, const QImage &image,
                         const TiffSaveOptions &options, QString *errorString = nullptr);

    static bool isMonochrome(const QImage &image);
    static bool isGrayscale(const QImage &image);
};

#endif // TIFFSAVER_H
