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
        logger.info("��Įע��ɹ�");

    }
    else
    {
		logger.error("��Įע��ʧ��");
  
        // �ͷ���Դ
        delete dm;
        FreeLibrary(go_hmodule);
        FreeLibrary((HMODULE)dm_hmodule);
        return 0;
    }

    // ������Ϸ����
    gameHwnd = (HWND)dm->FindWindowW("", windowName.c_str());
    if (gameHwnd == NULL) {
		logger.info("δ�ҵ�Ŀ�괰�ڣ�");
       
        delete dm;
        FreeLibrary(go_hmodule);
        FreeLibrary((HMODULE)dm_hmodule);
        return false;
    }


    // ��ȡ����λ��
    //��ͬ��Ϸ����С��ͼλ�ý���ƫ�Ƽ���
    if (GetWindowRect(gameHwnd, &gameWindowRect)) {
		logger.info("��ȡ��Ϸ����λ�óɹ���");
		logger.info("��ʼ��ʼ������");
		logger.info("��ʼ������ɹ�");
		logger.info("��ʼ��ʼ��С��ͼ ");

        miniMapRect.left = gameWindowRect.left + 75;
        miniMapRect.top = gameWindowRect.top + 65;
        miniMapRect.right = gameWindowRect.left + 290;
        miniMapRect.bottom = gameWindowRect.top + 280;
		logger.info("С��ͼ��ʼ���ɹ� ");
		logger.info("��ʼ��ʼ��ָ�� ");
        pointRect.left = gameWindowRect.left + 170;
        pointRect.top = gameWindowRect.top + 155;
        pointRect.right = gameWindowRect.left + 200;
        pointRect.bottom = gameWindowRect.top + 185;
		logger.info("ָ���ʼ���ɹ� ");
      
    }
    else {
		logger.error("��ȡ��Ϸ����λ��ʧ�ܣ�");
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
        
		logger.info("����С��ͼ�ɹ�������Ϊminimap.bmp");
        return true;
    }
    else {
        logger.error("����С��ͼʧ��");
        return false;
    }
}

bool NGame::CaptureMiniMapEx(const std::string& filename)
{
    if (dm->Capture(miniMapRect.left + 45, miniMapRect.top + 35, miniMapRect.left + 180, miniMapRect.top + 170, filename.c_str())) {
       
		logger.info("����С��ͼ�ɹ�������Ϊminimap.bmp");
        return true;
    }
    else {
        logger.error("����С��ͼʧ��");
        return false;
    }
}

bool NGame::CapturePoint(const std::string& filename)
{
    if (dm->Capture(pointRect.left, pointRect.top, pointRect.right, pointRect.bottom, filename.c_str())) {
        
		logger.info("����ָ��ɹ�������Ϊpoint.bmp");
        return true;
    }
    else {
        logger.error("����ָ��ʧ��");
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
		logger.error("Error: Unable to save mask image at path " + mask_path);
        return;
    }
	logger.info("Mask image saved successfully at " + mask_path);
   
}

int NGame::masktest()
{
    std::string image_path = "template.png"; // �滻Ϊ���ͼƬ·��
    std::string mask_path = "mask.png";   // �滻Ϊ��Ҫ��������ͼƬ��·��

    createAndSaveMaskFromImage(image_path, mask_path);

    return 0;
}

double NGame::calculateSimilarity(const cv::Mat& template_image, const cv::Mat& masked_image)
{
    // �������ͼ���Ƿ�Ϊ��
    if (template_image.empty() || masked_image.empty()) {
		logger.error("ͼ����Ϊ��");
       
        return -1.0;
    }

    // ���ͼ��ߴ��Ƿ���ͬ
    if (template_image.size() != masked_image.size()) {
		logger.error("ģ��ͼ�������ͼ��Ӧ�þ�����ͬ�Ĵ�С");
      
        return -1.0;
    }

    // ���ͼ�������Ƿ���ͬ
    if (template_image.type() != masked_image.type()) {
		logger.error("ģ��ͼ�������ͼ��Ӧ�þ�����ͬ������");
      
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

void NGame::rotateAndExtractWithMask(const std::string& image_path, const std::string& mask_path, const std::string& template_path)
{
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
	logger.info("������ƶȣ� " + std::to_string(max_similarity) + "�Ƕ� :" + std::to_string(max_similarity_angle) + " degrees");
	logger.info("��ʱ�� " + std::to_string(elapsed.count()) + " ms");
}

void NGame::fangiangtest()
{
    std::string image_path = "../data/Img/point.png";      // �滻Ϊ���ԭʼͼƬ·��
    std::string mask_path = "../data/Img/mask.png";        // �滻Ϊ�������ͼƬ·��
    std::string template_path = "../data/Img/template.png"; // �滻Ϊ���ģ��ͼƬ·��

    while (1) {

        CapturePoint(image_path);
        rotateAndExtractWithMask(image_path, mask_path, template_path);
        Sleep(500);
    }
}
