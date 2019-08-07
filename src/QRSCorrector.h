#ifndef QRS_CORRECTOR_H
#define QRS_CORRECTOR_H

#include "GDM.h"
#include "ECG.h"
#include "QRSMarkers.h"
#include "utility.h"

template<typename T>
Mat calcCorrelationMatrix(Ecg& card, T& qrs, float begin_time_delta, float end_time_delta, int channel = 0){
	Mat res = Mat::zeros(qrs.size(), qrs.size(), CV_32FC1);
	for (int i = 0; i < qrs.size(); ++i){
		for (int j = 0; j < qrs.size(); ++j){
			res.at<float>(i, j) = PCorrelation<T>(card, qrs, i, j, begin_time_delta, end_time_delta, channel);
		}
	}
	return res;
}

template<typename T>
float MarkerQuality(Ecg& card, T& qrs, const TemplateInfo& info){
	Mat cm = calcCorrelationMatrix(card, qrs, info.begin_time_template, info.end_time_template, info.channel);
	float Q = 0;
	for (int i = 0; i < cm.rows; ++i){
		if (i == info.template_index) continue;
		Q += cm.at<float>(i, info.template_index);
	}
	return Q / (cm.rows - 1);
}

template<typename T>
float MarkerQuality(Ecg& card, T& qrs, int marker_index,
	float begin_time_delta, float end_time_delta, int channel = 0){
	Mat cm = calcCorrelationMatrix(card, qrs, begin_time_delta, end_time_delta, channel);
	float Q = 0;
	for (int i = 0; i < cm.rows; ++i){
		if (i == marker_index) continue;
		Q += cm.at<float>(i, marker_index);
	}
	return Q / (cm.rows - 1);
}

template<typename T>
float QRSQuality(Ecg& card, T& qrs,
	float begin_time_delta, float end_time_delta,
	std::vector<float>& max_corr,
	std::vector<float>& time_shift,
	int channel = 0){
	Mat cm = calcCorrelationMatrix(card, qrs, begin_time_delta, end_time_delta, channel);
	max_corr = std::vector<float>(qrs.size());
	time_shift = std::vector<float>(qrs.size());
	float c;
	int max_corr_ind;
	for (int i = 0; i < cm.rows; ++i){
		c = -1;
		max_corr_ind = i;
		for (int j = 0; j < cm.cols; ++j){
			if (i == j) continue;
			if (c < cm.at<float>(i, j)){
				c = cm.at<float>(i, j);
				max_corr_ind = j;
			}
		}
		max_corr[i] = c;
		time_shift[i] = qrs[max_corr_ind] - qrs[i];
	}
	float Q = 0;
	for (int i = 0; i < max_corr.size(); ++i){
		Q += max_corr[i];
	}
	return Q / max_corr.size();
}

template<typename T = QRSMarkers>
float PCorrelation(Ecg& card, T& qrs, int ind0, int ind1,
	float begin_time_delta, float end_time_delta, int channel = 0){
	Vec2i ind_range0 = card.range2indexes(qrs[ind0] + begin_time_delta / 1000,
		qrs[ind0] + end_time_delta / 1000);
	Vec2i ind_range1 = card.range2indexes(qrs[ind1] + begin_time_delta / 1000,
		qrs[ind1] + end_time_delta / 1000);

	int s1 = ind_range0[1] - ind_range0[0];
	int s2 = ind_range1[1] - ind_range1[0];
	if (s1 != s2){ // если число элементов в выборке различно
		int n = std::max(s1, s2);
		if (s1 < s2){ // продлим отрезок ind_range0
			if (ind_range0[0] == 0) ind_range0[0] = s1 - s2;
			else if (ind_range0[1] == (card.size() - 1)) ind_range0[1] += s2 - s1;
		}
		else { // продлим отрезок ind_range1
			if (ind_range1[0] == 0) ind_range1[0] = s2 - s1;
			else if (ind_range1[1] == (card.size() - 1)) ind_range1[1] += s1 - s2;

		}
	}
	float cov = card.covariation(ind_range0, ind_range1, channel);
	float d0 = card.dispersionInRange(ind_range0, channel);
	float d1 = card.dispersionInRange(ind_range1, channel);
	return cov / sqrt(d0*d1);
}

class QRSCorrector : public GDM<float>{
public:
	TemplateInfo info;
	Ecg card;
	std::vector<float> markers;
	std::vector<float> max_corr;
	std::vector<float> time_shift;
	QRSCorrector(Ecg& card, QRSMarkers& qrs, const TemplateInfo& info);

	float S(std::vector<float>& markers){
		return -MarkerQuality<std::vector<float>>(card, markers, info.template_index, info.begin_time_template, info.end_time_template, info.channel);
	}
};

// Выполняет коррекцию маркеров qrs. 
// Возвращет качество маркеровки (средняя корреляция Пирсона)
float QRSCorrection(Ecg& card, QRSMarkers& qrs, const TemplateInfo& info);

#endif