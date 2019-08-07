#include "QRSMarkers.h"
#include "utility.h"
#include <vector>

float& QRSMarkers::operator[] (int index) {
	return time[index];
}

size_t QRSMarkers::size() const {
	return time.size();
}

QRSMarkers QRSMarkers::FromTxt(const std::string& filename) {
	QRSMarkers res;
	std::vector<std::string> lines;
	try{
		readFile(filename, lines);
	}
	catch (std::runtime_error& e){
		std::cout << e.what() << "\n";
	}
	for (int i = 0; i < lines.size(); ++i){
		size_t last_space_index;
		if ((last_space_index = lines[i].rfind(" ")) != std::string::npos){
			res.time.push_back(TimeString2float(lines[i].substr(last_space_index + 1)));
		}
	}
	return res;
}