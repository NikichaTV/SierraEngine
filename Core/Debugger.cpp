//
// Created by Nikolay Kanchevski on 28.09.22.
//

#include "Debugger.h"

#ifdef __GNUG__
    #include <cxxabi.h>
#endif

#if _WIN32
    #define DEFAULT() (SetConsoleTextAttribute(hConsole, 7))
    #define BLUE() (SetConsoleTextAttribute(hConsole, 1))
    #define GREEN() (SetConsoleTextAttribute(hConsole, 2))
    #define YELLOW() (SetConsoleTextAttribute(hConsole, 14))
    #define RED() (SetConsoleTextAttribute(hConsole, 4))
#else
    #define DEFAULT() (printf("\e[0;39m"))
    #define BLUE() (printf("\e[0;34m"))
    #define GREEN() (printf("\e[0;32m"))
    #define YELLOW() (printf("\e[0;33m"))
    #define RED() (printf("\e[0;31m"))
#endif

namespace Sierra::Core
{

#if DEBUG
    static const bool DEBUG_MODE = true;
#else
    static const bool DEBUG_MODE = false;
#endif

#if _WIN32
    HANDLE Debugger::hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
#endif

    void Debugger::DisplayInfo(const String &message)
    {
        BLUE();
        std::cout << "[i] " << message << ".\n";
        DEFAULT();
    }

    void Debugger::DisplaySuccess(const String &message)
    {
        GREEN();
        std::cout << "[+] " << message << ".\n";
        DEFAULT();
    }

    void Debugger::ThrowWarning(const String &message)
    {
        YELLOW();
        std::cout << "[!] " << message << "!\n";
        DEFAULT();
    }

    void Debugger::ThrowError(const String &message)
    {
        RED();
        std::cout << "[-] " << message << "!\n";
        if (DEBUG_MODE) throw std::runtime_error("Program execution failed miserably!");
        DEFAULT();
    }

    VkBool32 Debugger::ValidationCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, VkDebugUtilsMessageTypeFlagsEXT messageType, const VkDebugUtilsMessengerCallbackDataEXT *pCallbackData, void *pUserData)
    {
        switch (messageSeverity)
        {
            case VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT:
                ThrowWarning(String(pCallbackData->pMessage).substr(0, strlen(pCallbackData->pMessage) - 1));
                break;
            case VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT:
                ThrowError(String(pCallbackData->pMessage).substr(0, strlen(pCallbackData->pMessage) - 1));
                break;
            default:
                break;
        }

        return VK_FALSE;
    }

#ifdef __GNUG__

    String Debugger::Demangle(const char *name)
    {
        int status = -4; // Some arbitrary value to eliminate the compiler warning

        std::unique_ptr<char, void (*)(void *)> res
        {
            abi::__cxa_demangle(name, NULL, NULL, &status),
            std::free
        };

        return (status == 0) ? res.get() : name;
    }

#else
    // Do nothing if not g++
    String Debugger::Demangle(const char* tag)
    {
        return tag;
    }
#endif

    Debugger::FunctionProfiler::FunctionProfiler(const char *name)
        : name(name)
    {
        startTimepoint = std::chrono::steady_clock::now();
    }

    Debugger::FunctionProfiler::~FunctionProfiler()
    {
        if (!stopped) Stop();
    }

    void Debugger::FunctionProfiler::Stop()
    {
        auto endTimePoint = std::chrono::steady_clock::now();
        auto highResStart = std::chrono::duration_cast<std::chrono::milliseconds>(startTimepoint.time_since_epoch()).count();
        auto highResEnd = std::chrono::duration_cast<std::chrono::milliseconds>(endTimePoint.time_since_epoch()).count();
        auto elapsedTime = highResEnd - highResStart;

        std::string nameString = std::string(name);

        String templateData = "";
        uint templateIndex = nameString.substr(0, nameString.find_first_of('(')).find_first_of('<');
        if (templateIndex < nameString.size())
        {
            uint endTemplateIndex = nameString.find_first_of('>');
            templateData = nameString.substr(templateIndex, endTemplateIndex - templateIndex + 1);

            nameString = nameString.substr(0, templateIndex) + nameString.substr(endTemplateIndex + 1, nameString.size() - endTemplateIndex - 1);
        }

        if (!templateData.empty())
        {
            uint leftIndex;
            uint rightIndex = 0;

            String formattedTemplateData = "";
            while (rightIndex < templateData.size())
            {
                if (templateData.find(':') >= templateData.size()) break;

                if (templateData[rightIndex] == ',' || templateData[rightIndex] == '>')
                {
                    leftIndex = templateData.substr(0, rightIndex).find_last_of(':') + 1;
                    formattedTemplateData += templateData.substr(leftIndex, rightIndex - leftIndex) + ", ";
                }

                rightIndex++;
            }

            formattedTemplateData = '<' + formattedTemplateData.substr(0, formattedTemplateData.size() - 2  ) + ">::";
            templateData = formattedTemplateData;
        }

        uint braceIndex = nameString.find_last_of('(');

        uint j = braceIndex;
        uint columnCounter = 0;

        uint firstColumnIndex = 0;
        while (columnCounter < 3)
        {
            j--;

            if (j == -1)
            {
                break;
            }
            else if (nameString[j] == ':')
            {
                if (firstColumnIndex == 0)
                {
                    firstColumnIndex = j + 1;
                }

                columnCounter++;
            }
        }

        nameString = nameString.substr(0, firstColumnIndex) + templateData + nameString.substr(firstColumnIndex, nameString.size() - firstColumnIndex);

        braceIndex += templateData.size();

        nameString = nameString.substr(j + 1, braceIndex - j) + ')';

        ASSERT_INFO_FORMATTED("Method {0} took {1}ms", nameString, elapsedTime);

        stopped = true;
    }

}
