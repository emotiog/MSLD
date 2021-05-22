/*
 * msld.h
 *
 *  Created on: 2012. 6. 7.
 *      Author: spacetrain
 */

#ifndef MSLD_H_
#define MSLD_H_

#include "system.h"

using namespace std;
using namespace cv;

class MSLD
{
public:
	MSLD();

	void print_matrix( cv::Mat & mat );

	void BackwardWarping(cv::Mat & src, cv::Mat & dst, cv::Mat p_mat);
	void GetImagePatch(cv::Mat & src, cv::Mat & dst, SEGMENT seg);
	void GetMSLD(cv::Mat & src, vector<SEGMENT> * lines);
	void CalcMSLD(cv::Mat & patch, cv::Mat & msld);
};

#endif /* MSLD_H_ */
