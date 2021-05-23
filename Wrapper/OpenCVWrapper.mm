//
//  OpenCVWrapper.m
//  MSLD
//
//  Created by junsang on 2021/01/09.
//

#import <opencv2/opencv.hpp>
#import <opencv2/imgcodecs/ios.h>
#import "OpenCVWrapper.h"

#import "system.h"
#import "LineDetector.h"
#import "msld.h"

#import "DescManager.h"

@implementation OpenCVWrapper

+(UIImage *) convertToGray:(UIImage *)image {
    // Transform UIImage to cv::mat
    cv::Mat imageMat;
    
    UIImageToMat(image, imageMat);
    
    // If the image was already grayscale, return it
    if (imageMat.channels() == 1) {
        return image;
    }
    
    // Transform the cv::mat color image to gray
    cv::Mat grayMat;
    cv::cvtColor(imageMat, grayMat, cv::COLOR_BGR2GRAY);
    
    // Transform grayMat to UIImage and return
    return MatToUIImage(grayMat);
}

+(UIImage *) convertToCannyEdge:(UIImage *)image {
    cv::Mat imageMat;
    UIImageToMat(image, imageMat);
    
    cv::Mat canny = imageMat.clone();
    cv::Canny(imageMat, canny, 30, 30, 3);
    
    return MatToUIImage(canny);
}

+(UIImage*) detectLine:(UIImage*) inputImage isRandomColor:(bool) isRandom hasDescriptor:(bool) hasDescriptor {
    cv::Mat orgImg, gray;
    UIImageToMat(inputImage, orgImg);
    cv::cvtColor(orgImg, gray, cv::COLOR_BGR2GRAY);
    cv::Mat resultImage = orgImg.clone();
    
    LineDetector lineDetector(gray);
    MSLD descriptor;
    vector<SEGMENT> allSegments;
    lineDetector.line_detection(gray, &allSegments);
    
    if(hasDescriptor) {
        descriptor.GetMSLD(gray, &allSegments);
    }
    
    for(auto segment:allSegments) {
        lineDetector.draw_arrow(resultImage, &segment, isRandom);
    }
    
    return MatToUIImage(resultImage);
}

+(void) initDescManager:(NSString*) path {
    DescManager::instance().setPath([path UTF8String]);
//    if(DescManager::instance().count() == 0 ) {
//        DescManager::instance().Read();
//    }
}

+(void) appendDesc:(UIImage*) image savePath:(nonnull NSString *)path {
    cv::Mat origin, gray;
    
    UIImageToMat(image, origin);
    const std::string cppString = [path UTF8String];
    
    cv::cvtColor(origin, gray, cv::COLOR_BGR2GRAY);
    
    LineDetector lineDetect(gray);
    MSLD descriptor;
    std::vector<SEGMENT> segments;
    lineDetect.line_detection(gray, &segments);
    
    descriptor.GetMSLD(gray, &segments);
    
    DescManager::instance().append(origin, cppString, segments);
}
@end
