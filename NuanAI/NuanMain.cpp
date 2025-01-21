#include <iostream>
#include <Windows.h>
#include <stdio.h>  
#include <string.h>
#include <tchar.h>
#include "obj.h"
#include "NuanMain.h"
#include"NGame.h"
// main.cpp
#include "SGHTool.h"



using namespace std;


int main()
{


      
            //NGame game;

            // //初始化游戏，传入游戏窗口名称
            //if (!game.Initialize("无限暖暖")) {
            //    return -1; // 如果初始化失败，则退出程序
            //}
            //
            //game.ditu();
 // Example usage:
#include <opencv2/opencv.hpp>
#include <iostream>
#include <string>
#include <vector>
#include <filesystem>

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

    return 0;

        // 设置键盘钩子
        //HookKeyboard();
    }
