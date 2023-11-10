#include <iostream>
#include <cmath>
#include <vector>
#include "AutoRC.h"
using namespace std;

// ACI 319-19 APPENDIX B STEEL REINFORCEMENT INFORMATION
// ASTM STANDARD REINFORCING BARS
// Nominal diameter, in
double BarDiameter(int BarsSize) {
	switch (BarsSize) {
	case 3:
		return 0.375;
	case 4:
		return 0.500;
	case 5:
		return 0.625;
	case 6:
		return 0.750;
	case 7:
		return 0.875;
	case 8:
		return 1.000;
	case 9:
		return 1.128;
	case 10:
		return 1.270;
	case 11:
		return 1.410;
	case 14:
		return 1.693;
	case 18:
		return 2.257;
	default:
		cout << "Bars Size is Wrong!" << endl;
		return -1;
	}
}

// ACI 319-19 APPENDIX B STEEL REINFORCEMENT INFORMATION
// ASTM STANDARD REINFORCING BARS
// Nominal area, in^2
double BarArea(int BarsSize) {
	switch (BarsSize) {
	case 3:
		return 0.11;
	case 4:
		return 0.20;
	case 5:
		return 0.31;
	case 6:
		return 0.44;
	case 7:
		return 0.60;
	case 8:
		return 0.79;
	case 9:
		return 1.00;
	case 10:
		return 1.27;
	case 11:
		return 1.56;
	case 14:
		return 2.25;
	case 18:
		return 4.00;
	default:
		cout << "Bars Size is Wrong!" << endl;
		return -1;
	}
}

double ReinforceRatio(double Grade) {
	if (Grade == 60000.) {
		return 0.025;
	}
	else if (Grade == 80000.) {
		return 0.02;
	}
	else {
		cout << ">>ERROR!!!<<\nPlease make sure \"MainBarGrade\" is correct!" << endl;
		exit(0);
	}
}

// 是否為強震區
bool IsEarthquakeArea(RcData::_SDC_& SDC) {
	if (SDC == RcData::_SDC_::A || SDC == RcData::_SDC_::B || SDC == RcData::_SDC_::C) return false;
	else if (SDC == RcData::_SDC_::D || SDC == RcData::_SDC_::E || SDC == RcData::_SDC_::F) return true;
}

double CalculateDL(double Sw, double bw, double h, double hf, double Dl) {
	return (Sw * hf / 144. + bw / 12. * (h / 12. - hf / 12.)) * Dl / 1000.;
}

double CalculateLL(double Sw, double Ll) {
	return Sw * Ll / 12000.;
}

void Coordinate(RcData& Data) {
	// 撓曲主筋水平座標
	auto MainBarSpacing = (Data.bw - (Data.cc * 2.) - BarDiameter(Data.MainBar)) / double(Data.leading - 1);
	int MainBarCount = ceil(Data.leading / 2.);
	Data.Coordinate.MainBar_H.resize(MainBarCount);
	if (Data.leading == Data.UpperEachLayerCount[0]) {
		if (Data.OddAllowed == false) {
			for (int i = 0; i < MainBarCount; ++i) {
				if (i == 0)Data.Coordinate.MainBar_H[i] = MainBarSpacing / 2.;
				else Data.Coordinate.MainBar_H[i] += MainBarSpacing;
			}
		}
		else {
			for (int i = 0; i < MainBarCount; ++i) {
				if (i == 0)Data.Coordinate.MainBar_H[i] = 0.;
				else Data.Coordinate.MainBar_H[i] += MainBarSpacing;
			}
		}
	}
	else {
		if (Data.OddAllowed == false) {
			for (int i = 0; i < MainBarCount; ++i) {
				if (i == 0)Data.Coordinate.MainBar_H[i] = MainBarSpacing / 2.;
				else Data.Coordinate.MainBar_H[i] += MainBarSpacing;
			}
		}
		else {
			for (int i = 0; i < MainBarCount; ++i) {
				if (i == 0)Data.Coordinate.MainBar_H[i] = 0.;
				else Data.Coordinate.MainBar_H[i] += MainBarSpacing;
			}
		}
	}
	// 撓曲主筋垂直座標
	Data.Coordinate.MainBar_V = Data.PerLayerDMn2;
	// 扭矩主筋水平座標
	Data.Coordinate.TorsionBar_H = (Data.bw / 2.) - Data.cc - (BarDiameter(Data.TorsionBar) / 2.);
	// 扭矩主筋垂直座標
	auto TorsionBarSpacing = Data.h - (Data.cc * 2.) - (BarDiameter(Data.Stirrup) * 2.) - (BarDiameter(Data.MainBar) * 2.) / double(Data.VerticalTorsionBarCount - 1);
	for (int i = 0; i < Data.VerticalTorsionBarCount - 2; ++i) {
		if (i == 0)Data.Coordinate.TorsionBar_V[i] = Data.cc + BarDiameter(Data.Stirrup) + BarDiameter(Data.MainBar) + TorsionBarSpacing;
		else Data.Coordinate.TorsionBar_V[i] += TorsionBarSpacing;
	}
	// 腳水平座標
	int LegsCount = ceil(Data.legs / 2.);
	if (Data.OddAllowed == false) {
		for (int i = 0; i < LegsCount; ++i) {
			if (i == 0)Data.Coordinate.MainBar_H[i] = MainBarSpacing / 2.;
			else Data.Coordinate.MainBar_H[i] += MainBarSpacing;
		}
	}
	else {
		for (int i = 0; i < MainBarCount; ++i) {
			if (i == 0)Data.Coordinate.MainBar_H[i] = 0.;
			else Data.Coordinate.MainBar_H[i] += MainBarSpacing;
		}
	}

	// 用布林值
}