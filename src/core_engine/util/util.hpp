#ifndef GAME_ENGINE_UTIL_H
#define GAME_ENGINE_UTIL_H

#include <string>
#include <vector>

class util
{
public:
    static std::vector<std::string> split_string(const std::string &s, char delim)
    {
        std::vector<std::string> elems;

        const char* cstr = s.c_str();
        unsigned int strLength = (unsigned int)s.length();
        unsigned int start = 0;
        unsigned int end = 0;

        while(end <= strLength)
        {
            while(end <= strLength)
            {
                if(cstr[end] == delim)
                    break;
                end++;
            }

            elems.push_back(s.substr(start, end - start));
            start = end + 1;
            end = start;
        }

        return elems;
    }
};


#endif //GAME_ENGINE_UTIL_H
