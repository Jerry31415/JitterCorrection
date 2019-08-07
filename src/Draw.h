#ifndef DRAW_H
#define DRAW_H

#include "ECG.h"
#include "QRSMarkers.h"
#include "utility.h"

void DrawEcg(Ecg& card, Mat& graph, int channel);

void DrawEcgWithQRSMarkers(Ecg& card, QRSMarkers& qrs, Mat& graph, int channel);

void DrawCardTemplate(Ecg& card, QRSMarkers& qrs, const TemplateInfo& info, Mat& dst, std::string text = "");

#endif