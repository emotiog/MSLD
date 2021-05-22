//
//  DescManager.hpp
//  OpenCVTest
//
//  Created by sungphill on 2020/04/03.
//  Copyright © 2020 sungphill. All rights reserved.
//

#ifndef DescManager_hpp
#define DescManager_hpp

#include "system.h"

struct Scene{
    cv::Mat img;
    string path;
    vector<SEGMENT> segments;
};

class DescManager
{
private:
    DescManager() {}
    static DescManager* instance_;
    
    vector<Scene> scenes;
    string filepath;
    
public:
    static DescManager& instance();
    void setPath(string path);
    
    void clear();
    int count();
    void append(const cv::Mat &src, const string imgPath, const vector<SEGMENT> &srcDesc);
    
    void Read();
    void Write();
    void Delete();
    int Match(vector<SEGMENT> *src, string &path);
};
    
#endif /* DescManager_hpp */
