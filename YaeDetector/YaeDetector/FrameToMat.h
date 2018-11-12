#pragma once
#include "stdafx.h"

Mat FrameToMat(PVideoFrame &frame) {
	const int width = frame->GetRowSize() / 3;
	const int height = frame->GetHeight();
	BYTE* frameData = (BYTE*)frame->GetReadPtr();
	Mat mat = Mat(height, width, CV_8UC3, frameData);
	return mat.clone();
}