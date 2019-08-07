#include "ECG.h"
#include "utility.h"

EPoint EPoint::FromString(const std::string& str, int channels){
	EPoint p;
	p.time = parseTimeFromString(str);
	size_t index;
	if ((index = str.find(',')) != std::string::npos){
		std::stringstream ss;
		ss << str.substr(index + 1);
		std::string svalue;
		for (int i = 0; i < channels; ++i){
			std::getline(ss, svalue, ',');
			p.value.push_back(std::stof(svalue));
		}
	}
	return p;
}

EPoint& Ecg::operator[] (int index) {
	return (index<0) ? curve[0] : (index >= size()) ? curve[size() - 1] : curve[index];
}

inline size_t Ecg::size() const {
	return curve.size();
}

size_t Ecg::depth() const {
	return (curve.empty()) ? 0 : curve[0].value.size();
}

float Ecg::maxValue(int channel) const {
	float max_val = curve[0].value[channel];
	for (size_t i = 1; i < curve.size(); ++i){
		if (curve[i].value[channel]>max_val)
			max_val = curve[i].value[channel];
	}
	return max_val;
}

float Ecg::minValue(int channel) const {
	float min_val = curve[0].value[channel];
	for (size_t i = 1; i < curve.size(); ++i){
		if (curve[i].value[channel]<min_val)
			min_val = curve[i].value[channel];
	}
	return min_val;
}

Vec2i Ecg::range2indexes(float begin_time_range, float end_time_range){
	int begin_index(0), end_index(size() - 1);
	int k = 0;
	for (; k < size(); ++k){
		if (curve[k].time >= begin_time_range){
			begin_index = k;
			break;
		}
	}
	for (; k < size(); ++k){
		if (curve[k].time >= end_time_range){
			end_index = k;
			break;
		}
	}
	return Vec2i(begin_index, end_index);
}

float Ecg::meanInRange(const Vec2i& irange, int channel){
	int n = irange[1] - irange[0];
	float res(0);
	for (int i = 0; i < n; ++i){
		res += (*this)[irange[0] + i].value[channel];
	}
	return res / n;
}

float Ecg::covariation(const Vec2i& irange0, const Vec2i& irange1, int channel){
	int n = irange0[1] - irange0[0];
	float mean0 = meanInRange(irange0, channel);
	float mean1 = meanInRange(irange1, channel);
	float res(0), v0, v1;
	for (int i = 0; i < n; ++i){
		v0 = (*this)[irange0[0] + i].value[channel];
		v1 = (*this)[irange1[0] + i].value[channel];
		res += (v0 - mean0)*(v1 - mean1);
	}
	return res;
}

float Ecg::dispersionInRange(const Vec2i& irange, int channel){
	int n = irange[1] - irange[0];
	float res(0), v;
	float mean = meanInRange(irange, channel);
	for (int i = 0; i < n; ++i){
		v = (*this)[irange[0] + i].value[channel] - mean;
		res += v*v;
	}
	return res;
}

Ecg Ecg::FromCSV(const std::string& filename) {
	auto getSymbolNumber = [](const std::string& str, char symbol){
		int cnt = 0;
		for (auto& e : str) cnt += (e == symbol);
		return cnt;
	};
	Ecg res;
	std::vector<std::string> lines;
	try{
		readFile(filename, lines);
	}
	catch (std::runtime_error& e){
		std::cout << e.what() << "\n";
	}
	int channelsNumber = getSymbolNumber(lines[0], ',');
	for (size_t i = 1; i < lines.size(); ++i){
		res.curve.push_back(EPoint::FromString(lines[i], channelsNumber));
	}
	return res;
}

