#ifndef _COURSE_INFO_HPP
#define _COURSE_INFO_HPP

#include <string>

// define a list of course fields
enum CourseField{
    PROF,
    NAME,
    CODE,
    CREDIT,
    DAY,
    UNDEFINED,
    ALL

};

// It maintain all the course field informations for each single course code. 
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

        void InitWith(const char* info);    // update the attributes above 
        void ClearField();
        void CopyFullInfo(char* _dst);      // copy all the course information to the buffer "_dst"

};



#endif