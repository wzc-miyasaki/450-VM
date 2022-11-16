#ifndef _COURSE_INFO_HPP
#define _COURSE_INFO_HPP

#include <string>

enum CourseField{
    PROF,
    NAME,
    CODE,
    CREDIT,
    DAY,
    UNDEFINED
};

class CourseInfo
{
    public:
        CourseInfo(const char* full_info);
        bool hasCode(const char* _code);
        void getField(CourseField field, char *_dst);
    private:
        char prof[64];
        char name[64];
        char code[8];
        char credit[4];
        char day[20];

        void InitWith(const char* info);
        void ClearField();

};



#endif