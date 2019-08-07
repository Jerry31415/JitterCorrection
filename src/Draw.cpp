#include "ECG.h"
#include "QRSMarkers.h"
#include "Draw.h"
#include <opencv2\imgproc\imgproc.hpp>
#include <opencv\cv.h>
using namespace cv;

void DrawEcg(Ecg& card, Mat& graph, int channel){
	int height = 1024;
	float max_val(card.maxValue(channel));
	graph = Mat::zeros(height, card.size(), CV_8UC1);
	Point2i prev_point, curr_point;
	float val = card[0].value[channel];
	int pos_y = height / 2 - (height - 2)*(val / 2 * max_val);
	prev_point = Point2i(0, pos_y);
	for (int i = 1; i < card.size(); ++i){
		val = card[i].value[channel];
		pos_y = height / 2 - (height - 2)*(val / 2 * max_val);
		if (pos_y < 0 || pos_y >= graph.rows){
			std::cout << "Error: pos_y=" << pos_y << " i=" << i << "\n";
			std::cin.get();
		}
		curr_point.x = i;
		curr_point.y = pos_y;
		line(graph, prev_point, curr_point, Scalar(255));
		prev_point = curr_point;
	}
}

void DrawEcgWithQRSMarkers(Ecg& card, QRSMarkers& qrs, Mat& graph, int channel){
	float max_val(card.maxValue(channel));
	int height = 1024;
	graph = Mat::zeros(height, card.size(), CV_8UC3);
	line(graph, Point2i(0, height / 2), Point2i(card.size() - 1, height / 2), Scalar(255, 0, 0), 2);
	Point2i prev_point, curr_point;
	float val = card[0].value[channel];
	int pos_y = height / 2 - (height - 2)*(val / 2 * max_val);
	prev_point = Point2i(0, pos_y);
	int cnt = 0;
	for (int i = 1; i < card.size(); ++i){
		if (abs(qrs[cnt] - card[i].time)<0.01){
			line(graph, Point2i(i, 0), Point2i(i, height - 1), Scalar(0, 255, 0), 1);
			++cnt;
			putText(graph, toString<float>(qrs[cnt]), Point2i(i + 8, 32),
				CV_FONT_HERSHEY_DUPLEX,
				1, Scalar(0, 255, 0), 1);
		}
		val = card[i].value[channel];
		pos_y = height / 2 - (height - 2)*(val / 2 * max_val);
		if (pos_y < 0 || pos_y >= graph.rows){
			std::cout << "Error: pos_y=" << pos_y << " i=" << i << "\n";
			std::cin.get();
		}
		curr_point.x = i;
		curr_point.y = pos_y;
		line(graph, prev_point, curr_point, Scalar(255, 255, 255), 1);
		prev_point = curr_point;
	}
}

void DrawCardTemplate(Ecg& card, QRSMarkers& qrs, const TemplateInfo& info, Mat& dst, std::string text){

	int height = 1024;
	int scale_width = 16;
	int cnt = -1;

	float begin_time_range = qrs[info.template_index] + info.begin_time_template / 1000;
	float end_time_range = qrs[info.template_index] + info.end_time_template / 1000;
	Vec2i tind = card.range2indexes(begin_time_range, end_time_range);
	int s = tind[1] - tind[0];
	for (int qrs_index = 0; qrs_index < qrs.size(); ++qrs_index){
		begin_time_range = qrs[qrs_index] + info.begin_time_template / 1000;
		end_time_range = qrs[qrs_index] + info.end_time_template / 1000;
		Vec2i rind = card.range2indexes(begin_time_range, end_time_range);
		int si = rind[1] - rind[0];
		if (si != s){
			int n = std::max(s, si);
			if (si < s){
				if (rind[0] == 0) rind[0] = si - s;
				else if (rind[1] == (card.size() - 1)) rind[1] += s - si;
			}
		}
		if (cnt < 0){
			dst = Mat::zeros(height, scale_width*(rind[1] - rind[0]), CV_8UC3);
		}
		float max_val(card.maxValue(info.channel));

		Point2i prev_point, curr_point;
		float val = card[rind[0]].value[info.channel];
		int pos_y = height / 2 - (height - 2)*(val / 2 * max_val);

		prev_point = Point2i(0, pos_y);
		cnt = 1;
		for (int i = rind[0] + 1; i < rind[1]; ++i, cnt += scale_width){
			val = card[i].value[info.channel];
			pos_y = height / 2 - (height - 2)*(val / 2 * max_val);
			if (pos_y < 0 || pos_y >= dst.rows){
				std::cout << "Error: pos_y=" << pos_y << " i=" << i << "\n";
				std::cin.get();
			}
			curr_point.x = cnt;
			curr_point.y = pos_y;
			line(dst, prev_point, curr_point, Scalar(255, 255, 255), 1);
			prev_point = curr_point;
		}

	}
	putText(dst, text, Point2i(8, 32), CV_FONT_HERSHEY_DUPLEX, 1, Scalar(0, 255, 0), 1);
}

