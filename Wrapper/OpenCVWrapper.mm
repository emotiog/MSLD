//
//  OpenCVWrapper.m
//  MSLD
//
//  Created by junsang on 2021/01/09.
//

#import "OpenCVWrapper.h"
#import <opencv2/opencv2.h>
#import <opencv2/imgcodecs/ios.h>

@implementation OpenCVWrapper

+(UIImage *) makeGrayImage:(UIImage *)image {
    // Transform UIImage to cv::mat
    cv::Mat imageMat;
    
    UIImageToMat(image, imageMat);
    
    // If the image was already grayscale, return it
    if (imageMat.channels() == 1) {
        return image;
    }
    
    // Transform the cv::mat color image to gray
    cv::Mat grayMat;
    cv::cvtColor(imageMat, grayMat, COLOR_BGR2GRAY);
    
    // Transform grayMat to UIImage and return
    return MatToUIImage(grayMat);
}

@end
