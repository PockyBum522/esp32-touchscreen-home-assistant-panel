#include "Logger.h"

Logger::Logger(LogLevel logLevel, bool *enabled)
{
    _logLevel = logLevel;

    if (_logLevel != LoggingDisabled && enabled)
        Serial.begin(115200);

    if (enabled)
    {
        if (_logLevel == LogLevel::Uninitialized)
            Serial.println("ERROR LOG LEVEL WAS UNINITIALIZED, PLEASE FIX AND RE-UPLOAD");
    }

    _debugSerialOn = enabled;
}

void Logger::Debug(const String &message, const Logger::FileAndLine &fileAndLine)
{
    if (_logLevel >= LogLevel::Debug)
    {
        WriteLogLine(message, "DBG", fileAndLine);
    }
}

void Logger::Information(const String &message, const Logger::FileAndLine &fileAndLine)
{
    if (_logLevel >= LogLevel::Information)
    {
        WriteLogLine(message, "INF", fileAndLine);
    }
}

void Logger::Warning(const String &message, const Logger::FileAndLine &fileAndLine)
{
    if (_logLevel >= LogLevel::Warning)
    {
        WriteLogLine(message, "WRN", fileAndLine);
    }
}

void Logger::Error(const String &message, const Logger::FileAndLine &fileAndLine)
{
    if (_logLevel >= LogLevel::Error)
    {
        WriteLogLine(message, "ERR", fileAndLine);
    }
}

void Logger::Fatal(const String &message, const Logger::FileAndLine &fileAndLine)
{
    if (_logLevel >= LogLevel::Fatal)
    {
        WriteLogLine(message, "FTL", fileAndLine);
    }
}

void Logger::WriteLogLine(const String &message, const String &logLevelAbbreviation, const FileAndLine &originalFileAndLine)
{
    String formattedFileAndLine = FormatFileAndLine(originalFileAndLine);

    String paddedMillis = GetPaddedMillis(8);


    if (_debugSerialOn)
    {
        Serial.print("[");
        Serial.print(paddedMillis);
        Serial.print("]");

        Serial.print("[");
        Serial.print(formattedFileAndLine);
        Serial.print("] ");

        Serial.print(logLevelAbbreviation);
        Serial.print(": ");

        Serial.println(message);
    }
}

String Logger::FormatFileAndLine(const FileAndLine &originalFileAndLine)
{
    String formattedFileAndLine = originalFileAndLine.file.c_str();
    formattedFileAndLine += ":";
    formattedFileAndLine += originalFileAndLine.line;

    return formattedFileAndLine;
}

String Logger::GetPaddedMillis(int totalCharsToPadTo)
{
    unsigned int millisCurrentLen = (new String(millis()))->length();

    if (millisCurrentLen >= totalCharsToPadTo)
        return String(millis());

    String returnString = "";

    for (unsigned int i = millisCurrentLen; i < totalCharsToPadTo; i++)
    {
        returnString += " ";
    }

    returnString += String(millis());

    return returnString;
}
