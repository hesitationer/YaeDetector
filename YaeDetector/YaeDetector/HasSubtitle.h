#pragma once

#include "stdafx.h"
#include "FrameToMat.h"
#include "SubtitleDetector.h"

AVSValue __cdecl HasSubtitle(AVSValue args, void* user_data, IScriptEnvironment* env)
{
	int frame_number, left, right, top, bottom, strokeWidth, strokeGap, colorGap;
	// Validate Input
	if (!args[0].IsClip()) {
		env->ThrowError("HasSubtitle: No clip supplied!");
	}
	if (!args[1].IsInt()) {
		env->ThrowError("HasSubtitle: No frame number supplied!");
	}

	PClip video = args[0].AsClip();
	VideoInfo vi = video->GetVideoInfo();

	if (vi.IsPlanar() || !vi.IsRGB()) {
		env->ThrowError("HasSubtitle: input must be RGB24. should use ConvertRGB24() before");
	}

	//Apply Default Variable
	frame_number = args[1].AsInt();
	left = args[2].IsInt() ? args[2].AsInt() : 0;
	right = args[3].IsInt() ? args[3].AsInt() : vi.width;
	top = args[4].IsInt() ? args[4].AsInt() : 0;
	bottom = args[5].IsInt() ? args[5].AsInt() : vi.height;
	strokeWidth = args[6].IsInt() ? args[6].AsInt() : 3;
	strokeGap = args[7].IsInt() ? args[7].AsInt() : 5;
	colorGap = args[8].IsInt() ? args[8].AsInt() : 30;

	int smallWidth = strokeWidth - strokeGap > 0 ? (strokeWidth - strokeGap) * 2 + 1 : 3;
	int bigWidth = (strokeWidth + strokeGap) * 2 + 1;
	Mat smallKernel = getStructuringElement(MORPH_ELLIPSE, Size(smallWidth, smallWidth));
	Mat bigKernel = getStructuringElement(MORPH_ELLIPSE, Size(bigWidth, bigWidth));
	Mat strokeKernel = getStructuringElement(MORPH_ELLIPSE, Size(strokeWidth * 2 + 1, strokeWidth * 2 + 1));

	//load video frame
	PVideoFrame src = video->GetFrame(frame_number, env);
	Mat frame = FrameToMat(src);
	Rect subtitlePosition = Rect(left, frame.rows - bottom, right - left, bottom - top);
	Mat subtitleFrame = frame(subtitlePosition);
	Mat blackMask;
	inRange(subtitleFrame, Scalar(0, 0, 0), Scalar(colorGap, colorGap, colorGap), blackMask);

	if (countNonZero(blackMask) == 0) {
		return (AVSValue)false;
	}

	Mat inpainedMask = SubtitleDetector(blackMask, bigKernel, smallKernel);
	// dilate(inpainedMask, inpainedMask, strokeKernel); //dilate mask for cover all subtitle

	if (countNonZero(inpainedMask) == 0) {
		return (AVSValue)false;
	}


	return (AVSValue)true;
}