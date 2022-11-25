#include "courseinfo.hpp"
#include "global.hpp"

CourseInfo:: CourseInfo(const char* full_info)
{
    ClearField();
    InitWith(full_info);
}

bool 
CourseInfo:: hasCode(const char* _code)
{
    return strcmp(code, _code) == 0;
}

void
CourseInfo:: getField(CourseField field, char *_dst)
{
    memset(_dst, 0, strlen(_dst));
    char *ptr;
    switch(field)
    {       
        case PROF: ptr = prof; break;
        case NAME: ptr = name; break;
        case CODE: ptr = code; break;
        case CREDIT: ptr = credit; break;
        case DAY: ptr = day; break;
        default: ptr = NULL; break;
    }
    if(ptr != NULL)
        memcpy(_dst, ptr, strlen(ptr));
    else if(field == ALL)
        CopyFullInfo(_dst);
}


void 
CourseInfo:: InitWith(const char* info)
{
    char *ptr = code;
    int ed = strlen(info);
    int nextField = 1;
    int idx = 0;
    char c;
    while(idx < ed)
    {
        c = *(info + idx);
        if(c == '\r' || c == '\n')
            break;
        else if(c == ','){
            switch (nextField)
            {
                case 1: ptr = credit; break;
                case 2: ptr = prof; break;
                case 3: ptr = day; break;
                case 4: ptr = name; break;
                default:    break;
            }
            ++nextField;
        
        } else {
            *ptr = c;
            ++ptr;
       } 
       ++idx;
    }
}

void
CourseInfo:: ClearField()
{
    memset(prof, 0, sizeof(prof));
    memset(name, 0, sizeof(name));
    memset(code, 0, sizeof(code));
    memset(credit, 0, sizeof(credit));
    memset(day, 0, sizeof(day));
}

void 
CourseInfo:: CopyFullInfo(char* _dst)
{
    std::string str_cred(credit);
    std::string str_prof(prof);
    std::string str_name(name);
    std::string str_day(day);

    std::string info = str_cred + "," + str_prof + "," + str_day + "," + str_name;
    const char* source = info.c_str();
    memcpy(_dst, source, strlen(source)+1);
}