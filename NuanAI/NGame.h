#pragma once
#include <iostream>
#include <Windows.h>
#include<string.h>
#include <vector>
#include <cmath>
#include <chrono>
#include<opencv2/opencv.hpp>
#include "obj.h" 
#pragma comment(lib,"opencv_world4110d.lib")
using namespace cv;
using namespace std;
#define ��Į���·�� "E:\\DmLib\\xd47243.dll"
#define �ƽ�Dll·�� "E:\\DmLib\\Go.dll"
typedef void (WINAPI* GoFunProc)(DWORD g_dm);


class NGame {
public:
    HWND gameHwnd; // ���ھ��
    dmsoft* dm; // ��Į���ʵ��
    RECT gameWindowRect;//������λ��
    RECT miniMapRect; // С��ͼλ�ã�����ʹ��OpenCV��Point�ṹ��
    RECT pointRect;     //ָ��λ��
    double characterAngle; // ��ǰ��ɫ����Ƕ�
    
    mutable std::mutex angleMutex;
public:
    // ���캯��
    NGame() {}

    // ��ʼ������
    bool Initialize(const string& windowName) {
        DWORD dm_hmodule = LoadDm(��Į���·��);
        if (dm_hmodule == NULL) { return 0; }
        HMODULE go_hmodule = LoadLibraryA(�ƽ�Dll·��);
        GoFunProc GoFun = NULL;
        GoFun = (GoFunProc)GetProcAddress(go_hmodule, "Go");
        if (GoFun == NULL) { return 0; }
        GoFun(dm_hmodule);
        dm = new dmsoft();
        long nret = dm->Reg("", "");
        cout << nret << endl;
        if (nret == 1)
        {
            cout << "��Įע��ɹ�" << endl;
        }
        else
        {
            cout << "��Įע��ʧ��" << endl;
            // �ͷ���Դ
            delete dm;
            FreeLibrary(go_hmodule);
            FreeLibrary((HMODULE)dm_hmodule);
            return 0;
        }

        // ������Ϸ����
        gameHwnd = (HWND)dm->FindWindowW("", windowName.c_str());
        if (gameHwnd == NULL) {
            cout << "δ�ҵ�Ŀ�괰�ڣ�" << endl;
            delete dm;
            FreeLibrary(go_hmodule);
            FreeLibrary((HMODULE)dm_hmodule);
            return false;
        }


        // ��ȡ����λ��
        //��ͬ��Ϸ����С��ͼλ�ý���ƫ�Ƽ���
        if (GetWindowRect(gameHwnd, &gameWindowRect)) {
            cout << "��ʼ������ɹ� " << endl;
            cout << "��ʼ��ʼ��С��ͼ " << endl;
            miniMapRect.left = gameWindowRect.left + 75;
            miniMapRect.top = gameWindowRect.top + 65;
            miniMapRect.right = gameWindowRect.left +290;
            miniMapRect.bottom = gameWindowRect.top + 280;
            cout << "С��ͼ��ʼ���ɹ� " << endl;
            cout << "��ʼ��ʼ��ָ�� " << endl;
            pointRect.left = gameWindowRect.left + 170;
            pointRect.top = gameWindowRect.top + 155;
            pointRect.right = gameWindowRect.left + 200;
            pointRect.bottom = gameWindowRect.top + 185;
            cout << "С��ͼ��ʼ���ɹ� " << endl;
        }
        else {
            cout << "��ȡ��Ϸ����λ��ʧ�ܣ�" << endl;
            return false;
        }

        return true;
    }

    bool CaptureGameWindow(const std::string& filename) {
        if (dm->Capture(gameWindowRect.left+10, gameWindowRect.top+10, gameWindowRect.right-10, gameWindowRect.bottom-10, filename.c_str())) {
            cout << "����ͼ�ɹ�"<< endl;
            return true;
        }
        else {
            return false;
        }

    }

    bool CaptureMiniMap(const std::string& filename) {
    
        if (dm->Capture(miniMapRect.left, miniMapRect.top, miniMapRect.right, miniMapRect.bottom, filename.c_str())) {
            cout << "����С��ͼ�ɹ�������Ϊminimap.bmp" << endl;
            return true;
        }
        else {
            return false;
        }
       
    }
    bool CaptureMiniMapEx(const std::string& filename) {

        if (dm->Capture(miniMapRect.left+45, miniMapRect.top+35, miniMapRect.left+180, miniMapRect.top+170, filename.c_str())) {
            cout << "����С��ͼ�ɹ�������Ϊminimap.bmp" << endl;
            return true;
        }
        else {
            return false;
        }

    }

    bool CapturePoint(const std::string& filename) {
        if (dm->Capture(pointRect.left, pointRect.top, pointRect.right, pointRect.bottom, filename.c_str())) {
            std::cout << "����ָ��ɹ�������Ϊ" << filename << std::endl;
            return true;
        }
        else {
            return false;
        }
    }

    std::vector<int> find_peaks(const cv::Mat& gradX, int minDistance = 10, double threshold = 0.5) {
        std::vector<int> peaks;
        for (int col = 1; col < gradX.cols - 1; ++col) {
            float prev = gradX.at<float>(col - 1);
            float curr = gradX.at<float>(col);
            float next = gradX.at<float>(col + 1);
            if (curr > prev && curr > next && curr > threshold) {
                peaks.push_back(col);
            }
        }
        return peaks;
    }
    void setCharacterAngle(double newAngle) {
        std::lock_guard<std::mutex> lock(angleMutex);
        characterAngle = newAngle;
    }
    double getCharacterAngle() const {
        std::lock_guard<std::mutex> lock(angleMutex);
        return characterAngle;
    }
   
    void RunLoop() {
        while(1){

            
            // ���0.2��
            std::this_thread::sleep_for(std::chrono::milliseconds(2000));
        }
        
    }
    void createAndSaveMaskFromImage(const std::string& image_path, const std::string& mask_path) {
        // ��ȡ����͸��ͨ����ͼƬ
        cv::Mat image = cv::imread(image_path, cv::IMREAD_UNCHANGED);

        // ���ͼƬ�Ƿ�ɹ���ȡ
        if (image.empty()) {
            std::cerr << "Error: Unable to load image at path " << image_path << std::endl;
            return;
        }

        // ���ͼƬ�Ƿ���͸��ͨ��
        if (image.channels() < 4) {
            std::cerr << "Error: Image does not have an alpha channel" << std::endl;
            return;
        }

        // ��ȡ͸��ͨ��
        std::vector<cv::Mat> channels;
        cv::split(image, channels);
        cv::Mat alpha_channel = channels[3]; // Alphaͨ��ͨ���������һ��ͨ��

        // ��������ͼ��͸����������ɫ����͸����������ɫ
        cv::Mat mask;
        cv::threshold(alpha_channel, mask, 0, 255, cv::THRESH_BINARY); // ��ֵ������

        // ��������ͼ��
        if (!cv::imwrite(mask_path, mask)) {
            std::cerr << "Error: Unable to save mask image at path " << mask_path << std::endl;
            return;
        }

        std::cout << "Mask image saved successfully at " << mask_path << std::endl;
    }

    int masktest() {
        std::string image_path = "template.png"; // �滻Ϊ���ͼƬ·��
        std::string mask_path = "mask.png";   // �滻Ϊ��Ҫ��������ͼƬ��·��

        createAndSaveMaskFromImage(image_path, mask_path);

        return 0;
    }


    void extractImageUsingMask(const std::string& image_path, const std::string& mask_path, const std::string& output_path) {
        // ��ȡԭʼͼ��
        cv::Mat image = cv::imread(image_path, cv::IMREAD_UNCHANGED);
        // ��ȡ����ͼ��
        cv::Mat mask = cv::imread(mask_path, cv::IMREAD_GRAYSCALE);

        // ���ͼ��������Ƿ�ɹ���ȡ
        if (image.empty() || mask.empty()) {
            std::cerr << "Error: Unable to load image or mask" << std::endl;
            return;
        }

        // ȷ�������ǵ�ͨ����
        if (mask.channels() != 1) {
            std::cerr << "Error: Mask image is not single-channel" << std::endl;
            return;
        }

        // ���������ȡͼ��
        cv::Mat masked_image;
        image.copyTo(masked_image, mask);

        // �����ȡ���ͼ��
        if (!cv::imwrite(output_path, masked_image)) {
            std::cerr << "Error: Unable to save extracted image at path " << output_path << std::endl;
            return;
        }

        std::cout << "Extracted image saved successfully at " << output_path << std::endl;
    }
    int extracttest() {
       
        std::string image_path = "point.png";       // �滻Ϊ���ԭʼͼƬ·��
        std::string mask_path = "mask.png";         // �滻Ϊ�������ͼƬ·��
        std::string output_path = "tiqu.png";  // �滻Ϊ��Ҫ�����ȡ��ͼƬ��·��
        CapturePoint(image_path);
        extractImageUsingMask(image_path, mask_path, output_path);

        return 0;
    }


   


    void rotateAndExtractImageUsingMask(const std::string& image_path, const std::string& mask_path, const std::string& output_dir) {
        // ��ȡԭʼͼ��
        cv::Mat image = cv::imread(image_path, cv::IMREAD_UNCHANGED);
        // ��ȡ����ͼ��
        cv::Mat mask = cv::imread(mask_path, cv::IMREAD_GRAYSCALE);

        // ���ͼ��������Ƿ�ɹ���ȡ
        if (image.empty() || mask.empty()) {
            std::cerr << "Error: Unable to load image or mask" << std::endl;
            return;
        }

        // ȷ�������ǵ�ͨ����
        if (mask.channels() != 1) {
            std::cerr << "Error: Mask image is not single-channel" << std::endl;
            return;
        }

        // ��ʼ�������Դ洢��ת�ĽǶ�
        int angle = 0;

        // ѭ����ת����ͼ��ÿ10����תһ�Σ��ܹ���ת360��
        for (int rotation_count = 0; rotation_count < 36; ++rotation_count) {
            // ������ת����
            cv::Point2f center(mask.cols / 2.0, mask.rows / 2.0);
            cv::Mat rotation_matrix = cv::getRotationMatrix2D(center, angle, 1.0);

            // ִ����ת
            cv::Mat rotated_mask;
            cv::warpAffine(mask, rotated_mask, rotation_matrix, mask.size());

            // ������ת�Ƕ�
            angle += 10;

            // ������ת��������ȡͼ��
            cv::Mat masked_image;
            image.copyTo(masked_image, rotated_mask);

            // �������·��
            std::string output_path = output_dir + "/rotated_masked_image_" + std::to_string(rotation_count) + ".png";

            // �����ȡ���ͼ��
            if (!cv::imwrite(output_path, masked_image)) {
                std::cerr << "Error: Unable to save extracted image at path " << output_path << std::endl;
            }
            else {
                std::cout << "Extracted image " << rotation_count << " saved successfully at " << output_path << std::endl;
            }
        }

        std::cout << "All rotations and extractions completed." << std::endl;
    }

    int rotatemasktest() {
        std::string image_path = "point.png";       // �滻Ϊ���ԭʼͼƬ·��
        std::string mask_path = "mask.png";
        std::string output_dir = "image";     // �滻Ϊ���Ŀ¼·��

        rotateAndExtractImageUsingMask(image_path, mask_path, output_dir);

        return 0;
    }
    void rotateImageAndExtractWithMask(const std::string& image_path, const std::string& mask_path, const std::string& output_dir) {
        // ��ȡԭʼͼ��
        cv::Mat image = cv::imread(image_path, cv::IMREAD_UNCHANGED);
        // ��ȡ����ͼ��
        cv::Mat mask = cv::imread(mask_path, cv::IMREAD_GRAYSCALE);

        // ���ͼ��������Ƿ�ɹ���ȡ
        if (image.empty() || mask.empty()) {
            std::cerr << "Error: Unable to load image or mask" << std::endl;
            return;
        }

        // ȷ�������ǵ�ͨ����
        if (mask.channels() != 1) {
            std::cerr << "Error: Mask image is not single-channel" << std::endl;
            return;
        }

        // ��ʼ�������Դ洢��ת�ĽǶ�
        int angle = 0;

        // ѭ����תԭʼͼ��ÿ10����תһ�Σ��ܹ���ת360��
        for (int rotation_count = 0; rotation_count < 36; ++rotation_count) {
            // ������ת����
            cv::Point2f center(image.cols / 2.0, image.rows / 2.0);
            cv::Mat rotation_matrix = cv::getRotationMatrix2D(center, angle, 1.0);

            // ִ����ת
            cv::Mat rotated_image;
            cv::warpAffine(image, rotated_image, rotation_matrix, image.size());

            // ������ת�Ƕ�
            angle += 10;

            // ���ݲ���ת�������ȡ��ת���ͼ��
            cv::Mat masked_image;
            rotated_image.copyTo(masked_image, mask);

            // �������·��
            std::string output_path = output_dir + "/rotated_image_masked_" + std::to_string(rotation_count) + ".png";

            // �����ȡ���ͼ��
            if (!cv::imwrite(output_path, masked_image)) {
                std::cerr << "Error: Unable to save extracted image at path " << output_path << std::endl;
            }
            else {
                std::cout << "Extracted image " << rotation_count << " saved successfully at " << output_path << std::endl;
            }
        }

        std::cout << "All rotations and extractions completed." << std::endl;
    }

    int rotateimagetest() {
        std::string image_path = "point.png";       // �滻Ϊ���ԭʼͼƬ·��
        std::string mask_path = "mask.png";
        std::string output_dir = "image1";     // �滻Ϊ���Ŀ¼·��

        rotateImageAndExtractWithMask(image_path, mask_path, output_dir);

        return 0;
    }



    double calculateSimilarity(const cv::Mat& template_image, const cv::Mat& masked_image) {
        // �������ͼ���Ƿ�Ϊ��
        if (template_image.empty() || masked_image.empty()) {
            std::cerr << "Input images should not be empty" << std::endl;
            return -1.0;
        }

        // ���ͼ��ߴ��Ƿ���ͬ
        if (template_image.size() != masked_image.size()) {
            std::cerr << "Template and masked images should have the same size" << std::endl;
            return -1.0;
        }

        // ���ͼ�������Ƿ���ͬ
        if (template_image.type() != masked_image.type()) {
            std::cerr << "Template and masked images should have the same type" << std::endl;
            return -1.0;
        }

        // ����������
        cv::Mat result;
        int match_method = cv::TM_CCOEFF_NORMED; // ʹ�ù�һ�����ϵ��

        // ����ģ��ƥ��
        cv::matchTemplate(masked_image, template_image, result, match_method);

        // ��ȡ���ƥ������ƶ�ֵ
        double minVal, maxVal;
        cv::Point minLoc, maxLoc;
        cv::minMaxLoc(result, &minVal, &maxVal, &minLoc, &maxLoc);

        // ����������ƶ�ֵ
        return maxVal;
    }

    void rotateAndExtractWithMask(const std::string& image_path, const std::string& mask_path, const std::string& template_path) {
        // ��ȡԭʼͼ��
        cv::Mat image = cv::imread(image_path, cv::IMREAD_COLOR);
        // ��ȡ����ͼ��
        cv::Mat mask = cv::imread(mask_path, cv::IMREAD_GRAYSCALE);
        // ��ȡģ��ͼ��
        cv::Mat template_image = cv::imread(template_path, cv::IMREAD_COLOR);

        // ���ͼ��������Ƿ�ɹ���ȡ
        if (image.empty() || mask.empty() || template_image.empty()) {
            std::cerr << "Error: Unable to load image, mask or template" << std::endl;
            return;
        }

        // ȷ�������Ƕ�ֵ��
        cv::threshold(mask, mask, 128, 255, cv::THRESH_BINARY);

        // Ԥ����ת��Ϊ�Ҷ�ͼ�������Ҫ��
        cv::Mat image_gray, template_gray;
        cv::cvtColor(image, image_gray, cv::COLOR_BGR2GRAY);
        cv::cvtColor(template_image, template_gray, cv::COLOR_BGR2GRAY);




        // ��ʼ�������Դ洢������ƶȺͶ�Ӧ�ĽǶ�
        double max_similarity = 0.0;
        int max_similarity_angle = 0;

        // ��ȡ��ʼʱ��
        auto start_time = std::chrono::high_resolution_clock::now();

        // ѭ����תԭʼͼ��ÿ10����תһ�Σ��ܹ���ת360��
        for (int angle = 0; angle < 360; angle += 10) {
            // ������ת����
            cv::Point2f center(image.cols / 2.0, image.rows / 2.0);
            cv::Mat rotation_matrix = cv::getRotationMatrix2D(center, angle, 1.0);

            // ִ����ת
            cv::Mat rotated_image;
            cv::warpAffine(image, rotated_image, rotation_matrix, image.size());

            // Ӧ������
            cv::Mat masked_image;
            rotated_image.copyTo(masked_image, mask);
            
            cv::Mat template_mask;
            template_image.copyTo(template_mask, mask);


            // ������ģ��ͼ������ƶ�
            double similarity = calculateSimilarity(template_mask, masked_image);

            // ����������ƶȺͶ�Ӧ�ĽǶ�
            if (similarity > max_similarity) {
                max_similarity = similarity;
                max_similarity_angle = angle;
            }
        }

        // ��ȡ����ʱ��
        auto end_time = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double, std::milli> elapsed = end_time - start_time;

        // ���������ƶȺͶ�Ӧ�ĽǶ�
        std::cout << "Maximum similarity: " << max_similarity << " at angle: " << max_similarity_angle << " degrees" << std::endl;
        // �����ʱ
        std::cout << "Elapsed time: " << elapsed.count() << " ms" << std::endl;
    }


    void fangiangtest() {
        std::string image_path = "point.png";      // �滻Ϊ���ԭʼͼƬ·��
        std::string mask_path = "mask.png";        // �滻Ϊ�������ͼƬ·��
        std::string template_path = "template.png"; // �滻Ϊ���ģ��ͼƬ·��
      
        while (1) {

            CapturePoint(image_path);
            rotateAndExtractWithMask(image_path, mask_path, template_path);
        }
        

    }
    // ��ȡ��Ϸ���ڵĺ�����ʹ�ô���������ͼƬ�������2��
    void CaptureGameWindowsWithInterval(int maxFrames) {
        int frameCount = 0;
        while (frameCount < maxFrames) {
            // �����ļ���
            std::string filename = std::to_string(frameCount) + ".png";

            // ���ý�ȡ��Ϸ���ڵĺ���
            CaptureGameWindow(filename);

            // �����Ϣ����ʾ�Ѿ���ȡ��һ��ͼƬ
            std::cout << "Captured " << filename << std::endl;

            // ˯��2��
            std::this_thread::sleep_for(std::chrono::seconds(3));

            // ����֡����
            ++frameCount;
        }
    }
    void findAndMarkSmallMapPosition(const std::string& bigMapPath, const std::string& smallMapPath) {
        // ���ش��ͼ��С��ͼ
        CaptureMiniMapEx(smallMapPath.c_str());
        cv::Mat bigMap = cv::imread(bigMapPath);
        cv::Mat smallMap = cv::imread(smallMapPath);
        if (bigMap.empty() || smallMap.empty()) {
            std::cerr << "Error: One of the images did not load properly." << std::endl;
            return;
        }

        // ת��Ϊ�Ҷ�ͼ
        cv::Mat grayBigMap, graySmallMap;
        cv::cvtColor(bigMap, grayBigMap, cv::COLOR_BGR2GRAY);
        cv::cvtColor(smallMap, graySmallMap, cv::COLOR_BGR2GRAY);

        // ����ORB�����
        cv::Ptr<cv::ORB> orb = cv::ORB::create();

        // ���ؼ��㲢����������
        std::vector<cv::KeyPoint> keypointsBigMap, keypointsSmallMap;
        cv::Mat descriptorsBigMap, descriptorsSmallMap;
        orb->detectAndCompute(grayBigMap, cv::noArray(), keypointsBigMap, descriptorsBigMap);
        orb->detectAndCompute(graySmallMap, cv::noArray(), keypointsSmallMap, descriptorsSmallMap);

        // ����ƥ����
        cv::Ptr<cv::DescriptorMatcher> matcher = cv::BFMatcher::create(cv::NORM_HAMMING);

        // ʹ��KNNƥ��
        std::vector<std::vector<cv::DMatch>> knn_matches;
        matcher->knnMatch(descriptorsSmallMap, descriptorsBigMap, knn_matches, 2);

        // ���ʼ���
        const float ratio_threshold = 0.75f;
        std::vector<cv::DMatch> good_matches;
        for (size_t i = 0; i < knn_matches.size(); i++) {
            if (knn_matches[i].size() > 1) {
                if (knn_matches[i][0].distance < ratio_threshold * knn_matches[i][1].distance) {
                    good_matches.push_back(knn_matches[i][0]);
                }
            }
        }

        // ����ƥ����
        cv::Mat img_matches;
        cv::drawMatches(smallMap, keypointsSmallMap, bigMap, keypointsBigMap, good_matches, img_matches,
            cv::Scalar::all(-1), cv::Scalar::all(-1), std::vector<char>(), cv::DrawMatchesFlags::NOT_DRAW_SINGLE_POINTS);

        // ��ʾƥ����
        cv::imshow("Matches", img_matches);

        // ���������ƥ�䣬���С��ͼ�ڴ��ͼ�ϵ�λ��
        if (!good_matches.empty()) {
            // ��ȡƥ����λ��
            std::vector<cv::Point2f> obj;
            std::vector<cv::Point2f> scene;
            for (size_t i = 0; i < good_matches.size(); i++) {
                obj.push_back(keypointsSmallMap[good_matches[i].queryIdx].pt);
                scene.push_back(keypointsBigMap[good_matches[i].trainIdx].pt);
            }

            // ����Homography
            cv::Mat H = cv::findHomography(obj, scene, cv::RANSAC);

            // ��Homography����С��ͼ���ĸ����ڴ��ͼ�ϵ�λ��
            std::vector<cv::Point2f> obj_corners(4);
            obj_corners[0] = cv::Point2f(0, 0);
            obj_corners[1] = cv::Point2f(smallMap.cols, 0);
            obj_corners[2] = cv::Point2f(smallMap.cols, smallMap.rows);
            obj_corners[3] = cv::Point2f(0, smallMap.rows);
            std::vector<cv::Point2f> scene_corners(4);
            cv::perspectiveTransform(obj_corners, scene_corners, H);
            // ��������С��ͼ�ڴ��ͼ�ϵ�λ��
            cv::line(bigMap, scene_corners[2] + cv::Point2f(smallMap.cols, 0), scene_corners[3] + cv::Point2f(smallMap.cols, 0), cv::Scalar(0, 255, 0), 4);
            cv::line(bigMap, scene_corners[3] + cv::Point2f(smallMap.cols, 0), scene_corners[0] + cv::Point2f(smallMap.cols, 0), cv::Scalar(0, 255, 0), 4);

            // ��ʾ��Ǻ�Ĵ��ͼ
            cv::imshow("Marked Big Map", bigMap);
        }
        else {
            std::cout << "No good matches found." << std::endl;
        }

        // �ȴ��û�������ر����д���
        cv::waitKey(0);
        cv::destroyAllWindows();
    }

    int ditu() {
        // ���ú������滻Ϊʵ�ʵĴ��ͼ��С��ͼ·��
        findAndMarkSmallMapPosition("bigmap.png", "minimap.png");
        return 0;
    }
 
};