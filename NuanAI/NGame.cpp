#include "NGame.h"

NGame::NGame()
{
    dm_path = "xd47243.dll";
    reg_path = "Go.dll";
}

bool NGame::Initialize(const string& windowName)
{
    DWORD dm_hmodule = LoadDm(dm_path.c_str());
    if (dm_hmodule == NULL) { return 0; }
    HMODULE go_hmodule = LoadLibraryA(reg_path.c_str());
    GoFunProc GoFun = NULL;
    GoFun = (GoFunProc)GetProcAddress(go_hmodule, "Go");
    if (GoFun == NULL) { return 0; }
    GoFun(dm_hmodule);
    dm = new dmsoft();
    long nret = dm->Reg("", "");

    if (nret == 1)
    {
        logger.info("大漠注册成功");

    }
    else
    {
		logger.error("大漠注册失败");
  
        // 释放资源
        delete dm;
        FreeLibrary(go_hmodule);
        FreeLibrary((HMODULE)dm_hmodule);
        return 0;
    }

    // 查找游戏窗口
    gameHwnd = (HWND)dm->FindWindowW("", windowName.c_str());
    if (gameHwnd == NULL) {
		logger.info("未找到目标窗口！");
       
        delete dm;
        FreeLibrary(go_hmodule);
        FreeLibrary((HMODULE)dm_hmodule);
        return false;
    }


    // 获取窗口位置
    //不同游戏根据小地图位置进行偏移计算
    if (GetWindowRect(gameHwnd, &gameWindowRect)) {
		logger.info("获取游戏窗口位置成功！");
		logger.info("开始初始化坐标");
		logger.info("初始化坐标成功");
		logger.info("开始初始化小地图 ");

        miniMapRect.left = gameWindowRect.left + 75;
        miniMapRect.top = gameWindowRect.top + 65;
        miniMapRect.right = gameWindowRect.left + 290;
        miniMapRect.bottom = gameWindowRect.top + 280;
		logger.info("小地图初始化成功 ");
		logger.info("开始初始化指针 ");
        pointRect.left = gameWindowRect.left + 170;
        pointRect.top = gameWindowRect.top + 155;
        pointRect.right = gameWindowRect.left + 200;
        pointRect.bottom = gameWindowRect.top + 185;
		logger.info("指针初始化成功 ");
      
    }
    else {
		logger.error("获取游戏窗口位置失败！");
        return false;
    }

    return true;
}

bool NGame::CaptureGameWindow(const std::string& filename)
{
    return false;
}

bool NGame::CaptureMiniMap(const std::string& filename)
{

    if (dm->Capture(miniMapRect.left, miniMapRect.top, miniMapRect.right, miniMapRect.bottom, filename.c_str())) {
        
		logger.info("捕获小地图成功，保存为minimap.bmp");
        return true;
    }
    else {
        logger.error("捕获小地图失败");
        return false;
    }
}

bool NGame::CaptureMiniMapEx(const std::string& filename)
{
    if (dm->Capture(miniMapRect.left + 45, miniMapRect.top + 35, miniMapRect.left + 180, miniMapRect.top + 170, filename.c_str())) {
       
		logger.info("捕获小地图成功，保存为minimap.bmp");
        return true;
    }
    else {
        logger.error("捕获小地图失败");
        return false;
    }
}

bool NGame::CapturePoint(const std::string& filename)
{
    if (dm->Capture(pointRect.left, pointRect.top, pointRect.right, pointRect.bottom, filename.c_str())) {
        
		logger.info("捕获指针成功，保存为point.bmp");
        return true;
    }
    else {
        logger.error("捕获指针失败");
        return false;
    }
}

void NGame::setCharacterAngle(double newAngle)
{
    std::lock_guard<std::mutex> lock(angleMutex);
    characterAngle = newAngle;
}

double NGame::getCharacterAngle() const
{
    std::lock_guard<std::mutex> lock(angleMutex);
    return characterAngle;
}

void NGame::createAndSaveMaskFromImage(const std::string& image_path, const std::string& mask_path)
{
    // 读取带有透明通道的图片
    cv::Mat image = cv::imread(image_path, cv::IMREAD_UNCHANGED);

    // 检查图片是否成功读取
    if (image.empty()) {
        std::cerr << "Error: Unable to load image at path " << image_path << std::endl;
        return;
    }

    // 检查图片是否有透明通道
    if (image.channels() < 4) {
        std::cerr << "Error: Image does not have an alpha channel" << std::endl;
        return;
    }

    // 提取透明通道
    std::vector<cv::Mat> channels;
    cv::split(image, channels);
    cv::Mat alpha_channel = channels[3]; // Alpha通道通常是在最后一个通道

    // 创建掩码图像，透明部分填充黑色，不透明部分填充白色
    cv::Mat mask;
    cv::threshold(alpha_channel, mask, 0, 255, cv::THRESH_BINARY); // 二值化处理

    // 保存掩码图像
    if (!cv::imwrite(mask_path, mask)) {
		logger.error("Error: Unable to save mask image at path " + mask_path);
        return;
    }
	logger.info("Mask image saved successfully at " + mask_path);
   
}

int NGame::masktest()
{
    std::string image_path = "template.png"; // 替换为你的图片路径
    std::string mask_path = "mask.png";   // 替换为你要保存掩码图片的路径

    createAndSaveMaskFromImage(image_path, mask_path);

    return 0;
}

double NGame::calculateSimilarity(const cv::Mat& template_image, const cv::Mat& masked_image)
{
    // 检查输入图像是否为空
    if (template_image.empty() || masked_image.empty()) {
		logger.error("图像不能为空");
       
        return -1.0;
    }

    // 检查图像尺寸是否相同
    if (template_image.size() != masked_image.size()) {
		logger.error("模板图像和掩码图像应该具有相同的大小");
      
        return -1.0;
    }

    // 检查图像类型是否相同
    if (template_image.type() != masked_image.type()) {
		logger.error("模板图像和掩码图像应该具有相同的类型");
      
        return -1.0;
    }

    // 定义结果矩阵
    cv::Mat result;
    int match_method = cv::TM_CCOEFF_NORMED; // 使用归一化相关系数

    // 进行模板匹配
    cv::matchTemplate(masked_image, template_image, result, match_method);

    // 获取最佳匹配的相似度值
    double minVal, maxVal;
    cv::Point minLoc, maxLoc;
    cv::minMaxLoc(result, &minVal, &maxVal, &minLoc, &maxLoc);

    // 返回最大相似度值
    return maxVal;
}

void NGame::rotateAndExtractWithMask(const std::string& image_path, const std::string& mask_path, const std::string& template_path)
{
    // 读取原始图像
    cv::Mat image = cv::imread(image_path, cv::IMREAD_COLOR);
    // 读取掩码图像
    cv::Mat mask = cv::imread(mask_path, cv::IMREAD_GRAYSCALE);
    // 读取模板图像
    cv::Mat template_image = cv::imread(template_path, cv::IMREAD_COLOR);

    // 检查图像和掩码是否成功读取
    if (image.empty() || mask.empty() || template_image.empty()) {
        std::cerr << "Error: Unable to load image, mask or template" << std::endl;
        return;
    }

    // 确保掩码是二值的
    cv::threshold(mask, mask, 128, 255, cv::THRESH_BINARY);

    // 预处理：转换为灰度图像（如果需要）
    cv::Mat image_gray, template_gray;
    cv::cvtColor(image, image_gray, cv::COLOR_BGR2GRAY);
    cv::cvtColor(template_image, template_gray, cv::COLOR_BGR2GRAY);




    // 初始化变量以存储最大相似度和对应的角度
    double max_similarity = 0.0;
    int max_similarity_angle = 0;

    // 获取开始时间
    auto start_time = std::chrono::high_resolution_clock::now();

    // 循环旋转原始图像，每10度旋转一次，总共旋转360度
    for (int angle = 0; angle < 360; angle += 10) {
        // 计算旋转矩阵
        cv::Point2f center(image.cols / 2.0, image.rows / 2.0);
        cv::Mat rotation_matrix = cv::getRotationMatrix2D(center, angle, 1.0);

        // 执行旋转
        cv::Mat rotated_image;
        cv::warpAffine(image, rotated_image, rotation_matrix, image.size());

        // 应用掩码
        cv::Mat masked_image;
        rotated_image.copyTo(masked_image, mask);

        cv::Mat template_mask;
        template_image.copyTo(template_mask, mask);


        // 计算与模板图像的相似度
        double similarity = calculateSimilarity(template_mask, masked_image);

        // 更新最大相似度和对应的角度
        if (similarity > max_similarity) {
            max_similarity = similarity;
            max_similarity_angle = angle;
        }
    }

    // 获取结束时间
    auto end_time = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double, std::milli> elapsed = end_time - start_time;

    // 输出最大相似度和对应的角度
	logger.info("最大相似度： " + std::to_string(max_similarity) + "角度 :" + std::to_string(max_similarity_angle) + " degrees");
	logger.info("耗时： " + std::to_string(elapsed.count()) + " ms");
}

void NGame::fangiangtest()
{
    std::string image_path = "../data/Img/point.png";      // 替换为你的原始图片路径
    std::string mask_path = "../data/Img/mask.png";        // 替换为你的掩码图片路径
    std::string template_path = "../data/Img/template.png"; // 替换为你的模板图片路径

    while (1) {

        CapturePoint(image_path);
        rotateAndExtractWithMask(image_path, mask_path, template_path);
        Sleep(500);
    }
}
