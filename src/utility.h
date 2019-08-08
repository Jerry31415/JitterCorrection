#ifndef UTILITY_H
#define UTILITY_H

#include <iostream>
#include <fstream>
#include <vector>
#include <sstream>

template<typename T>
std::string toString(const T& obj){
	std::stringstream ss;
	ss << obj;
	return ss.str();
}

void readFile(const std::string& filename, std::vector<std::string>& lines);
float TimeString2float(const std::string& time);
float parseTimeFromString(const std::string& str);

struct TemplateInfo{
	float begin_time_template;
	float end_time_template;
	float max_jitter;
	float compare_treshold;
	int template_index;
	int channel;
	int max_itter;
	TemplateInfo();
	TemplateInfo(const TemplateInfo& obj);
};

#endif