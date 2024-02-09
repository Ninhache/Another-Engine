#pragma once

#include <iostream>
#include <fstream>
#include <chrono>

class Logger
{
public:
    /**
     * @brief Creates a new Logger,
     * The logger is an util class to handle logging events, all the logs events are writed in a file called "log.txt" if the boolean "m_fileLog" equals true, also it can be printed in the console by putting the "m_consoleLog" to true
     *
     * The class is used in the project but, not enough.
     */
    Logger() {
        m_logfile.open("log.txt", std::ios::out | std::ios::trunc);
        if (!m_logfile.is_open()) {
            std::cerr << "Error opening log file" << std::endl;
        }

        log("Logger loaded");
    }

    /**
     * @brief Destroy the logger by closing the log file
     */
    ~Logger() {
        m_logfile.close();
    }

    enum LogLevel {
        DEBUG = 1,
        INFO,
        WARNING,
        ERROR,
        CRITICAL,
        UNKNOWN
    };

    void log(LogLevel level, std::string message) {
        auto now = std::chrono::system_clock::now();
        auto now_c = std::chrono::system_clock::to_time_t(now);
        std::tm *tm = std::localtime(&now_c);

        char buffer[9];
        std::strftime(buffer, sizeof(buffer), "%H:%M:%S", tm);

        if (level <= DEBUG || level >= CRITICAL) {
            level = LogLevel::UNKNOWN;
        }

        std::string logMessage = std::string(LogLevelNames[level]) + " " + std::string(buffer) + " " + message + "\n";

        if (m_fileLog) {
            m_logfile << logMessage;
            m_logfile.flush();
        }

        if (m_consoleLog) {
            std::cout << logMessage;
        }
    }

    /**
     * @brief Log the message into the logfile using the INFO level
     *
     * @param message - The message to write into the streams
     */
    void log(std::string message) {
        log(LogLevel::INFO, message);
    }

    /**
     * @brief Log the message into the logfile using the ERROR level
     *
     * @param message - The message to write into the streams
     */
    void error(std::string message) {
        log(LogLevel::ERROR, message);
    }

    /**
     * @brief Log the message into the logfile using the WARN level
     *
     * @param message - The message to write into the streams
     */
    void warn(std::string message) {
        log(LogLevel::WARNING, message);
    }

    /**
     * @brief Log the message into the logfile using the CRITICAL level
     *
     * @param message - The message to write into the streams
     */
    void critical(std::string message) {
        log(LogLevel::CRITICAL, message);
    }

    /**
     * @brief Log the message into the logfile using the DEBUG level
     *
     * @param message - The message to write into the streams
     */
    void debug(std::string message) {
        log(LogLevel::DEBUG, message);
    }

    /**
     * @brief Change the "m_consoleLog" value
     *
     * If it's true, the console will print all the logs
     * If it's false, the console won't print
     *
     * @param value - The new value
     */
    void setConsoleLog(bool value) { this->m_consoleLog = value; }

    /**
     * @brief Change the "setFileLog" value
     *
     * If it's true, the file will be filled with all the logs
     * If it's false, the file won't be filled
     *
     * @param value - The new value
     */
    void setFileLog(bool value) { this->m_fileLog = value; }

private:
    bool m_consoleLog = true;
    bool m_fileLog = true;
    std::ofstream m_logfile;

    inline static const char* const LogLevelNames[6] = {
        "",
        "[DEBUG]",
        "[INFO ]",
        "[WARNG]",
        "[ERROR]",
        "[CRITC]"
    };

};

inline Logger logger;

// const char* const Logger::LogLevelNames[] = {
//     "",
//     "[DEBUG]",
//     "[INFO]",
//     "[WARNING]",
//     "[ERROR]",
//     "[CRITICAL]"
// };
