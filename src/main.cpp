#include <Windows.h>
#include "opencv2\imgcodecs\imgcodecs.hpp"
#include "QRSCorrector.h"
#include "Draw.h"

using namespace cv;

std::string path = "report";
std::string ecg_filename;
std::string qrs_markers_filename;
QRSMarkers srcMarkers;
QRSMarkers rr;
Ecg card;

void createReport(const TemplateInfo& info){
	std::vector<float> time_shifts(rr.size());
	Mat srcCorr = calcCorrelationMatrix(card, srcMarkers, info.begin_time_template, info.end_time_template, info.channel);
	Mat dstCorr = calcCorrelationMatrix(card, rr, info.begin_time_template, info.end_time_template, info.channel);
	for (int i = 0; i < time_shifts.size(); ++i){
		time_shifts[i] = srcMarkers[i] - rr[i];
	}
	std::ofstream f("report.html");
	f << "<html>\n";
	f << "<table>\n";
	f << "<tr><td>Channel:</td><td>" << info.channel << "</td></tr>\n";
	f << "<tr><td>Template index:</td><td>" << info.template_index << "</td></tr>\n";
	f << "<tr><td>Time range:</td><td> [" << info.begin_time_template << ", " << info.end_time_template << "]</td></tr>\n";
	f << "</table>\n<hr>";
	
	f << "Time shifts:\n<table border=\"1\">\n";
	f << "<tr><td>Index:</td><td>Shift value:</td><td>srcCorr</td><td>dstCorr</td></tr>";
	for (int i = 0; i < time_shifts.size(); ++i) {
		f << "<tr><td>" << i << "</td>";
		f << "<td>" << time_shifts[i] << "</td>";
		f << "<td>" << srcCorr.at<float>(info.template_index, i) << "</td>";
		f << "<td>" << dstCorr.at<float>(info.template_index, i) << "</td>";
		f << "</tr>";
	}
	f << "</table>\n";
	f << "<br>Source template:\n";
	f << "<p><img src = \"report/src_template.png\" width=\"100%\"></p>";
	f << "The template after correction:\n";
	f << "<p><img src = \"report/dst_template.png\" width=\"100%\"></p>";
	f << "Source ecg:\n";
	f << "<p><img src = \"report/ecg_src.png\" width=\"100%\"></p>";
	f << "The ecg after correction:\n";
	f << "<p><img src = \"report/ecg_dst.png\" width=\"100%\"></p>";
	f << "<p></html>\n";
	f.close();
}

int main(int argc, char** argv){
	system(std::string("mkdir " + path).c_str());
	TemplateInfo info;
	info.channel = 0; // channel index
	info.template_index = 4; // marker index
	info.begin_time_template = -400; // begin template delta time
	info.end_time_template = 700; // end template delta time
	if (argc < 7){
		std::cerr << "Error: incorrect arguments\n";
		system("pause");
		std::exit(-1);
	}
	ecg_filename = argv[1];
	qrs_markers_filename = argv[2];
	info.channel = std::stoi(argv[3]);
	info.template_index = std::stoi(argv[4]);
	info.begin_time_template = std::stof(argv[5]);
	info.end_time_template = std::stof(argv[6]);
	std::string text_index = "; Marker index:" + toString(info.template_index);
	Mat im;
	// read ecg and qrs markers
	srcMarkers = QRSMarkers::FromTxt(qrs_markers_filename);
	rr = QRSMarkers::FromTxt(qrs_markers_filename);
	card = Ecg::FromCSV(ecg_filename);
	// check arguments
	if (info.channel < 0 || info.channel >= card.depth()){
		std::cerr << "Error: incorrect channel index (must be from 0 to " << card.depth() - 1 << ")\n";
		system("pause");
		std::exit(-1);
	}
	if (info.template_index < 0 || info.template_index >= rr.size()){
		std::cerr << "Error: incorrect template index (must be from 0 to " << rr.size() - 1 << ")\n";
		system("pause");
		std::exit(-1);
	}
	// draw source ecg with markers 
	DrawEcgWithQRSMarkers(card, rr, im, info.channel);
	imwrite(path + "/ecg_src.png", im);
	// calc quality of markers
	float Q = MarkerQuality(card, rr, info); 
	// draw source template
	DrawCardTemplate(card, rr, info, im, "Q=" + toString(Q) + text_index);
	imwrite(path + "/src_template.png", im);	
	// correction of markers
	Q = QRSCorrection(card, rr, info);
	// draw filtered template
	DrawCardTemplate(card, rr, info, im, "Q=" + toString(Q) + text_index);
	imwrite(path+"/dst_template.png", im);
	// draw ecg with filtered markers
	DrawEcgWithQRSMarkers(card, rr, im, info.channel);
	imwrite(path + "/ecg_dst.png", im);
	// create report.html file
	createReport(info);
	// open report.html file
	ShellExecute(NULL, "open", "report.html", NULL, NULL, SW_SHOWNORMAL);
}
