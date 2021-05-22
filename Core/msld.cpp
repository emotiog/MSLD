/*
 * msld.cpp
 *
 *  Created on: 2012. 6. 7.
 *      Author: spacetrain
 *  Modified on: 2020. 3. 30 - sp9103
 */

#include "msld.h"

int Gx(Mat & patch, int x, int y)
{
	return (patch.at<unsigned char>(y+1,x-1) + 2*patch.at<unsigned char>(y+1,x) + patch.at<unsigned char>(y+1,x+1))
			    		- (patch.at<unsigned char>(y-1,x-1) + 2*patch.at<unsigned char>(y-1,x) + patch.at<unsigned char>(y-1,x+1));
}

int Gy(Mat & patch, int x, int y)
{
	return (patch.at<unsigned char>(y-1,x+1) + 2*patch.at<unsigned char>(y,x+1) + patch.at<unsigned char>(y+1,x+1))
			    		- (patch.at<unsigned char>(y-1,x-1) + 2*patch.at<unsigned char>(y,x-1) + patch.at<unsigned char>(y+1,x-1));
}

static int bound_check(int x, int y, int w, int h){
	if(x<0 || x>= w || y<0 || y>=h)
		return 0;
	return 1;
}

MSLD::MSLD()
{
}

void MSLD::print_matrix( Mat & mat )
{
	if(mat.channels() == 1)
	{
		for(int i=0; i<mat.rows; i++)
		{
			cout << i << ": ";
			for(int j=0; j<mat.cols; j++)
			{
				cout << "[" << j << "] ";
				cout << mat.at<double>(i,j) << "\t";
			}
			cout << endl;
		}
	}
	else
	{
		for(int i=0; i<mat.rows; i++)
		{
			cout << i << ": ";
			for(int j=0; j<mat.cols; j++)
			{
				cout << "[" << j << "] ";
				for(int k=0; k<mat.channels(); k++)
				{
					cout << "(" << k << ") "<< mat.at<Vec4d>(i,j)[k] << "\t";
				}
			}
			cout << endl;
		}
	}
}

void MSLD::BackwardWarping(Mat & src, Mat & dst, Mat p_mat)
{
	double w, pixel, ratio, px, py;

	double wx[2];
	double wy[2];

	int i, j, x, y;

	Mat rot_inv = p_mat.inv();

	for( j = 0 ; j < dst.rows ; j++ )
	{
		for( i = 0 ; i < dst.cols ; i++ )
		{
			ratio = pixel = 0.0;
			w = rot_inv.at<double>(2,0)*i + rot_inv.at<double>(2,1)*j + rot_inv.at<double>(2,2);

			px = rot_inv.at<double>(0,0)*i + rot_inv.at<double>(0,1)*j + rot_inv.at<double>(0,2);
			py = rot_inv.at<double>(1,0)*i + rot_inv.at<double>(1,1)*j + rot_inv.at<double>(1,2);

			wx[1] = px - floor(px);
			wx[0] = 1.0 - wx[1];

			wy[1] = py - floor(py);
			wy[0] = 1.0 - wy[1];

			x = floor(px);
			y = floor(py);

			if( bound_check(x, y, src.cols, src.rows) )
			{
				pixel += wx[0]*wy[0]*src.at<unsigned char>(y,x);
				ratio += wx[0]*wy[0];
			}
			if( bound_check(x+1, y, src.cols, src.rows) )
			{
				pixel += wx[1]*wy[0]*src.at<unsigned char>(y,x+1);
				ratio += wx[1]*wy[0];
			}
			if( bound_check(x, y+1, src.cols, src.rows) )
			{
				pixel += wx[0]*wy[1]*src.at<unsigned char>(y+1,x);
				ratio += wx[0]*wy[1];
			}
			if( bound_check(x+1, y+1, src.cols, src.rows) )
			{
				pixel += wx[1]*wy[1]*src.at<unsigned char>(y+1,x+1);
				ratio += wx[1]*wy[1];
			}
			dst.at<unsigned char>(j,i) = (unsigned char)floor( pixel/ratio + 0.5 );
		}
	}
}

void MSLD::GetImagePatch(Mat & src, Mat & dst, SEGMENT seg)
{
	Mat rot = Mat::zeros(3,3, CV_64FC1);

	double offset_x = 0.0;
	double offset_y = 0.0;

	Mat l = Mat::zeros(3,1,CV_64FC1);
	l.at<double>(0,0) = (double)(seg.x2 - seg.x1);
	l.at<double>(1,0) = (double)(seg.y2 - seg.y1);
	l.at<double>(2,0) = 1.0;

    // length
	double s = sqrt(l.at<double>(0,0)*l.at<double>(0,0)+l.at<double>(1,0)*l.at<double>(1,0));

    // line normalize
	Mat ln = Mat::zeros(3,1,CV_64FC1);
	ln.at<double>(0,0) = l.at<double>(0,0) / s;
	ln.at<double>(1,0) = l.at<double>(1,0) / s;
	ln.at<double>(2,0) = 1.0;

    // line normal - orthogonal vector
	Mat n = Mat::zeros(3,1,CV_64FC1);
	n.at<double>(0,0) = (double)(cos(90.0*CV_PI/180.0)*l.at<double>(0,0) - sin(90.0*CV_PI/180.0)*l.at<double>(1,0));
	n.at<double>(1,0) = (double)(sin(90.0*CV_PI/180.0)*l.at<double>(0,0) + cos(90.0*CV_PI/180.0)*l.at<double>(1,0));
	n.at<double>(2,0) = 1.0;

	s = sqrt(n.at<double>(0,0)*n.at<double>(0,0)+n.at<double>(1,0)*n.at<double>(1,0));

	n.at<double>(0,0) = n.at<double>(0,0) / s;
	n.at<double>(1,0) = n.at<double>(1,0) / s;

	Mat e1 = Mat::zeros(3,1,CV_64FC1);
	Mat e2 = Mat::zeros(3,1,CV_64FC1);
	e1.at<double>(0,0) = seg.x1;
	e1.at<double>(1,0) = seg.y1;
	e1.at<double>(2,0) = 0.0;
	e2.at<double>(0,0) = seg.x2;
	e2.at<double>(1,0) = seg.y2;
	e2.at<double>(2,0) = 0.0;

	rot.at<double>(0,0) = cos(-seg.fAngle);
	rot.at<double>(0,1) = -sin(-seg.fAngle);
	rot.at<double>(1,0) = sin(-seg.fAngle);
	rot.at<double>(1,1) = cos(-seg.fAngle);
	rot.at<double>(2,2) = 1.0;
	rot.at<double>(0,2) = 0.0;
	rot.at<double>(1,2) = 0.0;

	Mat v1 = 23.0*n + e1 - 3.0*ln;
	v1 = rot * v1;
	Mat v2 = -23.0*n + e1 - 3.0*ln;
	v2 = rot * v2;
	Mat v3 = 23.0*n + e2 + 3.0*ln;
	v3 = rot * v3;
	Mat v4 = -23.0*n + e2 + 3.0*ln;
	v4 = rot * v4;

	offset_x = v1.at<double>(0,0) < v2.at<double>(0,0) ? v1.at<double>(0,0) : v2.at<double>(0,0);
	offset_x = offset_x < v3.at<double>(0,0) ? offset_x : v3.at<double>(0,0);
	offset_x = offset_x < v4.at<double>(0,0) ? offset_x : v4.at<double>(0,0);

	offset_y = v1.at<double>(1,0) < v2.at<double>(1,0) ? v1.at<double>(1,0) : v2.at<double>(1,0);
	offset_y = offset_y < v3.at<double>(1,0) ? offset_y : v3.at<double>(1,0);
	offset_y = offset_y < v4.at<double>(1,0) ? offset_y : v4.at<double>(1,0);

    // Rotate & Translate to (0,0)
	rot.at<double>(0,0) = cos(-seg.fAngle);
	rot.at<double>(0,1) = -sin(-seg.fAngle);
	rot.at<double>(1,0) = sin(-seg.fAngle);
	rot.at<double>(1,1) = cos(-seg.fAngle);
	rot.at<double>(2,2) = 1.0;
	rot.at<double>(0,2) = -offset_x;
	rot.at<double>(1,2) = -offset_y;

	BackwardWarping(src, dst, rot);
}

void MSLD::CalcMSLD(Mat & patch, Mat & msld)
{
	Mat GDM = Mat::zeros(9, patch.cols-6, CV_64FC4); // channel per gradient direction
	Mat G = Mat::zeros(5*9, patch.cols-2, CV_64FC2); // channel x-y

	for(int i=0; i<patch.rows; i++)
	{
		if(i==0 || i >= patch.rows-1)
			continue;

		for(int j=0; j<patch.cols; j++)
		{
			if(j==0 || j >= patch.cols-1)
				continue;
            
            //calculate partial gradient
			double gx = (double)Gx(patch, j, i);
			double gy = (double)Gy(patch, j, i);
			int d = abs(i-23);
			float sigma = 22.5f;
			double w = 1 / (sqrt(2.0*CV_PI)*(double)sigma) * exp(-1.0*(double)(d*d) / (2.0*(double)(sigma*sigma)));
            
			gx = w*gx;
			gy = w*gy;

			G.at<Vec2d>(i-1,j-1)[0] = gx;
			G.at<Vec2d>(i-1,j-1)[1] = gy;
		}
	}

	for(int i=0; i<GDM.rows; i++)
	{
		for(int j=0; j<GDM.cols; j++)
		{
			double gx=0.0, gy=0.0;
			double w1=0.0, w2=0.0;
			int d1=0, d2=0;

			for(int k=0; k<5; k++)
			{
                //w2 for neighbor
				d1 = abs(2-k); // 2 1 0 1 2
//				d2 = (3+k)%5;  // 3 4 0 1 2 //???
                d2 = 5 - d1;
				w1 = (double)d2/(double)(d1+d2);    // 3/5 4/5 inf 1/2 1/2
				w2 = (double)d1/(double)(d1+d2);    // 2/5 1/5 inf 1/2 1/2

				for(int l=0; l<5; l++)
				{
					gx = G.at<Vec2d>(k+5*i,l+j)[0];
					gy = G.at<Vec2d>(k+5*i,l+j)[1];

					if(k<=1 && gy>=0.0 && i!=0)
					{
						GDM.at<Vec4d>(i-1,j)[0] += w2*gy;
						GDM.at<Vec4d>(i,j)[0] += w1*gy;
					}
					else if(k<=1 && gy<0.0 && i!=0)
					{
						GDM.at<Vec4d>(i-1,j)[1] += -w2*gy;
						GDM.at<Vec4d>(i,j)[1] += -w1*gy;
					}

					if(k<=1 && gx>=0.0 && i!=0)
					{
						GDM.at<Vec4d>(i-1,j)[2] += w2*gx;
						GDM.at<Vec4d>(i,j)[2] += w1*gx;
					}
					else if(k<=1 && gx<0.0 && i!=0)
					{
						GDM.at<Vec4d>(i-1,j)[3] += -w2*gx;
						GDM.at<Vec4d>(i,j)[3] += -w1*gx;
					}

					if(k==2 && gy>=0.0)
					{
						GDM.at<Vec4d>(i,j)[0] += gy;
					}
					else if(k==2 && gy<0.0)
					{
						GDM.at<Vec4d>(i,j)[1] += -gy;
					}
					if(k==2 && gx>=0.0)
					{
						GDM.at<Vec4d>(i,j)[2] += gx;
					}
					else if(k==2 && gx<0.0)
					{
						GDM.at<Vec4d>(i,j)[3] += -gx;
					}

					if(k>=3 && gy>=0.0 && i!=GDM.rows-1)
					{
						GDM.at<Vec4d>(i+1,j)[0] += w2*gy;
						GDM.at<Vec4d>(i,j)[0] += w1*gy;
					}
					else if(k>=3 && gy<0.0 && i!=GDM.rows-1)
					{
						GDM.at<Vec4d>(i+1,j)[1] += -w2*gy;
						GDM.at<Vec4d>(i,j)[1] += -w1*gy;
					}

					if(k>=3 && gx>=0.0 && i!=GDM.rows-1)
					{
						GDM.at<Vec4d>(i+1,j)[2] += w2*gx;
						GDM.at<Vec4d>(i,j)[2] += w1*gx;
					}
					else if(k>=3 && gx<0.0 && i!=GDM.rows-1)
					{
						GDM.at<Vec4d>(i+1,j)[3] += -w2*gx;
						GDM.at<Vec4d>(i,j)[3] += -w1*gx;
					}
				}
			}
		}
	}

	Mat mean = Mat::zeros(4*9,1,CV_64FC1);
	Mat stddev = Mat::zeros(4*9,1,CV_64FC1);
	double mag_mean = 0.0, mag_stddev = 0.0;

	for(int i=0; i<GDM.cols; i++)
	{
		for(int j=0; j<GDM.rows; j++)
		{
			for(int k=0; k<4; k++)
			{
				mean.at<double>(j*4+k,0) += GDM.at<Vec4d>(j,i)[k];
			}
		}
	}
	for(int i=0; i<mean.rows; i++)
	{
		mean.at<double>(i,0) /= (double)GDM.cols;
		mag_mean += mean.at<double>(i,0)*mean.at<double>(i,0);
	}
	mag_mean = sqrt(mag_mean);

	for(int i=0; i<GDM.cols; i++)
	{
		for(int j=0; j<GDM.rows; j++)
		{
			for(int k=0; k<4; k++)
			{
				stddev.at<double>(j*4+k,0) += (GDM.at<Vec4d>(j,i)[k] - mean.at<double>(j*4+k,0))*(GDM.at<Vec4d>(j,i)[k] - mean.at<double>(j*4+k,0));
			}
		}
	}
	for(int i=0; i<stddev.rows; i++)
	{
		stddev.at<double>(i,0) /= (double)GDM.cols;
		stddev.at<double>(i,0) = sqrt(stddev.at<double>(i,0));
		mag_stddev += stddev.at<double>(i,0)*stddev.at<double>(i,0);
	}
	mag_stddev = sqrt(mag_stddev);

    //normalize
	for(int i=0; i<8*9; i++)
	{
		if(i<8*9/2)
			msld.at<float>(i,0) = (float)(mean.at<double>(i,0)/mag_mean) / 1.414213562f;
		else
			msld.at<float>(i,0) = (float)(stddev.at<double>(i-8*9/2,0)/mag_stddev) / 1.414213562f;
	}
}

void MSLD::GetMSLD(Mat & src, vector<SEGMENT> * lines)
{
	for(unsigned int i=0; i < lines->size(); i++)
	{
		SEGMENT seg = lines->at(i);

		Mat msld = Mat::zeros(72,1,CV_32FC1);

		double length = sqrt( (seg.x1-seg.x2)*(seg.x1-seg.x2) + (seg.y1-seg.y2)*(seg.y1-seg.y2) );
		Mat patch = Mat::zeros(45+2, round(length)+6, CV_8UC1);
		GetImagePatch(src, patch, seg);
		CalcMSLD(patch, msld);
//		print_matrix(msld);
		lines->at(i).msld = msld.clone();
	}
}
