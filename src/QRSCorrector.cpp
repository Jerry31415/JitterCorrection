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
	std::vector<bool> trainable_flags(qrs.size(), true);
	float step = 0.1f; // ��������� ���
	trainable_flags[info.template_index] = false;
	float Q = -corr.S(corr.markers); 
	int max_itter = 8; // ����� ��������� ���������
	int itter = 0;
	while (true){
		// ��� ������������ ������
		corr.step(corr.markers, step, trainable_flags, step);
		float Qt = -corr.S(corr.markers);
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
			if (itter == max_itter) break;
			// ����������� ������� ���������
			++itter;
		}
	}
	// ��������� �������� ��������
	return MarkerQuality(card, qrs, info);
}

