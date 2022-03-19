#pragma once

#include <cstdint>
#include <iostream>
#include <vector>
#include <tuple>

class NewPixelArray;

class Bitmap {
    // Структуры просто на поиграться с padding:
public:
    struct TestHeader {
        unsigned char field1; // __attribute__((__packed__));
        unsigned short field2; //__attribute__((__packed__));
        unsigned long field3; //__attribute__((__packed__));
    };
    struct TestHeader2 {
        unsigned char field1; // __attribute__((__packed__));
        unsigned short field2; //__attribute__((__packed__));
        unsigned long field3; //__attribute__((__packed__));
    } __attribute__((__packed__));

public:
    struct BMPHeader {
        uint16_t signature;
        uint32_t file_size;
        uint32_t dummy;
        uint32_t bitarray_offset;
    } __attribute__((__packed__));

    struct DIBHeader {
        uint32_t dib_header_size;
        uint32_t width;  // Ширина
        uint32_t height;  // Высота
        uint64_t dummy;  // Это же нам не нужно?
        uint32_t raw_bitmap_data_size;  // (including padding)
        uint64_t dummy2;  // Это же нам не нужно?
        uint64_t dummy3;  // Это же нам не нужно?
    } __attribute__((__packed__));

    struct Pixel {
        uint8_t blue = 0;
        uint8_t green = 0;
        uint8_t red = 0;

        bool operator==(const Pixel& rhv) const {
            return std::tie(blue, green, red) == std::tie(rhv.blue, rhv.green, rhv.red);
        }

        bool operator!=(const Pixel& rhv) const {
            return !(*this == rhv);
        }
    };

public:
    void SetFileWidth(size_t new_width) {
        dib_header_.width = new_width;
    }

    void SetFileHeight(size_t new_height) {
        dib_header_.height = new_height;
    }

    Pixel& GetPixel(size_t row, size_t column) {
        return pixels_.GetPixel(row, column);
    }

    const Pixel& GetPixel(size_t row, size_t column) const {
        return pixels_.GetPixel(row, column);
    }

    uint32_t GetHeight() const {
        return pixels_.GetHeight();
    }

    uint32_t GetWidth() const {
        return pixels_.GetWidth();
    }

    // Возвращает истину, если картина пустая, иначе - ложь
    bool IsEmpty() const {
        return pixels_.IsEmpty();
    }

public:
    class PixelArray {
    public:
        PixelArray() {}

        void PushBack(Pixel pixel) {
            pixel_array_elements_.push_back(pixel);
        }

        Pixel& GetPixel(size_t row, size_t column) {
            return pixel_array_elements_[pixel_array_width_ * row + column];
        }

        void SetHeight(size_t row) {
            pixel_array_height_ = row;
        }

        void SetWidth(size_t column) {
            pixel_array_width_ = column;
        }

        const Pixel& GetPixel(size_t row, size_t column) const {
            return pixel_array_elements_[pixel_array_width_ * row + column];
        }

        uint32_t GetHeight() const {
            return pixel_array_height_;
        }

        uint32_t GetWidth() const {
            return pixel_array_width_;
        }

        // Возвращает истину, если картина пустая, иначе - ложь
        bool IsEmpty() const {
            return pixel_array_elements_.empty();
        }

        // Данный метод вызывается только в случае абсолюнто пустой матрицы, с resize париться нет смысла.
        void Fill(size_t height, size_t width,
                  uint8_t default_red = 0,
                  uint8_t default_green = 0,
                  uint8_t default_blue = 0) {
            pixel_array_width_ = width;
            pixel_array_height_ = height;
            for (size_t i = 0; i < height * width; ++i) {
                Pixel pixel{default_blue, default_green, default_red};
                pixel_array_elements_.push_back(pixel);
            }
        }

    protected:
        size_t pixel_array_height_;
        size_t pixel_array_width_;
        std::vector<Pixel> pixel_array_elements_;
    };

    void CopyPixels(const PixelArray& other) {
        pixels_ = other;
    }

protected:
    PixelArray pixels_;
    BMPHeader bmp_header_;
    DIBHeader dib_header_;

public:
    // Загружает файл из переданного потока чтения (функция под этой как раз возвращает поток)
    bool Load(std::istream& stream);

    // Загружает файл с картинкой по имени в поток
    bool Load(const char* file_name);

    // Создаёт по имени файла bmp файл и загружает туда что-то.
    bool WriteFile(const char* file_name);

    // Мы можем не иметь права записывать что-то на диск или не иметь какой-то папки
    // поэтому делим create file в виде двух функций, по аналогии с load.
    bool WriteFile(std::ostream& stream);
};
