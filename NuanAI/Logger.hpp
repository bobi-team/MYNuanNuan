#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <ctime>

class Logger {
public:
    enum Level {
        CINFO,
        CWARNING,
        CERROR
    };

    // �޸Ĺ��캯�������Ĭ�ϲ���
    Logger(const std::string& filename = "log.txt")
        : logFile(filename, std::ios::app), enableConsoleOutput(true), enableFileOutput(true) {
        if (!logFile.is_open()) {
            std::cerr << "�޷�����־�ļ�: " << filename << std::endl;
        }
    }

    ~Logger() {
        if (logFile.is_open()) {
            logFile.close();
        }
    }

    void setConsoleOutputEnabled(bool enabled) {
        enableConsoleOutput = enabled;
    }

    void setFileOutputEnabled(bool enabled) {
        enableFileOutput = enabled;
    }

    void log(const std::string& message, Level level) {
        std::string levelStr;
        switch (level) {
        case CINFO:
            levelStr = "INFO";
            break;
        case CWARNING:
            levelStr = "WARNING";
            break;
        case CERROR:
            levelStr = "ERROR";
            break;
        }

        std::string formattedMessage = formatMessage(message, levelStr);

        if (enableConsoleOutput) {
            std::cout << formattedMessage << std::endl;
        }
        if (enableFileOutput && logFile.is_open()) {
            logFile << formattedMessage << std::endl;
        }
    }

    void info(const std::string& message) {
        log(message, CINFO);
    }

    void warning(const std::string& message) {
        log(message, CWARNING);
    }

    void error(const std::string& message) {
        log(message, CERROR);
    }

private:
    std::ofstream logFile;
    bool enableConsoleOutput;
    bool enableFileOutput;

    std::string getCurrentTime() {
        std::time_t now = std::time(nullptr);
        struct tm buf;
        localtime_s(&buf, &now); // ʹ��localtime_s����localtime
        char timeStr[100] = { 0 };
        std::strftime(timeStr, sizeof(timeStr), "%Y-%m-%d %H:%M:%S", &buf);
        return std::string(timeStr);
    }

    std::string formatMessage(const std::string& message, const std::string& levelStr) {
        std::ostringstream oss;
        oss << "[" << getCurrentTime() << "] [" << levelStr << "] " << message;
        return oss.str();
    }
};
