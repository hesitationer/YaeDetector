#pragma once
#include "stdafx.h"
#include "SubtitleDetector.h"
#include "MatToFrame.h"
#include "FrameToMat.h"

class YaeDetector : public GenericVideoFilter {
public:
	YaeDetector(AVSValue args, IScriptEnvironment* env);
	PVideoFrame __stdcall GetFrame(int n, IScriptEnvironment* env);
private:
	int left, right, top, bottom; // Position where subtitle appear in frame
	int stokeWidth, stokeGap;
	Mat smallKernel, bigKernel, stokeKernel;
	Mat prevFrame;
	int colorGap;
};

YaeDetector::YaeDetector(AVSValue args, IScriptEnvironment* env) :
	GenericVideoFilter(args[0].AsClip()) {
	if (vi.IsPlanar() || !vi.IsRGB()) {
		env->ThrowError("YaeRemover: input must be RGB24. Precede YaeRemover() with ConvertRGB24()");
	}
	else {
		this->left = args[1].IsInt() ? args[1].AsInt() : 0;
		this->right = args[2].IsInt() ? args[2].AsInt() : vi.width;
		this->top = args[3].IsInt() ? args[3].AsInt() : 0;
		this->bottom = args[4].IsInt() ? args[4].AsInt() : vi.height;
		this->stokeWidth = args[5].IsInt() ? args[5].AsInt() : 3;
		this->stokeGap = args[6].IsInt() ? args[6].AsInt() : 5;
		this->colorGap = args[7].IsInt() ? args[7].AsInt() : 30;

		int smallWidth = stokeWidth - stokeGap > 0 ? (stokeWidth - stokeGap) * 2 + 1 : 3;
		int bigWidth = (stokeWidth + stokeGap) * 2 + 1;
		this->smallKernel = getStructuringElement(MORPH_ELLIPSE, Size(smallWidth, smallWidth));
		this->bigKernel = getStructuringElement(MORPH_ELLIPSE, Size(bigWidth, bigWidth));
		this->stokeKernel = getStructuringElement(MORPH_ELLIPSE, Size(stokeWidth * 2 + 1, stokeWidth * 2 + 1));
	}
}

PVideoFrame __stdcall YaeDetector::GetFrame(int n, IScriptEnvironment* env) {
	PVideoFrame src = child->GetFrame(n, env);
	Mat frame = FrameToMat(src);
	Mat blankFrame(frame.rows, frame.cols, CV_8UC3, Scalar(255,255,255));
	Rect subtitlePosition = Rect(left, frame.rows - this->bottom, this->right - this->left, this->bottom - this->top);
	Mat subtitleFrame = frame(subtitlePosition);
	Mat blackMask;
	inRange(subtitleFrame, Scalar(0, 0, 0), Scalar(colorGap, colorGap, colorGap), blackMask);
	if (countNonZero(blackMask) == 0) {
		cvtColor(blackMask, blackMask, cv::COLOR_GRAY2BGR);
		blackMask.copyTo(frame(subtitlePosition));
		return MatToFrame(frame, env, vi);
	}
	Mat inpainedMask = SubtitleDetector(blackMask, bigKernel, smallKernel);
	dilate(inpainedMask, inpainedMask, this->stokeKernel);
	cvtColor(inpainedMask, inpainedMask, cv::COLOR_GRAY2BGR);
	inpainedMask.copyTo(frame(subtitlePosition));
	return MatToFrame(frame, env, vi);
}

//std::wstring str = L"REMOVE SUBTITLE: BEFORE frameToArray";OutputDebugString(str.data());
