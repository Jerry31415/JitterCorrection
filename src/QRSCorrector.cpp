#include "QRSCorrector.h"

QRSCorrector::QRSCorrector(Ecg& card, QRSMarkers& qrs, const TemplateInfo& info){
	(*this).info = info;
	(*this).card = card;
	markers = std::vector<float>(qrs.size());
	for (int i = 0; i < qrs.size(); ++i) markers[i] = qrs[i];
}

float QRSCorrection(Ecg& card, QRSMarkers& qrs, const TemplateInfo& info){
	QRSCorrector corr(card, qrs, info);
	corr.init(qrs.size()); // инициализируем число параметров
	corr.info.template_index = info.template_index; // индекс целевого маркера
	std::vector<bool> trainable_flags(qrs.size(), true);
	float step = 0.1f; // начальный шаг
	trainable_flags[info.template_index] = false;
	float Q = -corr.S(corr.markers); 
	int max_itter = 8; // число иттераций алгоритма
	int itter = 0;
	while (true){
		// шаг градиентного спуска
		corr.step(corr.markers, step, trainable_flags, step);
		float Qt = -corr.S(corr.markers);
		if (Qt > Q){ // если средн€€ коррел€ци€ выросла, то модифицируем маркеры
			for (int i = 0; i < qrs.size(); ++i) qrs[i] = corr.markers[i];
			Q = Qt;
		}
		else { // если качество не изменилось (или ухудшилось)
			// возвращаемс€ к предыдущей аппроксимации маркеров
			for (int i = 0; i < qrs.size(); ++i) corr.markers[i] = qrs[i];
			// уменьшаем шаг на пор€док
			step /= 10;
			// если прошло max_itter изменений шага, то алгоритм завершает свою работу
			if (itter == max_itter) break;
			// увеличивает счетчик иттераций
			++itter;
		}
	}
	// возвращем качество разметки
	return MarkerQuality(card, qrs, info);
}

