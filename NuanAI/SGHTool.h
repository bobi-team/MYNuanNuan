#include <opencv2/opencv.hpp>
#include <iostream>
#include <string>
#include <vector>
#include <filesystem>
#include<opencv2/opencv.hpp>

#pragma comment(lib,"opencv_world4110d.lib")
namespace fs = std::filesystem;

class SGHTool {
private:
    std::string directory;
    int blockSize;
    int rows;
    int cols;
    cv::Mat largeImage;

    bool loadAndStitchImages() {
        std::vector<cv::Mat> imageRows;
        for (int row = 0; row < rows; ++row) {
            std::vector<cv::Mat> imageRow;
            for (int col = 0; col < cols; ++col) {
                std::string filename = directory + "/6-" + std::to_string(row) + "-" + std::to_string(col) + ".webp";
                if (fs::exists(filename)) {
                    cv::Mat img = cv::imread(filename, cv::IMREAD_COLOR);
                    if (!img.empty()) {
                        imageRow.push_back(img);
                    }
                    else {
                        std::cerr << "Failed to load image: " << filename << std::endl;
                        return false;
                    }
                }
                else {
                    std::cerr << "File does not exist: " << filename << std::endl;
                    return false;
                }
            }
            cv::Mat stitchedRow;
            cv::hconcat(imageRow, stitchedRow);
            imageRows.push_back(stitchedRow);
        }
        cv::vconcat(imageRows, largeImage);
        return true;
    }

public:
    SGHTool(const std::string& dir, int blockSize_, int rows_, int cols_)
        : directory(dir), blockSize(blockSize_), rows(rows_), cols(cols_), largeImage() {
    }

    ~SGHTool() {}

    void run() {
        if (loadAndStitchImages()) {
            cv::imshow("Stitched Image", largeImage);
            cv::waitKey(0);
        }
    }
};