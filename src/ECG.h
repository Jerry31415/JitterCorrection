#ifndef ECG_H
#define ECG_H

#include <opencv\cv.h>
#include <vector>
#include <iostream>
#include "utility.h"

using namespace cv;

class EPoint{
public:
	float time;
	std::vector<float> value;

	static EPoint FromString(const std::string& str, int channels);

	friend std::ostream& operator<<(std::ostream& os, const EPoint& obj){
		os << obj.time;
		for (auto& e : obj.value) os << " " << e;
		return os;
	}
};

/*
Контейнер для хранения кривой ЭКГ
*/
class Ecg{
public:
	std::vector<EPoint> curve; // кривые ЭКГ для нескольких каналов
	EPoint& operator[] (int index);
	inline size_t size() const;
	size_t depth() const;
	float maxValue(int channel) const;
	float minValue(int channel) const;
	Vec2i range2indexes(float begin_time_range, float end_time_range);
	float meanInRange(const Vec2i& irange, int channel);
	float covariation(const Vec2i& irange0, const Vec2i& irange1, int channel);
	float dispersionInRange(const Vec2i& irange, int channel);
	static Ecg FromCSV(const std::string& filename);
};

#endif