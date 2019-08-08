#include "QRSCorrector.h"

QRSCorrector::QRSCorrector(Ecg& card, QRSMarkers& qrs, const TemplateInfo& info){
	(*this).info = info;
	(*this).card = card;
	markers = std::vector<float>(qrs.size());
	for (int i = 0; i < qrs.size(); ++i) markers[i] = qrs[i];
}

float QRSCorrection(Ecg& card, QRSMarkers& qrs, const TemplateInfo& info){
	QRSCorrector corr(card, qrs, info);
	corr.init(qrs.size()); // �������������� ����� ����������
	corr.info.template_index = info.template_index; // ������ �������� �������
	std::vector<bool> trainable_flags(qrs.size(), false);
	float step = 0.1f; // ��������� ���
	float Q;
	std::vector<int> cindx;
	for (int i = 0; i < qrs.size(); ++i){
		if (i == info.template_index) continue;
		float C = PCorrelation(card, qrs, info.template_index, i, info.begin_time_template, info.end_time_template, info.channel);		
		if (C >= info.compare_treshold) {
			cindx.push_back(i);
		}
	}
	for (auto& i : cindx){
		step = 0.1f;
		Q = PCorrelation(card, qrs, info.template_index, i, info.begin_time_template, info.end_time_template, info.channel);
		trainable_flags[i] = true;
		int itter = 0;
		while (true){
			// ��� ������������ ������
			corr.step(corr.markers, step, trainable_flags, step);
			float Qt = PCorrelation(card, corr.markers, info.template_index, i, info.begin_time_template, info.end_time_template, info.channel);
			if (Qt > Q){ // ���� ������� ���������� �������, �� ������������ �������
				for (int i = 0; i < qrs.size(); ++i) qrs[i] = corr.markers[i];
				Q = Qt;
			}
			else { // ���� �������� �� ���������� (��� ����������)
				// ������������ � ���������� ������������� ��������
				for (int i = 0; i < qrs.size(); ++i) corr.markers[i] = qrs[i];
				// ��������� ��� �� �������
				step /= 10;
				// ���� ������ max_itter ��������� ����, �� �������� ��������� ���� ������
				if (itter == info.max_itter) break;
				// ����������� ������� ���������
				++itter;
			}
		}
		trainable_flags[i] = false;
	}
	// ��������� �������� ��������
	return MarkerQuality(card, qrs, info);
}

