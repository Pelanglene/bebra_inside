#include "bitmap.h"

#include <fstream>
#include <stdexcept>

//TODO В заголовке плохо подключать except

bool Bitmap::Load(const char* file_name) {
    std::fstream file;
    file.open(file_name, std::ios_base::in | std::ios_base::binary); // Открываем файл на чтение как бинарный
    if (!file.is_open()) {
        return false;
        // TODO обработать эту ситуацию
    }
    Load(file);
    file.close();
    return true;
}

// TODO Проверки, что поток не сломался, правильный файл итп.
// if(stream) - поток неявно может быть приведён к логическому выражению.
// Если этот if возвращает false - поток сломался (мы не можем из него больше читать).
// Работать это может так: while(std::getline(stream, line)) {...}.
// if(stream) не проверяет, закончился ли файл, поэтому пишут так:
// while(std::getline(stream, line) && !stream.eof()) {...}
// eof - end of file.
bool Bitmap::Load(std::istream& stream) {
    // TODO Никак не обрабатывается ситуация со сломанным потоком (assert?)
    stream.read(reinterpret_cast<char*> (&bmp_header_), sizeof(bmp_header_));
    stream.read(reinterpret_cast<char*> (&dib_header_), sizeof(dib_header_));

    const size_t RUBBISH_BETWEEN_PIXEL_LINES = (4 - (dib_header_.width * 3) % 4) % 4;
    pixels_.SetHeight(dib_header_.height);
    pixels_.SetWidth(dib_header_.width);
    for (size_t row = 0; row < dib_header_.height; ++row) {
        for (size_t column = 0; column < dib_header_.width; ++column) {
            Pixel pixel{};
            stream.read(reinterpret_cast<char*> (&pixel), sizeof(pixel));
            pixels_.PushBack(pixel);
        }
        stream.ignore(RUBBISH_BETWEEN_PIXEL_LINES); // Тут тип должен быть long long вроде, а у меня size_t
    }

    return true;
}

bool Bitmap::WriteFile(const char* file_name) {
    std::ofstream file;
    file.open(file_name, std::ios_base::out | std::ios_base::binary); // Открываем файл на вписывание как бинарный
    if (!file.is_open()) {
        return false;
        // TODO обработать эту ситуацию
    }
    WriteFile(file);
    file.close();
    return true;
}

bool Bitmap::WriteFile(std::ostream& stream) {
    stream.write(reinterpret_cast<char*> (&bmp_header_), sizeof(bmp_header_));
    stream.write(reinterpret_cast<char*> (&dib_header_), sizeof(dib_header_));

    const size_t RUBBISH_BETWEEN_PIXEL_LINES = (4 - (dib_header_.width * 3) % 4) % 4;
    for (size_t row = 0; row < dib_header_.height; ++row) {
        for (size_t column = 0; column < dib_header_.width; ++column) {
            stream.write(reinterpret_cast<char*> (&pixels_.GetPixel(row, column)),
                         sizeof(pixels_.GetPixel(row, column)));
        }
        // В оригинальном файле мог быть мусор, поэтому дальше мы должны вернуть мусор на место
        static const uint32_t RUBBISH = 0x55AA55AA;
        stream.write(reinterpret_cast<const char*> (&RUBBISH), RUBBISH_BETWEEN_PIXEL_LINES);
    }
    return true;
}