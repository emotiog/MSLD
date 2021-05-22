/*
 * LineDetector.h
 *
 *  Created on: 2012. 5. 23.
 *      Author: spacetrain
 */

#ifndef LINEDETECTOR_H_
#define LINEDETECTOR_H_

#define THRESHOLD_LENGTH 30 // default: 30

#include "system.h"

class LineDetector
{
public:
	LineDetector();
	LineDetector(cv::Mat & src);
//	~LineDetector();

    void init();
    
	int label;

	int imagewidth;
	int imageheight;

	int threshold_length;
	float threshold_dist;

	void set(cv::Mat & src);
	void set(cv::Size sz);

	void MergeLines(SEGMENT * Seg1, SEGMENT * Seg2, SEGMENT * SegMerged);

	bool get_point_chain( const cv::Mat & img, const cv::Point pt, cv::Point * chained_pt, int & direction, int step );

	double dist_point_line( const cv::Mat & p, cv::Mat & l );

	bool merge_segments( SEGMENT * seg1, SEGMENT * seg2, SEGMENT * seg_merged );

	template<class tType>
	void incident_point( tType * pt, cv::Mat & l );

	void extract_segments( vector<cv::Point2i> * points, vector<SEGMENT> * segments );

	void line_detection( cv::Mat & src, vector<SEGMENT> * segments_all );

	void point_inboard_test(cv::Mat & src, cv::Point2i * pt);

	void AdditionalOperationsOnSegments(cv::Mat & src, SEGMENT * seg);

	void draw_arrow( cv::Mat & mat, const SEGMENT * seg, bool bRandColor);
};

#endif
