#include "region.h"

CRegion::CRegion(uint32 index)
:m_index(index)
{
	m_colsZoomRate	= 2;
	m_rowsZoomRate	= 2;
	resolution  		= 0.001;
	alarm = 0;
}

CRegion::~CRegion()
{
	frameindex = 0;
	Rects.clear();
        vector< Rect >().swap(Rects);
        if(!mask.empty())
            mask.release();
        if(!foregrondframe.empty())
            foregrondframe.release();
        if(!TmpFrame.empty())
            TmpFrame.release();
}

void CRegion::sleep_release()
{
	alarm = 0;
	frameindex = 0;
}
void CRegion::pause_release()
{
	alarm = 0;
	frameindex = 0;
}

int CRegion::set_rectangle(vector <Rect> rect)
{
	uint8 i =0;
	Rects.clear();
	for(i = 0; i <rect.size(); i++)
	{
		Rect  tmp;
		tmp = rect[i];
		Rects.push_back(tmp);
	}

	if(Rects.size() == 0)
	{
		cout<<"camera "<<m_index<<" CRegion::set_rectangle wrong size is 0 "<<endl;
		return -1;
	}
	return 0;
}


void CRegion::motiondetective(Mat &dispalyFrame,Mat &morph)
{
	bool motionAlarmDone = false;

	for(unsigned int i = 0;i < Rects.size();i++)
	{
		Rect rt = Rects[i];
		if(rt.x + rt.width < morph.cols && rt.y + rt.height < morph.rows)
		{
			Mat tmp = morph(rt);
			int ret = countNonZero(tmp);
			tmp.release();
			motionAlarmDone = true;
			float rate = (float)ret / (float)(rt.width * rt.height);

			if(rate > resolution)
			{
				for(int i = rt.y;i < rt.y + rt.height;i++)
				{
					for(int j = rt.x;j < rt.x + rt.width;j++)
					{
						int R = dispalyFrame.at<Vec3b>(i, j).val[2];
						int G = dispalyFrame.at<Vec3b>(i, j).val[1];
						int B = dispalyFrame.at<Vec3b>(i, j).val[0];

						R = std::min(255, R + ret);
						G = std::max(0, G - ret);
						B = std::min(0, B - ret);
						dispalyFrame.at<Vec3b>(i, j) = Vec3b(B, G, R);
					}
				}
				alarm = 1;
				rectangle(dispalyFrame, rt, Scalar( 0, 0, 255 ), 2, 8, 0);
			}
			else
			{
				rectangle(dispalyFrame, rt, Scalar( 0, 255, 255 ), 2, 8, 0);
			}
		}
	}

	morph.release();

}

void CRegion::algorithmMorphology_Operations(Mat& src, Mat& dst)
{
		Mat element = getStructuringElement( cv::MORPH_RECT, Size( 3,3));
		Mat element1 = getStructuringElement( cv::MORPH_RECT, Size( 3,3 ));
		Mat tmp = Mat::zeros(src.rows, src.cols, src.type());
		morphologyEx(src, tmp, MORPH_OPEN, element);
		morphologyEx(tmp, dst, MORPH_CLOSE, element1);

		tmp.release();
		element.release();
		element1.release();
}

int CRegion::RegionDetectRun(Mat &dispalyFrame)
{
  alarm   =  0;
	dispalyFrame.copyTo(TmpFrame);

	m_zoomRows	=	TmpFrame.rows  /m_rowsZoomRate;
	m_zoomCols	= TmpFrame.cols   /m_colsZoomRate;

	w_Rate = (float)TmpFrame.cols / m_zoomCols;
	h_Rate = (float)TmpFrame.rows / m_zoomRows;

	mog(TmpFrame,foregrondframe,0.001);   // 0.001

	frameindex++;
	if(frameindex >= 250) frameindex= 250;
	if(frameindex <250) return 2;

	foregrondframe.copyTo(mask);

	cv::erode(mask, mask, cv::Mat());

	cv::dilate(mask, mask, cv::Mat());

	algorithmMorphology_Operations(mask, mask);

	motiondetective(dispalyFrame,mask);

	TmpFrame.release();

	return 0;
}
