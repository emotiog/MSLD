/*
 * LineDetector.h
 *
 *  Created on: 2012. 5. 23.
 *      Author: spacetrain
 */

#include "LineDetector.h"

using namespace cv;

LineDetector::LineDetector()
{
	init();
}

LineDetector::LineDetector(Mat & src)
{
	imageheight = src.rows;
	imagewidth = src.cols;
	init();
}

void LineDetector::init()
{
    threshold_dist = 1.5f;
    threshold_length = THRESHOLD_LENGTH;
    label = 0;
}

void LineDetector::set(Mat & src)
{
	imageheight = src.rows;
	imagewidth = src.cols;
	init();
}

void LineDetector::set(Size sz)
{
	imageheight = sz.height;
	imagewidth = sz.width;
	init();
}


void LineDetector::MergeLines(SEGMENT * Seg1, SEGMENT * Seg2, SEGMENT * SegMerged)
{
	double dXg = 0.0, dYg = 0.0;
	double dDelta1X = 0.0, dDelta1Y = 0.0, dDelta2X = 0.0, dDelta2Y = 0.0;
	float fAx = 0, fBx = 0, fCx = 0, fDx = 0;
	float fAy = 0, fBy = 0, fCy = 0, fDy = 0;
	double dLi = 0.0, dLj = 0.0;
	double dThi = 0.0, dThj = 0.0, dThr = 0.0;
	double dAxg = 0.0, dAyg = 0.0, dBxg = 0.0, dByg = 0.0, dCxg = 0.0, dCyg =
			0.0, dDxg = 0.0, dDyg = 0.0, dDelta1Xg = 0.0, dDelta2Xg = 0.0;

	fAx = Seg1->x1;
	fAy = Seg1->y1;

	fBx = Seg1->x2;
	fBy = Seg1->y2;

	fCx = Seg2->x1;
	fCy = Seg2->y1;

	fDx = Seg2->x2;
	fDy = Seg2->y2;

	float fDLix = (fBx - fAx);
	float fDLiy = (fBy - fAy);
	float fDLjx = (fDx - fCx);
	float fDLjy = (fDy - fCy);

	dLi = sqrt((double) (fDLix * fDLix) + (double) (fDLiy * fDLiy));
	dLj = sqrt((double) (fDLjx * fDLjx) + (double) (fDLjy * fDLjy));

	dXg = (dLi * (double) (fAx + fBx) + dLj * (double) (fCx + fDx))
			/ (double) (2.0 * (dLi + dLj));
	dYg = (dLi * (double) (fAy + fBy) + dLj * (double) (fCy + fDy))
			/ (double) (2.0 * (dLi + dLj));

	if(fDLix == 0.0f) dThi = CV_PI / 2.0;
	else dThi = atan(fDLiy / fDLix);

	if(fDLjx == 0.0f) dThj = CV_PI / 2.0;
	else dThj = atan(fDLjy / fDLjx);

	if (fabs(dThi - dThj) <= CV_PI / 2.0)
	{
		dThr = (dLi * dThi + dLj * dThj) / (dLi + dLj);
	}
	else
	{
		double dTmp = dThj - CV_PI * (dThj / fabs(dThj));
		dThr = dLi * dThi + dLj * dTmp;
		dThr /= (dLi + dLj);
	}

	dAxg = ((double) fAy - dYg) * sin(dThr) + ((double) fAx - dXg) * cos(dThr);
	dAyg = ((double) fAy - dYg) * cos(dThr) - ((double) fAx - dXg) * sin(dThr);

	dBxg = ((double) fBy - dYg) * sin(dThr) + ((double) fBx - dXg) * cos(dThr);
	dByg = ((double) fBy - dYg) * cos(dThr) - ((double) fBx - dXg) * sin(dThr);

	dCxg = ((double) fCy - dYg) * sin(dThr) + ((double) fCx - dXg) * cos(dThr);
	dCyg = ((double) fCy - dYg) * cos(dThr) - ((double) fCx - dXg) * sin(dThr);

	dDxg = ((double) fDy - dYg) * sin(dThr) + ((double) fDx - dXg) * cos(dThr);
	dDyg = ((double) fDy - dYg) * cos(dThr) - ((double) fDx - dXg) * sin(dThr);

	std::vector<double> vDeltaXG;
	vDeltaXG.push_back(dAxg);
	vDeltaXG.push_back(dBxg);
	vDeltaXG.push_back(dCxg);
	vDeltaXG.push_back(dDxg);

	std::sort(vDeltaXG.begin(), vDeltaXG.end());

	dDelta1Xg = vDeltaXG[0];
	dDelta2Xg = vDeltaXG[3];

	dDelta1X = dDelta1Xg * cos(dThr) + dXg;
	dDelta1Y = dDelta1Xg * sin(dThr) + dYg;
	dDelta2X = dDelta2Xg * cos(dThr) + dXg;
	dDelta2Y = dDelta2Xg * sin(dThr) + dYg;

	SegMerged->x1 = (float) dDelta1X;
	SegMerged->y1 = (float) dDelta1Y;
	SegMerged->x2 = (float) dDelta2X;
	SegMerged->y2 = (float) dDelta2Y;
}

double LineDetector::dist_point_line( const Mat & p, Mat & l )
{
	double x, y, w;

	x = l.at<double>(0,0);
	y = l.at<double>(1,0);

	w = sqrt(x*x+y*y);

	l.at<double>(0,0) = x  / w;
	l.at<double>(1,0) = y  / w;
	l.at<double>(2,0) = l.at<double>(2,0)  / w;

	return l.dot(p);
}

bool LineDetector::merge_segments( SEGMENT * seg1, SEGMENT * seg2, SEGMENT * seg_merged )
{
	double o[] = { 0.0, 0.0, 1.0 };
	double a[] = { 0.0, 0.0, 1.0 };
	double b[] = { 0.0, 0.0, 1.0 };
	double c[3];

	o[0] = ( seg2->x1 + seg2->x2 ) / 2.0;
	o[1] = ( seg2->y1 + seg2->y2 ) / 2.0;

	a[0] = seg1->x1;
	a[1] = seg1->y1;
	b[0] = seg1->x2;
	b[1] = seg1->y2;

	Mat ori = Mat(3, 1, CV_64FC1, o).clone();
	Mat p1 = Mat(3, 1, CV_64FC1, a).clone();
	Mat p2 = Mat(3, 1, CV_64FC1, b).clone();
	Mat l1 = Mat(3, 1, CV_64FC1, c).clone();

	l1 = p1.cross(p2);

	Point2f dSeg1Middle, dSeg2Middle;
	dSeg1Middle.x = (seg1->x1 + seg1->x2) /2.0f;
	dSeg1Middle.y = (seg1->y1 + seg1->y2) /2.0f;
	dSeg2Middle.x = (seg2->x1 + seg2->x2) /2.0f;
	dSeg2Middle.y = (seg2->y1 + seg2->y2) /2.0f;

	double dSeg1Length, dSeg2Length;
	dSeg1Length = sqrt((seg1->x1 - seg1->x2)*(seg1->x1 - seg1->x2)+(seg1->y1 - seg1->y2)*(seg1->y1 - seg1->y2));
	dSeg2Length = sqrt((seg2->x1 - seg2->x2)*(seg2->x1 - seg2->x2)+(seg2->y1 - seg2->y2)*(seg2->y1 - seg2->y2));

	double dMiddleDist = sqrt((dSeg1Middle.x - dSeg2Middle.x)*(dSeg1Middle.x - dSeg2Middle.x)
			+ (dSeg1Middle.y - dSeg2Middle.y)*(dSeg1Middle.y - dSeg2Middle.y));

	float fAngleDiff = seg1->fAngle - seg2->fAngle;
	fAngleDiff = fabs(fAngleDiff);
//	if(fAngleDiff > CV_PI / 2.0) fAngleDiff = CV_PI - fAngleDiff;

	double dist = dist_point_line( ori, l1 );

	if ( fabs( dist ) <= threshold_dist * 2.0
			&& dMiddleDist <= dSeg1Length / 2.0 + dSeg2Length / 2.0 + 20.0
			&& fAngleDiff <= CV_PI / 180.0f * 5.0f)
	{
		MergeLines(seg1, seg2, seg2);
		return true;
	}
	else
	{
		return false;
	}
}

template<class tType>
void LineDetector::incident_point( tType * pt, Mat & l )
{
	double a[] = { (double)pt->x, (double)pt->y, 1.0 };
	double b[] = { l.at<double>(0,0), l.at<double>(1,0), 0.0 };
	double c[3];

	Mat xk = Mat(3, 1, CV_64FC1, a).clone();
	Mat lh = Mat(3, 1, CV_64FC1, b).clone();
	Mat lk = Mat(3, 1, CV_64FC1, c).clone();

	lk = xk.cross(lh);
	xk = lk.cross(l);

	double s = 1.0 / xk.at<double>(2,0);
	xk.convertTo(xk, -1, s);

	pt->x = (float)xk.at<double>(0,0) < 0.0f ? 0.0f : (float)xk.at<double>(0,0) >= (imagewidth - 1.0f) ? (imagewidth - 1.0f) : (float)xk.at<double>(0,0);
	pt->y = (float)xk.at<double>(1,0) < 0.0f ? 0.0f : (float)xk.at<double>(1,0) >= (imageheight - 1.0f) ? (imageheight - 1.0f) : (float)xk.at<double>(1,0);

}

void LineDetector::extract_segments( vector<Point2i> * points, vector<SEGMENT> * segments )
{
	bool is_line;

	int i, j;
	SEGMENT seg;
	Point2i ps, pe, pt;

	vector<Point2i> l_points;

	int total = (int)points->size();

	for ( i = 0; i + threshold_length < total; ++i )
	{
		ps = points->at(i);
		pe = points->at(i + threshold_length);

		double a[] = { (double)ps.x, (double)ps.y, 1 };
		double b[] = { (double)pe.x, (double)pe.y, 1 };
		double c[3], d[3];

		Mat p1 = Mat(3, 1, CV_64FC1, a).clone();
		Mat p2 = Mat(3, 1, CV_64FC1, b).clone();
		Mat p = Mat(3, 1, CV_64FC1, c).clone();
		Mat l = Mat(3, 1, CV_64FC1, d).clone();
		l = p1.cross(p2);

		is_line = true;

		l_points.clear();
		l_points.push_back(ps);

		for ( j = 1; j < threshold_length; ++j )
		{
			pt.x = points->at(i+j).x;
			pt.y = points->at(i+j).y;

			p.at<double>(0,0) = (double)pt.x;
			p.at<double>(1,0) = (double)pt.y;
			p.at<double>(2,0) = 1.0;

			double dist = dist_point_line( p, l ); // ??

			if ( fabs( dist ) > threshold_dist )
			{
				is_line = false;
				break;
			}
			l_points.push_back(pt);
		}

		// Line check fail, test next point
		if ( is_line == false )
		{
			continue;
		}
		l_points.push_back(pe);

		Vec4f line;
		fitLine( Mat(l_points), line, DIST_L2, 0, 0.01, 0.01);
		a[0] = line[2];
		a[1] = line[3];
		b[0] = line[2] + line[0];
		b[1] = line[3] + line[1];

		p1 = Mat(3, 1, CV_64FC1, a).clone();
		p2 = Mat(3, 1, CV_64FC1, b).clone();

		l = p1.cross(p2);

		incident_point( &ps, l );

		// Extending line
		for ( j = threshold_length + 1; i + j < total; ++j )
		{
			pt.x = points->at(i+j).x;
			pt.y = points->at(i+j).y;

			p.at<double>(0,0) = (double)pt.x;
			p.at<double>(1,0) = (double)pt.y;
			p.at<double>(2,0) = 1.0;

			double dist = dist_point_line( p, l );

			if ( fabs( dist ) > threshold_dist )
			{
				--j;
				break;
			}

			pe = pt;
			l_points.push_back(pt);
		}
		fitLine( Mat(l_points), line, DIST_L2, 0, 0.01, 0.01);
		a[0] = line[2];
		a[1] = line[3];
		b[0] = line[2] + line[0];
		b[1] = line[3] + line[1];

		p1 = Mat(3, 1, CV_64FC1, a).clone();
		p2 = Mat(3, 1, CV_64FC1, b).clone();

		l = p1.cross(p2);

		Point2f e1, e2;
		e1.x = ps.x;
		e1.y = ps.y;
		e2.x = pe.x;
		e2.y = pe.y;

		incident_point( &e1, l );
		incident_point( &e2, l );
		seg.x1 = e1.x;
		seg.y1 = e1.y;
		seg.x2 = e2.x;
		seg.y2 = e2.y;

 		segments->push_back(seg);
		i = i + j;
	}
}

void LineDetector::point_inboard_test(Mat & src, Point2i * pt)
{
	pt->x = pt->x <= 5.0f ? 5.0f : pt->x >= src.cols - 5.0f ? src.cols - 5.0f : pt->x;
	pt->y = pt->y <= 5.0f ? 5.0f : pt->y >= src.rows - 5.0f ? src.rows - 5.0f : pt->y;
}

bool LineDetector::get_point_chain( const Mat & img, const Point pt, Point * chained_pt, int & direction, int step )
{
	int ri, ci;

	int indices[8][2] = { {1, 0}, {1, 1}, {0, 1}, {-1, 1},
			{-1, 0}, {-1, -1}, {0, -1}, {1, -1} }; // directions each corresponds to 1,2,...,8

	for ( int i = 0; i < 8; i++ )
	{
		ci = pt.x + indices[i][1];
		ri = pt.y + indices[i][0];

		if ( ri < 0 || ri == img.rows || ci < 0 || ci == img.cols )
			continue;

		if ( img.at<unsigned char>(ri, ci) == 0 )
			continue;

		if(step == 0)
		{
			chained_pt->x = ci;
			chained_pt->y = ri;
			direction = i;
			return true;
		}
		else
		{
			if(abs(i-direction) <= 2 || abs(i-direction) >= 6)
			{
				chained_pt->x = ci;
				chained_pt->y = ri;
				direction = i;
				return true;
			}
			else
				continue;
		}
	}
	return false;
}

void LineDetector::line_detection( Mat & src, vector<SEGMENT> * segments_all )
{
	int r, c;

	vector<Point2i> points;
	vector<SEGMENT> segments;

	vector<SEGMENT> segments_tmp;
	vector<SEGMENT> segments_tmp2;

	Mat canny = src.clone();
	Canny(src, canny, 30, 30, 3);
    
    // Remove Boundary of Image
	for(int i=0; i<src.rows;i++)
	{
		for(int j=0; j<src.cols;j++)
		{
			if( i < 5 || i > src.rows-5 || j < 5 || j > src.cols - 5)
				canny.at<unsigned char>(i,j) = 0;
		}
	}

	SEGMENT seg1, seg2;

	for ( r = 0; r < imageheight; r++ )
	{
		for ( c = 0; c < imagewidth; c++ )
		{
			// Find seeds - skip for non-seeds
			if ( canny.at<unsigned char>(r,c) == 0 )
				continue;

			// Found seeds
			Point2i pt;
			pt.x = c;
			pt.y = r;

			points.push_back(pt);
			canny.at<unsigned char>(pt.y, pt.x) = 0;

			int direction = 0;
			int step = 0;
			while (get_point_chain( canny, pt, &pt, direction, step))
			{
				points.push_back(pt);
				++step;
				canny.at<unsigned char>(pt.y, pt.x) = 0;
			}

			if ( points.size() < (unsigned int)threshold_length + 1 )
			{
				points.clear();
				continue;
			}

			extract_segments( &points, &segments );

			if ( segments.size() == 0 )
			{
				points.clear();
				continue;
			}
            
            for(auto seg : segments){
                float fLength = sqrt((seg.x1 - seg.x2)*(seg.x1 - seg.x2) + (seg.y1 - seg.y2)*(seg.y1 - seg.y2));
                if(fLength < threshold_length) continue;
                
                // Pass - Image boundary segment
                if( (seg.x1 <= 5.0f && seg.x2 <= 5.0f)
                    || (seg.y1 <= 5.0f && seg.y2 <= 5.0f)
                    || (seg.x1 >= imagewidth - 5.0f && seg.x2 >= imagewidth - 5.0f)
                    || (seg.y1 >= imageheight - 5.0f && seg.y2 >= imageheight - 5.0f) ) continue;

                AdditionalOperationsOnSegments(src, &seg);
                segments_tmp.push_back(seg);
            }
            
			points.clear();
			segments.clear();
		}
	}

	bool is_merged = false;

	while(segments_tmp.size() > 0)
	{
		if(segments_tmp.size() == 1)
		{
			seg1 = segments_tmp.back();
			segments_tmp.pop_back();
			segments_tmp2.push_back(seg1);
			break;
		}
		else
		{
			seg1 = segments_tmp.back();
			segments_tmp.pop_back();
			seg2 = segments_tmp.back();
			segments_tmp.pop_back();

			is_merged = merge_segments(&seg1, &seg2, &seg2);
			if(is_merged == true)
			{
				AdditionalOperationsOnSegments(src, &seg2);
				segments_tmp.push_back(seg2);
			}
			else
			{
				segments_tmp.push_back(seg2);
				segments_tmp2.push_back(seg1);
			}
		}
	}

    // Final Merge
	bool *IsAdded = new bool[segments_tmp2.size()];
	memset(IsAdded, false, segments_tmp2.size());

	for(int i = 0; i < (int)segments_tmp2.size(); ++i)
	{
		seg1 = segments_tmp2.at(i);
		if(IsAdded[i] == true) continue;

		is_merged = false;
        
		for(int j = 0; j < (int)segments_tmp2.size(); ++j)
		{
			if(i == j || IsAdded[j] == true) continue;

			seg2 = segments_tmp2.at(j);

			is_merged = merge_segments(&seg1, &seg2, &seg2);

			if(is_merged == true)
			{
				AdditionalOperationsOnSegments(src, &seg2);
				segments_all->push_back(seg2);
				IsAdded[j] = true;
				IsAdded[i] = true;
			}
		}
		if(IsAdded[i] != true)
		{
			segments_all->push_back(seg1);
			IsAdded[i] = true;
		}
	}
    
	delete[] IsAdded;
}

void LineDetector::AdditionalOperationsOnSegments(Mat & src, SEGMENT * seg)
{
	if(seg->x1 == 0.0f && seg->x2 == 0.0f && seg->y1 == 0.0f && seg->y2 == 0.0f) return;

	float fDx, fDy, fTemp;
	double dAngle;

	fDx = (float)(seg->x2 - seg->x1);
	fDy = (float)(seg->y2 - seg->y1);

	fTemp = 0.0f;

	if(fDx == 0.0f)
	{
		if(fDy > 0)
			dAngle = CV_PI / 2.0;
		else
			dAngle = -1.0 * CV_PI / 2.0;
	}
	else if(fDy == 0.0f)
	{
		if(fDx > 0)
			dAngle = 0.0;
		else
			dAngle = CV_PI;
	}
	else if(fDx < 0.0f && fDy > 0.0f)
		dAngle = CV_PI + atan( fDy/fDx );
	else if(fDx > 0.0f && fDy < 0.0f)
		dAngle = 2*CV_PI + atan( fDy/fDx );
	else if(fDx < 0.0f && fDy < 0.0f)
		dAngle = CV_PI + atan( fDy/fDx );
	else
		dAngle = atan( fDy/fDx );

	if(dAngle > 2.0 * CV_PI)
		dAngle -= 2.0 * CV_PI;

	seg->fAngle = (float)dAngle;

	Point2f pStart = Point2f(seg->x1, seg->y1);
	Point2f pEnd = Point2f(seg->x2, seg->y2);

	double dDx = 0.0, dDy = 0.0;
	dDx = (double) pEnd.x - (double) pStart.x;
	dDy = (double) pEnd.y - (double) pStart.y;

	int iNCP = 10;
	Point2f *pCP = new Point2f[iNCP];

	pCP[0] = pStart;
	pCP[iNCP - 1] = pEnd;
	for (int i = 0; i < iNCP; i++)
	{
		if (i == 0 || i == iNCP - 1)
		{
			continue;
		}
		pCP[i].x = pCP[0].x + (dDx / double(iNCP - 1) * (double) i);
		pCP[i].y = pCP[0].y + (dDy / double(iNCP - 1) * (double) i);
	}

	Point2i *pCPR = new Point2i[iNCP];
	Point2i *pCPL = new Point2i[iNCP];

	double dGap = 1.0;

	for(int i = 0; i < iNCP; i++)
	{
		pCPR[i].x = cvRound(pCP[i].x + dGap*cos(90.0 * CV_PI / 180.0 + dAngle));
		pCPR[i].y = cvRound(pCP[i].y + dGap*sin(90.0 * CV_PI / 180.0 + dAngle));
		pCPL[i].x = cvRound(pCP[i].x - dGap*cos(90.0 * CV_PI / 180.0 + dAngle));
		pCPL[i].y = cvRound(pCP[i].y - dGap*sin(90.0 * CV_PI / 180.0 + dAngle));
		point_inboard_test(src, &pCPR[i]);
		point_inboard_test(src, &pCPL[i]);
	}

	int iR = 0, iL = 0;
	for(int i = 0; i < iNCP; i++)
	{
		iR += src.at<unsigned char>(pCPR[i].y, pCPR[i].x);
		iL += src.at<unsigned char>(pCPL[i].y, pCPL[i].x);
	}

    // Change Start end position (set opposite direction)
	if(iR > iL)
	{
		fTemp = seg->x1; seg->x1 = seg->x2; seg->x2 = fTemp;
		fTemp = seg->y1; seg->y1 = seg->y2; seg->y2 = fTemp;

		fDx = (float)(seg->x2 - seg->x1);
		fDy = (float)(seg->y2 - seg->y1);

		dAngle = dAngle + CV_PI;
		if(dAngle >= 2.0*CV_PI)
			dAngle = dAngle - 2.0 * CV_PI;
		seg->fAngle = (float)dAngle;
	}

	delete[] pCP;
	delete[] pCPR;
	delete[] pCPL;

	seg->RecoveredCount = 0;
	seg->TrackedCount = 0;
	seg->LabelStable = -1;
	seg->Label = label++;

	return;
}

void LineDetector::draw_arrow( Mat & mat, const SEGMENT * seg, bool bRandColor)
{
	Point2i p1;
    Scalar bgr;
    
	double dGap = 10.0;
	double dAngle = (double)seg->fAngle;
	double dArrowAng = 30.0;

    if(bRandColor){
        int idx = seg->LabelStable < 0 ? seg->Label : seg->LabelStable;
        bgr = Scalar(29 * idx % 255, 37 * idx % 255, 79 * idx % 255, 255);
    }
    else
        bgr = Scalar(255, 0, 0, 255);
    
	p1.x = cvRound(seg->x2 - dGap*cos(dArrowAng * CV_PI / 180.0 + dAngle));
	p1.y = cvRound(seg->y2 - dGap*sin(dArrowAng * CV_PI / 180.0 + dAngle));
	point_inboard_test(mat, &p1);

	line(mat, Point(cvRound(seg->x1), cvRound(seg->y1)),
			Point(cvRound(seg->x2), cvRound(seg->y2)), bgr, 1, 1);
	line(mat, Point(cvRound(seg->x2), cvRound(seg->y2)),
			p1, bgr, 1, 1);
}
