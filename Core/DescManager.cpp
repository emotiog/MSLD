//
//  DescManager.cpp
//  OpenCVTest
//
//  Created by sungphill on 2020/04/03.
//  Copyright © 2020 sungphill. All rights reserved.
//

#include "DescManager.h"
#include <fstream>

DescManager* DescManager::instance_ = nullptr;

DescManager& DescManager::instance() {
    if(instance_ == nullptr)
    {
        instance_ = new DescManager();
    }
    return *instance_;
}

void DescManager::clear(){
    for(auto s:scenes){
        s.segments.clear();
    }
    
    scenes.clear();
}

void DescManager::setPath(string path){
    filepath = path;
}

int DescManager::count(){
    return (int)scenes.size();
}

void DescManager::append(const cv::Mat &src, const string imgPath, const vector<SEGMENT> &srcDesc){
    Scene newScene;
    
    newScene.img = src.clone();
    newScene.path = imgPath;
    newScene.segments = srcDesc;
    
    scenes.push_back(newScene);
}

void DescManager::Read(){
    if(filepath.size() < 0)
        return;
    
    ifstream readFile(filepath, ios::in | ios::binary);
    
    if(!readFile.is_open())
        return;
    
    scenes.clear();
    
    int size = -1;
    readFile.read((char*)&size, sizeof(int));
    
    for(int i = 0; i < size; ++i){
        Scene readScene;
        int len = -1;
        readFile.read((char*)&len, sizeof(int));
        readScene.path.resize(len);
        readFile.read(&readScene.path[0], sizeof(char)*len);
        
        int segCount = -1;
        readFile.read((char*)&segCount, sizeof(int));
        
        for(int j = 0; j < segCount; ++j){
            SEGMENT readSeg;
            readSeg.msld = cv::Mat::zeros(72,1,CV_32FC1);
            
            readFile.read((char*)&readSeg.x1, sizeof(float));
            readFile.read((char*)&readSeg.y1, sizeof(float));
            readFile.read((char*)&readSeg.x2, sizeof(float));
            readFile.read((char*)&readSeg.y2, sizeof(float));
            readFile.read((char*)&readSeg.Label, sizeof(int));
            readFile.read((char*)&readSeg.LabelStable, sizeof(int));
            readFile.read((char*)&readSeg.TrackedCount, sizeof(int));
            readFile.read((char*)&readSeg.LastDetectedFrame, sizeof(int));
            readFile.read((char*)&readSeg.RecoveredCount, sizeof(int));
            readFile.read((char*)&readSeg.fAngle, sizeof(float));
            
            for(int k = 0; k < 72; ++k)
                readFile.read((char*)&readSeg.msld.at<float>(k,0), sizeof(float));
            
            readScene.segments.push_back(readSeg);
        }
        
        scenes.push_back(readScene);
    }
    
    readFile.close();
}

void DescManager::Write(){
    if(filepath.size() < 0)
        return;
    
    std::ofstream writeFile(filepath, ios::binary | ios::out);
    
    if(!writeFile.is_open())
        return;
    
    int size = (int)scenes.size();
    writeFile.write((char*)&size, sizeof(int));
    
    for(auto scene:scenes){
        int len = (int)scene.path.length();
        writeFile.write((char*)&len, sizeof(int));
        writeFile.write(&scene.path[0], sizeof(char)*len);
        
        int segCount = (int)scene.segments.size();
        writeFile.write((char*)&segCount, sizeof(int));
        
        for(auto segment:scene.segments){
            writeFile.write((char*)&segment.x1, sizeof(float));
            writeFile.write((char*)&segment.y1, sizeof(float));
            writeFile.write((char*)&segment.x2, sizeof(float));
            writeFile.write((char*)&segment.y2, sizeof(float));
            writeFile.write((char*)&segment.Label, sizeof(int));
            writeFile.write((char*)&segment.LabelStable, sizeof(int));
            writeFile.write((char*)&segment.TrackedCount, sizeof(int));
            writeFile.write((char*)&segment.LastDetectedFrame, sizeof(int));
            writeFile.write((char*)&segment.RecoveredCount, sizeof(int));
            writeFile.write((char*)&segment.fAngle, sizeof(float));
            
            for(int  i = 0; i < 72; ++i)
                writeFile.write((char*)&segment.msld.at<float>(i,0), sizeof(float));
        }
    }
    
    writeFile.close();
}

void DescManager::Delete(){
    if(filepath.size() < 0)
        return;
    
    scenes.clear();
}

int DescManager::Match(vector<SEGMENT> *src, string &path){
    int resultIdx = -1;
    cv::BFMatcher matcher(cv::NORM_L2, true);
    cv::Mat srcDescript;
    float minDist = std::numeric_limits<float>::max();
    
    for(auto seg:*src)
        srcDescript.push_back(seg.msld.reshape(1,1));
    
    vector<cv::DMatch> matches;
    
    for(int i = 0; i < scenes.size(); ++i){
        Scene *scene = &scenes.at(i);
        cv::Mat sceneDescript;
        
        for(auto seg:scene->segments)
            sceneDescript.push_back(seg.msld.reshape(1,1));
        
        vector<cv::DMatch> matches;
        matcher.match(srcDescript, sceneDescript, matches);
        
        //Decide
        float averDist = 0;
        for(auto match:matches){
            averDist += abs(match.distance);
        }
        
        averDist /= matches.size();
        
        if(minDist > averDist){
            minDist = averDist;
            resultIdx = i;
            path = scene->path;
        }
    }
        
    return resultIdx;
}
