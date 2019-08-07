#ifndef QRSMarkers_H
#define QRSMarkers_H

#include <vector>
#include <string>
#include "utility.h"

class QRSMarkers{
public:
	std::vector<float> time;
	static QRSMarkers FromTxt(const std::string& filename);
	float& operator[] (int index);
	size_t size() const;	
};

#endif QRSMarkers_H