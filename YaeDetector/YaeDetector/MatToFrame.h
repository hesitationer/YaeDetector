#pragma once
#include "stdafx.h"
PVideoFrame MatToFrame(Mat &mat, IScriptEnvironment* env, VideoInfo vi) {

	std::wstring str = L"REMOVE SUBTITLE: " + std::to_wstring(vi.height) +L" "+ std::to_wstring(vi.width);OutputDebugString(str.data());
	PVideoFrame frame = env->NewVideoFrame(vi);
	BYTE* raw_data = frame->GetWritePtr();
	int x, y, n = 0, u = 0;
	const int channel_length = mat.channels();
	const int rows = mat.rows;
	const int cols = mat.cols;
	const int count = rows * cols * channel_length;
	if (channel_length == 1) {
		//we use psudo-gray scale to RGB for debug
		Mat rgbImage;
		cvtColor(mat, rgbImage, cv::COLOR_GRAY2BGR);		
		memcpy(raw_data, rgbImage.data, count*3);
	}
	else {
		memcpy(raw_data, mat.data, count); //use memcpy faster than step iterate
	}
	raw_data = mat.data;
	return frame;
}