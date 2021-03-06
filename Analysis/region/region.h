#ifndef _REGION_H_
#define _REGION_H_

#include "Common.h"

class CRegion
{

public:
	CRegion(uint32 index);
	~CRegion();

	uint32 m_index;
	uint8 alarm;

	vector< Rect > Rects;
	uint16 frameindex;
	
	void sleep_release();
	void pause_release();
	int set_rectangle(vector <Rect> rect);
	int RegionDetectRun(Mat &dispalyFrame);
	float ChangRate;

private:
	Mat TmpFrame;


	float resolution;

	int     m_rowsZoomRate;
	int	    m_colsZoomRate;

	int     m_zoomRows;
	int     m_zoomCols;

	float 	w_Rate;    /* width and height rate */
	float		h_Rate;

	Mat mask;

	cv::BackgroundSubtractorMOG2 mog;

	Mat foregrondframe;

	void motiondetective(Mat &dispalyFrame,Mat &morph);
	void algorithmMorphology_Operations( Mat& src, Mat& dst );
};

#endif
