#include <iostream>
#include <cmath>
using namespace std;

double Beff(double bw, double Sw, double hf, double ln, double bf, bool WingWidthType) {
	if (hf >= 0.5 * bw) {
		return min({ bf, 4 * bw });
	}
	else {
		if (WingWidthType == true) {
			return bw + min({ Sw / 2.0, 6.0 * hf, ln / 12.0 });
		}
		else {
			return bw + min({ Sw / 2.0, 8.0 * hf, ln / 12.0 });
		}
	}
}

double AsMinValue(double bw, double fc, double fy, double EffectiveDepth) {
	auto As1 = bw * EffectiveDepth * 3.0 * sqrt(fc) / fy;
	auto As2 = bw * EffectiveDepth * 200.0 / fy;
	return max({ As1, As2 });
}

double AsMaxValue(double bw, double EffectiveDepth, double ReinforceRatio) {
	return bw * EffectiveDepth * ReinforceRatio;
}