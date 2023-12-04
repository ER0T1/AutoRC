#include <iostream>
#include <cmath>
#include <vector>
#include <sstream>
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
	Data.Coordinate.TorsionBar_V.resize(Data.VerticalTorsionBarCount - 2);
	auto TorsionBarSpacing = Data.h - (Data.cc * 2.) - (BarDiameter(Data.Stirrup) * 2.) - (BarDiameter(Data.MainBar) * 2.) / double(Data.VerticalTorsionBarCount - 1);
	for (int i = 0; i < Data.VerticalTorsionBarCount - 2; ++i) {
		if (i == 0)Data.Coordinate.TorsionBar_V[i] = Data.cc + BarDiameter(Data.Stirrup) + BarDiameter(Data.MainBar) + TorsionBarSpacing;
		else Data.Coordinate.TorsionBar_V[i] += TorsionBarSpacing;
	}
	// 腳水平座標
	if (Data.leading == 2) if (Data.depend == 2) {
		Data.Coordinate.MainBarDepend_H = { true, true };
		if (Data.legs == 2) Data.Coordinate.legs_H = { true, true };
	}
	else if (Data.leading == 3) {
		if (Data.depend == 2) {
			Data.Coordinate.MainBarDepend_H = { true, false, true };
			if (Data.legs == 2) Data.Coordinate.legs_H = { true, false, true };
		}
		else if (Data.depend == 3) {
			Data.Coordinate.MainBarDepend_H = { true, true, true };
			if (Data.legs == 2) Data.Coordinate.legs_H = { true, false, true };
			else if (Data.legs == 3) Data.Coordinate.legs_H = { true, true, true };
		}
	}
	else if (Data.leading == 4) if (Data.depend == 4) {
		Data.Coordinate.MainBarDepend_H = { true, true, true, true };
		if (Data.legs == 4) Data.Coordinate.legs_H = { true, true, true, true };
	}
	else if (Data.leading == 5) {
		if (Data.depend == 3) {
			Data.Coordinate.MainBarDepend_H = { true, false, true, false, true };
			if (Data.legs == 3) Data.Coordinate.legs_H = { true, false, true, false, true };
		}
		else if (Data.depend == 4) {
			Data.Coordinate.MainBarDepend_H = { true, true, false, true, true };
			if (Data.legs == 4) Data.Coordinate.legs_H = { true, true, false, true, true };
		}
		else if (Data.depend == 5) {
			Data.Coordinate.MainBarDepend_H = { true, true, true, true, true };
			if (Data.legs == 3) Data.Coordinate.legs_H = { true, false, true, false, true };
			else if (Data.legs == 4) Data.Coordinate.legs_H = { true, true, false, true, true };
			else if (Data.legs == 5) Data.Coordinate.legs_H = { true, true, true, true, true };
		}
	}
	else if (Data.leading == 6) {
		if (Data.depend == 4) {
			Data.Coordinate.MainBarDepend_H = { true, false, true, true, false, true };
			if (Data.legs == 4) Data.Coordinate.legs_H = { true, false, true, true, false, true };
		}
		else if (Data.depend == 6) {
			Data.Coordinate.MainBarDepend_H = { true, true, true, true, true, true };
			if (Data.legs == 4) Data.Coordinate.legs_H = { true, false, true, true, false, true };
			else if (Data.legs == 6) Data.Coordinate.legs_H = { true, true, true, true, true, true };
		}
	}
	else if (Data.leading == 7) {
		if (Data.depend == 4) {
			Data.Coordinate.MainBarDepend_H = { true, false, true, false, true, false, true };
			if (Data.legs == 4) Data.Coordinate.legs_H = { true, false, true, false, true, false, true };
		}
		else if (Data.depend == 5) {
			Data.Coordinate.MainBarDepend_H = { true, false, true, true, true, false, true };
			if (Data.legs == 4) Data.Coordinate.legs_H = { true, false, true, false, true, false, true };
			else if (Data.legs == 5) Data.Coordinate.legs_H = { true, false, true, true, true, false, true };
		}
		else if (Data.depend == 6) {
			Data.Coordinate.MainBarDepend_H = { true, true, true, false, true, true, true };
			if (Data.legs == 4) Data.Coordinate.legs_H = { true, false, true, false, true, false, true };
			else if (Data.legs == 6) Data.Coordinate.legs_H = { true, true, true, false, true, true, true };
		}
		else if (Data.depend == 7) {
			Data.Coordinate.MainBarDepend_H = { true, true, true, true, true, true, true };
			if (Data.legs == 4) Data.Coordinate.legs_H = { true, false, true, false, true, false, true };
			else if (Data.legs == 5) Data.Coordinate.legs_H = { true, false, true, true, true, false, true };
			else if (Data.legs == 6) Data.Coordinate.legs_H = { true, true, true, false, true, true, true };
			else if (Data.legs == 7) Data.Coordinate.legs_H = { true, true, true, true, true, true, true };
		}
	}
	else if (Data.leading == 8) {
		if (Data.depend == 6) {
			Data.Coordinate.MainBarDepend_H = { true, true, false, true, true, false, true, true };
			if (Data.legs == 6) Data.Coordinate.legs_H = { true, true, false, true, true, false, true, true };
		}
		else if (Data.depend == 8) {
			Data.Coordinate.MainBarDepend_H = { true, true, true, true, true, true, true, true };
			if (Data.legs == 6) Data.Coordinate.legs_H = { true, true, false, true, true, false, true, true };
			else if (Data.legs == 8) Data.Coordinate.legs_H = { true, true, true, true, true, true, true, true };
		}
	}
	else if (Data.leading == 9) {
		if (Data.depend == 5) {
			Data.Coordinate.MainBarDepend_H = { true, false, true, false, true, false, true, false, true};
			if (Data.legs == 5) Data.Coordinate.legs_H = { true, false, true, false, true, false, true, false, true };
		}
		else if (Data.depend == 6) {
			Data.Coordinate.MainBarDepend_H = { true, false, true, true, false, true, true, false, true };
			if (Data.legs == 6) Data.Coordinate.legs_H = { true, false, true, true, false, true, true, false, true };
		}
		else if (Data.depend == 7) {
			Data.Coordinate.MainBarDepend_H = { true, false, true, true, true, true, true, false, true };
			if (Data.legs == 5) Data.Coordinate.legs_H = { true, false, true, false, true, false, true, false, true };
			else if (Data.legs == 6) Data.Coordinate.legs_H = { true, false, true, true, false, true, true, false, true };
			else if (Data.legs == 7) Data.Coordinate.legs_H = { true, false, true, true, true, true, true, false, true };
		}
		else if (Data.depend == 8) {
			Data.Coordinate.MainBarDepend_H = { true, true, true, true, false, true, true, true, true };
			if (Data.legs == 6) Data.Coordinate.legs_H = { true, false, true, true, false, true, true, false, true };
			else if (Data.legs == 8) Data.Coordinate.legs_H = { true, true, true, true, false, true, true, true, true };
		}
		else if (Data.depend == 9) {
			Data.Coordinate.MainBarDepend_H = { true, true, true, true, true, true, true, true, true };
			if (Data.legs == 5) Data.Coordinate.legs_H = { true, false, true, false, true, false, true, false, true };
			else if (Data.legs == 6) Data.Coordinate.legs_H = { true, false, true, true, false, true, true, false, true };
			else if (Data.legs == 7) Data.Coordinate.legs_H = { true, false, true, true, true, true, true, false, true };
			else if (Data.legs == 8) Data.Coordinate.legs_H = { true, true, true, true, false, true, true, true, true };
			else if (Data.legs == 9) Data.Coordinate.legs_H = { true, true, true, true, true, true, true, true, true };
		}
	}
	else if (Data.leading == 10) {
		if (Data.depend == 6) {
			Data.Coordinate.MainBarDepend_H = { true, false, true, false, true, true, false, true, false, true };
			if (Data.legs == 6) Data.Coordinate.legs_H = { true, false, true, false, true, true, false, true, false, true };
		}
		else if (Data.depend == 8) {
			Data.Coordinate.MainBarDepend_H = { true, false, true, true, true, true, true, true, false, true };
			if (Data.legs == 6) Data.Coordinate.legs_H = { true, false, true, false, true, true, false, true, false, true };
			else if (Data.legs == 8) Data.Coordinate.legs_H = { true, false, true, true, true, true, true, true, false, true };
		}
		else if (Data.depend == 10) {
			Data.Coordinate.MainBarDepend_H = { true, true, true, true, true, true, true, true, true, true };
			if (Data.legs == 6) Data.Coordinate.legs_H = { true, false, true, false, true, true, false, true, false, true };
			else if (Data.legs == 8) Data.Coordinate.legs_H = { true, false, true, true, true, true, true, true, false, true };
			else if (Data.legs == 10) Data.Coordinate.legs_H = { true, true, true, true, true, true, true, true, true, true };
		}
	}
	else if (Data.leading == 11) {
		if (Data.depend == 6) {
			Data.Coordinate.MainBarDepend_H = { true, false, true, false, true, false, true, false, true, false, true };
			if (Data.legs == 6) Data.Coordinate.legs_H = { true, false, true, false, true, false, true, false, true, false, true };
		}
		else if (Data.depend == 7) {
			Data.Coordinate.MainBarDepend_H = { true, false, true, false, true, true, true, false, true, false, true };
			if (Data.legs == 6) Data.Coordinate.legs_H = { true, false, true, false, true, false, true, false, true, false, true };
			else if (Data.legs == 7) Data.Coordinate.legs_H = { true, false, true, false, true, true, true, false, true, false, true };
		}
		else if (Data.depend == 8) {
			Data.Coordinate.MainBarDepend_H = { true, false, true, true, true, false, true, true, true, false, true };
			if (Data.legs == 6) Data.Coordinate.legs_H = { true, false, true, false, true, false, true, false, true, false, true };
			else if (Data.legs == 8) Data.Coordinate.legs_H = { true, false, true, true, true, false, true, true, true, false, true };
		}
		else if (Data.depend == 9) {
			Data.Coordinate.MainBarDepend_H = { true, false, true, true, true, true, true, true, true, false, true };
			if (Data.legs == 6) Data.Coordinate.legs_H = { true, false, true, false, true, false, true, false, true, false, true };
			else if (Data.legs == 7) Data.Coordinate.legs_H = { true, false, true, false, true, true, true, false, true, false, true };
			else if (Data.legs == 8) Data.Coordinate.legs_H = { true, false, true, true, true, false, true, true, true, false, true };
			else if (Data.legs == 9) Data.Coordinate.legs_H = { true, false, true, true, true, true, true, true, true, false, true };
		}
		else if (Data.depend == 10) {
			Data.Coordinate.MainBarDepend_H = { true, true, true, true, true, false, true, true, true, true, true };
			if (Data.legs == 6) Data.Coordinate.legs_H = { true, false, true, false, true, false, true, false, true, false, true };
			else if (Data.legs == 8) Data.Coordinate.legs_H = { true, false, true, true, true, false, true, true, true, false, true };
			else if (Data.legs == 10) Data.Coordinate.legs_H = { true, true, true, true, true, false, true, true, true, true, true };
		}
		else if (Data.depend == 11) {
			Data.Coordinate.MainBarDepend_H = { true, true, true, true, true, true, true, true, true, true, true };
			if (Data.legs == 6) Data.Coordinate.legs_H = { true, false, true, false, true, false, true, false, true, false, true };
			else if (Data.legs == 7) Data.Coordinate.legs_H = { true, false, true, false, true, true, true, false, true, false, true };
			else if (Data.legs == 8) Data.Coordinate.legs_H = { true, false, true, true, true, false, true, true, true, false, true };
			else if (Data.legs == 9) Data.Coordinate.legs_H = { true, false, true, true, true, true, true, true, true, false, true };
			else if (Data.legs == 10) Data.Coordinate.legs_H = { true, true, true, true, true, false, true, true, true, true, true };
			else if (Data.legs == 11) Data.Coordinate.legs_H = { true, true, true, true, true, true, true, true, true, true, true };
		}
	}
	else if (Data.leading == 12) {
		if (Data.depend == 8) {
			Data.Coordinate.MainBarDepend_H = { true, true, false, true, false, true, true, false, true, false, true, true };
			if (Data.legs == 8) Data.Coordinate.legs_H = { true, true, false, true, false, true, true, false, true, false, true, true };
		}
		else if (Data.depend == 10) {
			Data.Coordinate.MainBarDepend_H = { true, true, false, true, true, true, true, true, true, false, true, true };
			if (Data.legs == 8) Data.Coordinate.legs_H = { true, true, false, true, false, true, true, false, true, false, true, true };
			else if (Data.legs == 10) Data.Coordinate.legs_H = { true, true, false, true, true, true, true, true, true, false, true, true };
		}
		else if (Data.depend == 12) {
			Data.Coordinate.MainBarDepend_H = { true, true, true, true, true, true, true, true, true, true, true, true };
			if (Data.legs == 8) Data.Coordinate.legs_H = { true, true, false, true, false, true, true, false, true, false, true, true };
			else if (Data.legs == 10) Data.Coordinate.legs_H = { true, true, false, true, true, true, true, true, true, false, true, true };
			else if (Data.legs == 12) Data.Coordinate.legs_H = { true, true, true, true, true, true, true, true, true, true, true, true };
		}
	}
	else if (Data.leading == 13) {
		if (Data.depend == 7) {
			Data.Coordinate.MainBarDepend_H = { true, false, true, false, true, false, true, false, true, false, true, false, true };
			if (Data.legs == 7) Data.Coordinate.legs_H = { true, false, true, false, true, false, true, false, true, false, true, false, true };
		}
		else if (Data.depend == 8) {
			Data.Coordinate.MainBarDepend_H = { true, false, true, false, true, true, false, true, true, false, true, false, true };
			if (Data.legs == 8) Data.Coordinate.legs_H = { true, false, true, false, true, true, false, true, true, false, true, false, true };
		}			
		else if (Data.depend == 9) {
			Data.Coordinate.MainBarDepend_H = { true, false, true, false, true, true, true, true, true, false, true, false, true };
			if (Data.legs == 7) Data.Coordinate.legs_H = { true, false, true, false, true, false, true, false, true, false, true, false, true };
			else if (Data.legs == 8) Data.Coordinate.legs_H = { true, false, true, false, true, true, false, true, true, false, true, false, true };
			else if (Data.legs == 9) Data.Coordinate.legs_H = { true, false, true, false, true, true, true, true, true, false, true, false, true };
		}
		else if (Data.depend == 10) {
			Data.Coordinate.MainBarDepend_H = { true, false, true, true, true, true, false, true, true, true, true, false, true };
			if (Data.legs == 8) Data.Coordinate.legs_H = { true, false, true, false, true, true, false, true, true, false, true, false, true };
			else if (Data.legs == 10) Data.Coordinate.legs_H = { true, false, true, true, true, true, false, true, true, true, true, false, true };
		}
		else if (Data.depend == 11) {
			Data.Coordinate.MainBarDepend_H = { true, false, true, true, true, true, true, true, true, true, true, false, true };
			if (Data.legs == 7) Data.Coordinate.legs_H = { true, false, true, false, true, false, true, false, true, false, true, false, true };
			else if (Data.legs == 8) Data.Coordinate.legs_H = { true, false, true, false, true, true, false, true, true, false, true, false, true };
			else if (Data.legs == 9) Data.Coordinate.legs_H = { true, false, true, false, true, true, true, true, true, false, true, false, true };
			else if (Data.legs == 10) Data.Coordinate.legs_H = { true, false, true, true, true, true, false, true, true, true, true, false, true };
			else if (Data.legs == 11) Data.Coordinate.legs_H = { true, false, true, true, true, true, true, true, true, true, true, false, true };
		}
		else if (Data.depend == 12) {
			Data.Coordinate.MainBarDepend_H = { true, true, true, true, true, true, false, true, true, true, true, true, true };
			if (Data.legs == 8) Data.Coordinate.legs_H = { true, false, true, false, true, true, false, true, true, false, true, false, true };
			else if (Data.legs == 10) Data.Coordinate.legs_H = { true, false, true, true, true, true, false, true, true, true, true, false, true };
			else if (Data.legs == 12) Data.Coordinate.legs_H = { true, true, true, true, true, true, false, true, true, true, true, true, true };
		}
		else if (Data.depend == 13) {
			Data.Coordinate.MainBarDepend_H = { true, true, true, true, true, true, true, true, true, true, true, true, true };
			if (Data.legs == 7) Data.Coordinate.legs_H = { true, false, true, false, true, false, true, false, true, false, true, false, true };
			else if (Data.legs == 8) Data.Coordinate.legs_H = { true, false, true, false, true, true, false, true, true, false, true, false, true };
			else if (Data.legs == 9) Data.Coordinate.legs_H = { true, false, true, false, true, true, true, true, true, false, true, false, true };
			else if (Data.legs == 10) Data.Coordinate.legs_H = { true, false, true, true, true, true, false, true, true, true, true, false, true };
			else if (Data.legs == 11) Data.Coordinate.legs_H = { true, false, true, true, true, true, true, true, true, true, true, false, true };
			else if (Data.legs == 12) Data.Coordinate.legs_H = { true, true, true, true, true, true, false, true, true, true, true, true, true };
			else if (Data.legs == 13) Data.Coordinate.legs_H = { true, true, true, true, true, true, true, true, true, true, true, true, true };
		}
	}
	else if (Data.leading == 14) {
		if (Data.depend == 8) {
			Data.Coordinate.MainBarDepend_H = { true, false, true, false, true, false, true, true, false, true, false, true, false, true };
			if (Data.legs == 8) Data.Coordinate.legs_H = { true, false, true, false, true, false, true, true, false, true, false, true, false, true };
		}
		else if (Data.depend == 10) {
			Data.Coordinate.MainBarDepend_H = { true, false, true, false, true, true, true, true, true, true, false, true, false, true };
			if (Data.legs == 8) Data.Coordinate.legs_H = { true, false, true, false, true, false, true, true, false, true, false, true, false, true };
			else if (Data.legs == 10) Data.Coordinate.legs_H = { true, false, true, false, true, true, true, true, true, true, false, true, false, true };
		}
		else if (Data.depend == 12) {
			Data.Coordinate.MainBarDepend_H = { true, false, true, true, true, true, true, true, true, true, true, true, false, true };
			if (Data.legs == 8) Data.Coordinate.legs_H = { true, false, true, false, true, false, true, true, false, true, false, true, false, true };
			else if (Data.legs == 10) Data.Coordinate.legs_H = { true, false, true, false, true, true, true, true, true, true, false, true, false, true };
			else if (Data.legs == 12) Data.Coordinate.legs_H = { true, false, true, true, true, true, true, true, true, true, true, true, false, true };
		}
		else if (Data.depend == 14) {
			Data.Coordinate.MainBarDepend_H = { true, true, true, true, true, true, true, true, true, true, true, true, true, true };
			if (Data.legs == 8) Data.Coordinate.legs_H = { true, false, true, false, true, false, true, true, false, true, false, true, false, true };
			else if (Data.legs == 10) Data.Coordinate.legs_H = { true, false, true, false, true, true, true, true, true, true, false, true, false, true };
			else if (Data.legs == 12) Data.Coordinate.legs_H = { true, false, true, true, true, true, true, true, true, true, true, true, false, true };
			else if (Data.legs == 14) Data.Coordinate.legs_H = { true, true, true, true, true, true, true, true, true, true, true, true, true, true };
		}
	}
	else if (Data.leading == 15) {
		if (Data.depend == 8) {
			Data.Coordinate.MainBarDepend_H = { true, false, true, false, true, false, true, false, true, false, true, false, true, false, true };
			if (Data.legs == 8) Data.Coordinate.legs_H = { true, false, true, false, true, false, true, false, true, false, true, false, true, false, true };
		}
		else if (Data.depend == 9) {
			Data.Coordinate.MainBarDepend_H = { true, false, true, false, true, false, true, true, true, false, true, false, true, false, true };
			if (Data.legs == 8) Data.Coordinate.legs_H = { true, false, true, false, true, false, true, false, true, false, true, false, true, false, true };
			else if (Data.legs == 9) Data.Coordinate.legs_H = { true, false, true, false, true, false, true, true, true, false, true, false, true, false, true };
		}
		else if (Data.depend == 10) {
			Data.Coordinate.MainBarDepend_H = { true, false, true, false, true, true, true, false, true, true, true, false, true, false, true };
			if (Data.legs == 8) Data.Coordinate.legs_H = { true, false, true, false, true, false, true, false, true, false, true, false, true, false, true };
			else if (Data.legs == 10) Data.Coordinate.legs_H = { true, false, true, false, true, true, true, false, true, true, true, false, true, false, true };
		}
		else if (Data.depend == 11) {
			Data.Coordinate.MainBarDepend_H = { true, false, true, false, true, true, true, true, true, true, true, false, true, false, true };
			if (Data.legs == 8) Data.Coordinate.legs_H = { true, false, true, false, true, false, true, false, true, false, true, false, true, false, true };
			else if (Data.legs == 9) Data.Coordinate.legs_H = { true, false, true, false, true, false, true, true, true, false, true, false, true, false, true };
			else if (Data.legs == 10) Data.Coordinate.legs_H = { true, false, true, false, true, true, true, false, true, true, true, false, true, false, true };
			else if (Data.legs == 11) Data.Coordinate.legs_H = { true, false, true, false, true, true, true, true, true, true, true, false, true, false, true };
		}
		else if (Data.depend == 12) {
			Data.Coordinate.MainBarDepend_H = { true, false, true, true, true, true, true, false, true, true, true, true, true, false, true };
			if (Data.legs == 8) Data.Coordinate.legs_H = { true, false, true, false, true, false, true, false, true, false, true, false, true, false, true };
			else if (Data.legs == 10) Data.Coordinate.legs_H = { true, false, true, false, true, true, true, false, true, true, true, false, true, false, true };
			else if (Data.legs == 12) Data.Coordinate.legs_H = { true, false, true, true, true, true, true, false, true, true, true, true, true, false, true };
		}
		else if (Data.depend == 13) {
			Data.Coordinate.MainBarDepend_H = { true, false, true, true, true, true, true, true, true, true, true, true, true, false, true };
			if (Data.legs == 8) Data.Coordinate.legs_H = { true, false, true, false, true, false, true, false, true, false, true, false, true, false, true };
			else if (Data.legs == 9) Data.Coordinate.legs_H = { true, false, true, false, true, false, true, true, true, false, true, false, true, false, true };
			else if (Data.legs == 10) Data.Coordinate.legs_H = { true, false, true, false, true, true, true, false, true, true, true, false, true, false, true };
			else if (Data.legs == 11) Data.Coordinate.legs_H = { true, false, true, false, true, true, true, true, true, true, true, false, true, false, true };
			else if (Data.legs == 12) Data.Coordinate.legs_H = { true, false, true, true, true, true, true, false, true, true, true, true, true, false, true };
			else if (Data.legs == 13) Data.Coordinate.legs_H = { true, false, true, true, true, true, true, true, true, true, true, true, true, false, true };
		}
		else if (Data.depend == 14) {
			Data.Coordinate.MainBarDepend_H = { true, true, true, true, true, true, true, false, true, true, true, true, true, true, true };
			if (Data.legs == 8) Data.Coordinate.legs_H = { true, false, true, false, true, false, true, false, true, false, true, false, true, false, true };
			else if (Data.legs == 10) Data.Coordinate.legs_H = { true, false, true, false, true, true, true, false, true, true, true, false, true, false, true };
			else if (Data.legs == 12) Data.Coordinate.legs_H = { true, false, true, true, true, true, true, false, true, true, true, true, true, false, true };
			else if (Data.legs == 14) Data.Coordinate.legs_H = { true, true, true, true, true, true, true, false, true, true, true, true, true, true, true };
		}
		else if (Data.depend == 15) {
			Data.Coordinate.MainBarDepend_H = { true, true, true, true, true, true, true, true, true, true, true, true, true, true, true };
			if (Data.legs == 8) Data.Coordinate.legs_H = { true, false, true, false, true, false, true, false, true, false, true, false, true, false, true };
			else if (Data.legs == 9) Data.Coordinate.legs_H = { true, false, true, false, true, false, true, true, true, false, true, false, true, false, true };
			else if (Data.legs == 10) Data.Coordinate.legs_H = { true, false, true, false, true, true, true, false, true, true, true, false, true, false, true };
			else if (Data.legs == 11) Data.Coordinate.legs_H = { true, false, true, false, true, true, true, true, true, true, true, false, true, false, true };
			else if (Data.legs == 12) Data.Coordinate.legs_H = { true, false, true, true, true, true, true, false, true, true, true, true, true, false, true };
			else if (Data.legs == 13) Data.Coordinate.legs_H = { true, false, true, true, true, true, true, true, true, true, true, true, true, false, true };
			else if (Data.legs == 14) Data.Coordinate.legs_H = { true, true, true, true, true, true, true, false, true, true, true, true, true, true, true };
			else if (Data.legs == 15) Data.Coordinate.legs_H = { true, true, true, true, true, true, true, true, true, true, true, true, true, true, true };
		}
	}
}

void Uhook(stringstream& file, double MainBarR, double StirrupOfD, double clearcoverToCenter, double cx1, double cy1, double Offset, double R1, double hook1, double hook2, double hook3, double bw, double i) {
	double k = 0;
	if (i == -1) k = 1;
	file << "<g transform = \"translate(" << cx1 + Offset + k * (bw - 2 * clearcoverToCenter) << "," << cy1 + Offset << ")\" stroke = \"#000\" stroke-width = \"2\">";
	file << "<path d = \"M" << 0 << " " << -R1 << "A" << R1 * i << " " << R1 << " " << 0 << " " << 0 << " " << 1 - k << " " << hook1 * i << " " << -hook1 << "l" << hook2 * i << " " << hook2 << "l" << -hook3 * i << " " << hook3 << "l" << -hook2 * i << " " << -hook2 << "\" fill = \"white\"></path>";
	file << "<path d = \"M" << -sqrt(MainBarR * MainBarR / 2) * i << " " << sqrt(MainBarR * MainBarR / 2) << "l" << hook2 * i << " " << hook2 << "l" << -hook3 * i << " " << hook3 << "l" << (-hook2 + sqrt(StirrupOfD * StirrupOfD / 2)) * i << " " << -hook2 + sqrt(StirrupOfD * StirrupOfD / 2) << "\" fill = \"white\"></path>";
	file << "</g>";
}

void leg2(stringstream& file, double x, double y, double MainBarR, double StirrupOfD, double clearcoverToCenter, double Offset, double centerToCenter, double R1, double hook1, double hook2, double hook3, double h, double i) {

	double k = 1;
	double k1 = 0;
	if (i == -1) k = 0, k1 = 1;

	file << "<g transform = \"translate(" << x << "," << y << ")\" stroke = \"#000\" stroke-width = \"2\">";
	file << "<path d = \"M" << -R1 << " " << i * (-h / 2 + clearcoverToCenter) << "A" << R1 << " " << R1 << " " << 0 << " " << 0 << " " << k << " " << hook1 << " " << i * (-hook1 - h / 2 + clearcoverToCenter) << "l" << hook2 << " " << i * hook2 << "l" << -hook3 << " " << i * hook3 << "l" << -hook2 << " " << i * -hook2;
	file << "A" << MainBarR << " " << MainBarR << " " << 0 << " " << 0 << " " << k1 << " " << -MainBarR << " " << i * (-h / 2 + clearcoverToCenter) << "l" << 0 << " " << i * (h - 2 * clearcoverToCenter) << "A" << MainBarR << " " << MainBarR << " " << 0 << " " << 0 << " " << k1 << " " << 0 << " " << i * (1. / 2. * h - clearcoverToCenter + MainBarR);
	file << "l" << 3 * enlarge << " " << 0 << "l" << 0 << " " << i * StirrupOfD << "l" << -3 * enlarge << " " << 0 << "A" << R1 << " " << R1 << " " << 0 << " " << 0 << " " << k << " " << -R1 << " " << i * (1. / 2. * h - clearcoverToCenter) << "Z";
	file << "\" fill = \"white\"></path>";
	file << "</g>";
}

string Drawing(RcData& Data) {
	// 出圖
	double h = Data.h * enlarge, bw = Data.bw * enlarge, limmitOfH = h + 50., limmitOfB = bw + 50.;
	int MainBarCount = Data.leading;
	double MainBarR = BarDiameter(Data.MainBar) / 2. * enlarge, StirrupOfD = BarDiameter(Data.Stirrup) * enlarge;
	//需運算之值
	double clearCover = Data.cc * enlarge, clearcoverToCenter = clearCover + MainBarR;
	double centerToCenter = (bw - 2 * clearcoverToCenter) / (MainBarCount - 1);
	const auto Offset = 10;
	double s = Data.stirrupSpacing;//接上去的時候要拿掉
	stringstream file;

	file << "<svg width= \" " << limmitOfB << " \" height = \" " << limmitOfH << " \"> \n";
	file << "<rect x = '" << Offset << "' y = '" << Offset << "' width = \" " << bw << " \" height = \" "
		<< h << " \" style = \"fill:rgb(255, 255, 255);stroke-width:2;stroke:rgb(0,0,0)\" /> \n";

	double cx1 = clearcoverToCenter, cy1 = clearcoverToCenter;
	for (int i = 0; i < MainBarCount; i++) {
		if (Data.leader == false && Data.Coordinate.MainBarDepend_H[i] == false) {
			cx1 = cx1 + centerToCenter;
			continue;
		}
		file << "<circle cx = \" " << cx1 + Offset << " \" cy = \" " << cy1 + Offset << " \" r = \" "
			<< MainBarR << " \" stroke = \"black\" stroke-width = \"2\" fill = \"white\" /> \n";
		cx1 = cx1 + centerToCenter;
	}

	double cx2 = clearcoverToCenter, cy2 = h - clearcoverToCenter;
	for (int i = 0; i < MainBarCount; i++) {
		if (Data.leader == true && Data.Coordinate.MainBarDepend_H[i] == false) {
			cx2 = cx2 + centerToCenter;
			continue;
		}
		file << "<circle cx = \" " << cx2 + Offset << " \" cy = \" " << cy2 + Offset << " \" r = \" "
			<< MainBarR << " \" stroke = \"black\" stroke-width = \"2\" fill = \"white\" /> \n";
		cx2 = cx2 + centerToCenter;
	}


	/*新箍筋*/
	for (int i = 0; i <= 1; i++) {
		file << "<rect x = \"" << clearCover + Offset - i * StirrupOfD << " \" y = \"" << clearCover + Offset - i * StirrupOfD << " \" rx = \"" << MainBarR + i * StirrupOfD << "\" ry = \" " << MainBarR + i * StirrupOfD << "\" width = \"" << bw - 2 * clearCover + i * 2 * StirrupOfD << "\" height = \" " << h - 2 * clearCover + i * 2 * StirrupOfD << "\"";
		file << "style = \"fill:none;stroke:black;stroke-width:2;opacity:1\"/>";
	}
	/*箍筋細節*/
	cx1 = clearcoverToCenter;
	cy1 = clearcoverToCenter;
	double R1 = StirrupOfD + MainBarR;
	double hook1 = sqrt(R1 * R1 / 2), hook2 = sqrt(9 * enlarge * enlarge / 2), hook3 = sqrt(StirrupOfD * StirrupOfD / 2);

	Uhook(file, MainBarR, StirrupOfD, clearcoverToCenter, cx1, cy1, Offset, R1, hook1, hook2, hook3, bw, 1.0);//left corner hook 1.0
	Uhook(file, MainBarR, StirrupOfD, clearcoverToCenter, cx1, cy1, Offset, R1, hook1, hook2, hook3, bw, -1.0);//left corner hook -1.0
	/*legs x,y in this place is the center of the mainBar which you want the legs to put on*/

	leg2(file, clearcoverToCenter + Offset + 4 * centerToCenter, h / 2 + Offset, MainBarR, StirrupOfD, clearcoverToCenter, Offset, centerToCenter, R1, hook1, hook2, hook3, h, -1.);
	leg2(file, clearcoverToCenter + Offset + 2 * centerToCenter, h / 2 + Offset, MainBarR, StirrupOfD, clearcoverToCenter, Offset, centerToCenter, R1, hook1, hook2, hook3, h, 1);
	file << "</svg>\n";

	file << "<p>" << "Main reinforcement bar " << MainBarCount << " - N0." << 8 << "</p>";

	//Side view of the beam
	double enlarge2 = 5;
	double Ln = Data.ln * enlarge2;
	h = h / enlarge * enlarge2;
	clearcoverToCenter = clearcoverToCenter / enlarge * enlarge2;

	//這裡還在改
	s *= enlarge2;
	double stirrupCount = floor(Ln / s);
	double Place1st = (Ln - (floor(Ln / s) - 1) * s) / 2;
	bool placing = false;
	if (Place1st > 2 && Place1st < 1) {
		stirrupCount = floor((Ln - 4 * enlarge2) / s);
		Place1st = (Ln - (stirrupCount - 1) * s) / 2;
		placing = true;
	}
	//這裡還在改

	file << "<svg width= \" " << Ln + 50 << " \" height = \" " << h + 50 << " \"> \n";

	//mainbar
	file << "<rect x = '" << Offset << "' y = '" << Offset << "' width = \" " << Ln << " \" height = \" "
		<< h << " \" style = \"fill:rgb(255, 255, 255);stroke-width:2;stroke:rgb(0,0,0)\" /> \n";
	for (int i = 0; i < 2; i++) {
		file << "<line x1=\" " << Offset << "\" y1=\" " << Offset + clearcoverToCenter + i * (h - 2 * clearcoverToCenter) << " \" x2=\" " << Offset + Ln
			<< "\" y2=\" " << Offset + clearcoverToCenter + i * (h - 2 * clearcoverToCenter) << "\" style=\"stroke:rgb(0, 0, 0); stroke - width:2\" />";
	}
	//stirrup 1st one and last one
	if (placing) {
		for (int i = 0; i < 2; i++) {
			file << "<line x1=\" " << Offset + 2 * enlarge2 + i * (Ln - 4 * enlarge2) << "\" y1=\" " << Offset + clearcoverToCenter
				<< " \" x2=\" " << Offset + 2 * enlarge2 + i * (Ln - 4 * enlarge2) << "\" y2=\" " << Offset + h - clearcoverToCenter << "\" style=\"stroke:rgb(0, 0, 0); stroke - width:2\" />";
		}
	}

	//stirrup in the middle 
	for (int i = 0; i < stirrupCount; i++) {
		file << "<line x1=\"" << Offset + Place1st + i * s << "\" y1=\" " << Offset + clearcoverToCenter
			<< " \" x2=\" " << Offset + Place1st + i * s << "\" y2=\"" << Offset + h - clearcoverToCenter << "\" style=\"stroke:rgb(0, 0, 0); stroke - width:2\" />";
	}

	file << "</svg>\n";
	file << "</html>";
	cout << file.str() << endl;
	return file.str();
}