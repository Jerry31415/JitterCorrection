#include "utility.h"

void readFile(const std::string& filename, std::vector<std::string>& lines){
	std::ifstream f(filename.c_str());
	if (!f) {
		throw std::runtime_error("File with name \'" + filename + "\' - not found");
	}
	std::string line;
	while (std::getline(f, line)) lines.push_back(line);
	f.close();
}

float TimeString2float (const std::string& time){
	size_t index;
	int MM;
	float SS;
	if ((index = time.find(":")) != std::string::npos){
		MM = std::stoi(time.substr(0, index));
		SS = std::stof(time.substr(index + 1));
	}
	return (MM>0) ? (60 * MM + SS) : SS;
}

float parseTimeFromString(const std::string& str){
	std::string time;
	size_t beg_index, end_index;
	int MM;
	float SS;
	if ((beg_index = str.find('\'')) != std::string::npos){
		if ((end_index = str.rfind('\'')) != std::string::npos){
			time = str.substr(beg_index + 1, end_index - beg_index - 1);
			return TimeString2float(time);
		}
	}
	return -1;
}

TemplateInfo::TemplateInfo() {
	begin_time_template = 0;
	end_time_template = 0;
	template_index = 0;
	channel = 0;
	max_jitter = 0;
	compare_treshold = 1;
}

TemplateInfo::TemplateInfo(const TemplateInfo& obj){
	begin_time_template = obj.begin_time_template;
	end_time_template = obj.end_time_template;
	template_index = obj.template_index;
	channel = obj.channel;
	max_jitter = obj.max_jitter;
	compare_treshold = obj.compare_treshold;
}
