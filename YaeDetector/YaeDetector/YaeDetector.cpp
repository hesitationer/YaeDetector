// YaeDetector.cpp : Defines the exported functions for the DLL application.
//

#include "stdafx.h"
#include "HasSubtitle.h"
#include "YaeDetector.h"

const AVS_Linkage *AVS_linkage = 0;

AVSValue __cdecl Create_YaeDetector(AVSValue args, void* user_data, IScriptEnvironment* env) {
	return new YaeDetector(args, env);
}

extern "C" __declspec(dllexport) const char* __stdcall AvisynthPluginInit3(IScriptEnvironment* env, const AVS_Linkage* const vectors) {
	AVS_linkage = vectors;
	env->AddFunction("HasSubtitle", "c[Frame]i[Left]i[Right]i[Top]i[Bottom]i[StrokeWidth]i[StrokeGap]i[ColorGap]i", HasSubtitle, 0);
	env->AddFunction("YaeDetector", "c[Left]i[Right]i[Top]i[Bottom]i[StrokeWidth]i[StrokeGap]i[ColorGap]i", Create_YaeDetector, 0);
	return "YaeDetector plugin";
}
