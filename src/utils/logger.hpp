//
// Created by Sonni on 03/08/2019.
//

#ifndef INC_3D_ENGINE_LOGGER_HPP
#define INC_3D_ENGINE_LOGGER_HPP

#include <string>
#include <iostream>
#include <sstream>
#include <fstream>
#include <chrono>

namespace logger {
    static std::stringstream buffer;

    enum type {MSG, WARNING, ERR};
    enum output {COUT, LOCAL_FILE, NONE};
    static output out_mode = COUT;

    static void print_log(const type& t, const std::string& msg)
    {
        std::string final_log = "";
        if (t == WARNING)
        {
            final_log = "WARNING: ";
        }
        else if (t == ERR)
        {
            final_log = "ERROR: ";
        }


        auto time = std::chrono::system_clock::now();
        std::time_t time_t = std::chrono::system_clock::to_time_t(time);
        std::string cur_time(std::ctime(&time_t));

        final_log += "<" + cur_time.substr(0, cur_time.length() -1) + "> " + msg;

        switch (out_mode)
        {
            case COUT: {
                std::cout << final_log << std::endl;
                break;
            }
            case LOCAL_FILE: {
                std::fstream log;
                log.open("log.txt", std::fstream::app);

                log << final_log << std::endl;
                break;
            }
            default: {
                break;
            }
        }
    }

    template <typename T>
    void log_msg(T t)
    {
        buffer << t;

        print_log(MSG, buffer.str());

        buffer.str("");
    }

    template<typename T, typename... Args>
    void log_msg(T t, Args... args)
    {
        buffer << t;
        log_msg(args...);
    }

    template <typename T>
    void log_warning(T t)
    {
        buffer << t;

        print_log(WARNING, buffer.str());

        buffer.str("");
    }

    template<typename T, typename... Args>
    void log_warning(T t, Args... args)
    {
        buffer << t;
        log_warning(args...);
    }

    template <typename T>
    void log_err(T t)
    {
        buffer << t;

        print_log(ERR, buffer.str());
        buffer.str("");
    }

    template<typename T, typename... Args>
    void log_err(T t, Args... args)
    {
        buffer << t;
        log_err(args...);
    }


}


#endif //INC_3D_ENGINE_LOGGER_HPP
