#include "coursequery.hpp"
#include <string.h> 


CourseQuery:: CourseQuery(const char* file) : _filePath(file) 
{}

CourseQuery:: ~CourseQuery()
{}

// find matched course field based on "_src"
CourseField
CourseQuery:: WhatCategory(const char* _src) 
{
    if(strcmp(_src, "Credit") == 0)
        return CREDIT;
    else if(strcmp(_src, "Professor") == 0)
        return PROF;
    else if(strcmp(_src, "Days") == 0)
        return DAY;
    else if(strcmp(_src, "CourseName") == 0)
        return NAME;
    else if(strcmp(_src, "All") == 0)
        return ALL;
    else 
        return UNDEFINED;
}

// providing the course code and information category, it search through the text file ;
// the result is saved to the buffer "_dst" .
void
CourseQuery:: Search(const char *code, const char* category, char *_dst)
{
    FILE *txt;
    txt = fopen(_filePath, "r");
    if(txt == NULL){
        printf("Server cannot open file: %s\n", strerror(errno));
    } else {
        
        char buf[200] = {0};       
        while(fgets(buf, 200, txt) != NULL) 
        {
            CourseField c_type = WhatCategory(category);
            CourseInfo info(buf);

            if(info.hasCode(code)) {
                info.getField(c_type, _dst);
                break;
            }
        }

        fclose(txt);
    }

}