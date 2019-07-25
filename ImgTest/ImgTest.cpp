// ImgTest.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

//#include "lodepng.h"
#include "lodepng.h"
#include <iostream>
#include <vector>
#include <algorithm>
#include <numeric>
#include <chrono>

namespace cs = std::chrono;

int main(int argv, char* argc[])
{
	if (argv != 2) {
		return -1;
	}
	//const char* filename = "C:/Users/Arvid/Desktop/HIA Project/Samples 3/Hamilton/1,0 - 1,5/1,09.png";
	const char* filename = argc[1];

	std::vector<unsigned char> image; //the raw pixels
	unsigned width, height;

	//decode
	unsigned error = lodepng::decode(image, width, height, filename);

	std::cout << width << " " << height << "\n" << image[0] << "\n";

	//if there's an error, display it
	if (error) std::cout << "decoder error " << error << ": " << lodepng_error_text(error) << std::endl;

	auto t0 = cs::high_resolution_clock::now();

	std::vector<double> red;
	std::vector<double> green;
	std::vector<double> blue;

	for (size_t i = 0; i < image.size(); i += 4) {
		red.push_back(image[i]);
		green.push_back(image[i + 1]);
		blue.push_back(image[i + 2]);
	}

	const double r_max = *std::max_element(red.begin(), red.end());	
	const double g_max = *std::max_element(green.begin(), green.end());
	const double b_max = *std::max_element(blue.begin(), blue.end());

	std::transform(red.begin(), red.end(), red.begin(), [r_max](double d) { return d / r_max; });
	std::transform(green.begin(), green.end(), green.begin(), [g_max](double d) { return d / g_max; });
	std::transform(blue.begin(), blue.end(), blue.begin(), [b_max](double d) { return d / b_max; });

	std::vector<double> red_f;
	std::vector<double> green_f;
	std::vector<double> blue_f;
	
	for (size_t i = 0; i < red.size(); ++i) {
		double r_i = red[i];
		double g_i = green[i];
		double b_i = blue[i];

		double eps = 0.0001;
		if (((b_i + eps) / (r_i + eps) < 0.6) && ((r_i + eps) / (g_i + eps) > 0.8) && (r_i > 0.16)) {
			red_f.push_back(r_i);
			green_f.push_back(g_i);
			blue_f.push_back(b_i);
		}
	}

	double r_m = std::accumulate(red_f.begin(), red_f.end(), 0.0) / red_f.size();
	double g_m = std::accumulate(green_f.begin(), green_f.end(), 0.0) / green_f.size();
	double b_m = std::accumulate(blue_f.begin(), blue_f.end(), 0.0) / blue_f.size();

	//std::cout << r_m << " " << g_m << " " << b_m << "\n";
	std::cout << "Red - Green ratio: " << r_m / g_m << "\n";

	auto t1 = cs::high_resolution_clock::now();
	std::cout << "Processing image took: " << cs::duration_cast<cs::milliseconds>(t1 - t0).count() << "ms\n";

	std::cout << (double)red_f.size() / (red.size() - red_f.size()) << "\n";

	getchar();

	return r_m / g_m;
}

// R code for comparing r_m/g_m (standardised red mean to standardised green mean ratio) and their respective hb levels (e.g. ratio between 0.8 and 1.37 means hb is below 0.25g/L)
// This obviously needs to be translated into C++ for the program to function, and the action for each interval should be changed to something more useful, such as writing to a .csv file
// 
//if (sum(tempImg != c(0, 0, 0, 0)) / sum(tempImg == c(0, 0, 0, 0)) < (1 / 45)) {
//	file.rename(file, paste(workDirectory, "/UNKNOWN/", file, sep = ""))
//		print(paste(file, "is unknown1"))
//}
//else if (mean(red) / mean(green) > 0.8 & mean(red) / mean(green) < 1.37) {
//	print(paste(file, "has <0.25 g/L hemoglobin"))
//		file.rename(file, paste(workDirectory, "/LESS THAN 0,25/", file, sep = ""))
//}
//else if (mean(red) / mean(green) > 1.37 & mean(red) / mean(green) < 1.72) {
//	print(paste(file, "has 0.2 to 0.5 g/L hemoglobin"))
//		file.rename(file, paste(workDirectory, "/0,2 - 0,5/", file, sep = ""))
//}
//else if (mean(red) / mean(green) > 1.72 & mean(red) / mean(green) < 2.3) {
//	print(paste(file, "has 0.4 to 0.7 g/L hemoglobin"))
//		file.rename(file, paste(workDirectory, "/0,4 - 0,7/", file, sep = ""))
//}
//else if (mean(red) / mean(green) > 2.3 & mean(red) / mean(green) < 2.9) {
//	print(paste(file, "has 0.6 to 1.1 g/L hemoglobin"))
//		file.rename(file, paste(workDirectory, "/0,6 - 1,1/", file, sep = ""))
//}
//else if (mean(red) / mean(green) > 2.9 & mean(red) / mean(green) < 3.1) {
//	print(paste(file, "has 1.0 to 1.5 g/L hemoglobin"))
//		file.rename(file, paste(workDirectory, "/1,0 - 1,5/", file, sep = ""))
//}
//else if (mean(red) / mean(green) > 3.1 & mean(red) / mean(green) < 3.2) {
//	print(paste(file, "has 1.4 to 1.7 g/L hemoglobin"))
//		file.rename(file, paste(workDirectory, "/1,4 - 1,7/", file, sep = ""))
//}
//else if (mean(red) / mean(green) > 3.2) {
//	print(paste(file, "has >1.6 g/L hemoglobin"))
//		file.rename(file, paste(workDirectory, "/MORE THAN 1,6/", file, sep = ""))
//}
//else {
//	print(paste(file, "is unknown2"))
//		file.rename(file, paste(workDirectory, "/UNKNOWN/", file, sep = ""))
//}