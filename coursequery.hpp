#ifndef _COURSE_QUERY_HPP
#define _COURSE_QUERY_HPP

#include "courseinfo.hpp"

class CourseQuery
{

    public:
        CourseQuery(const char* file);
        ~CourseQuery();
        CourseField WhatCategory(const char* _src);
        void Search(const char *code, const char* category, char *_dst);
    
    private:
        const char* _filePath;
    
};

#endif