/*
 * system.h
 *
 *  Created on: 2012. 6. 7.
 *      Author: spacetrain
 */

#ifndef SYSTEM_H_
#define SYSTEM_H_

#include <iostream>
#include <cstdio>
#include <stdio.h>
#include <string>

#include <opencv2/features2d/features2d.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/opencv.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/video/video.hpp>

using namespace std;

struct SEGMENT
{
	float x1, y1;
	float x2, y2;
	int Label;
	int LabelStable;
	int TrackedCount;
	int LastDetectedFrame;
	int RecoveredCount;
	float fAngle;
    cv::Mat msld;
};

struct Keyframe
{
	std::vector<SEGMENT> lines;
	cv::Mat image;
};

#endif /* SYSTEM_H_ */
