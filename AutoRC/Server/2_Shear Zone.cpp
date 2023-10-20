#include <iostream>
#include <numeric>
#include <algorithm>
#include <cmath>
#include "AutoRC.h"
using namespace std;

// ACI 318-19 22.5.5.1
double Vc(double bw, double fc, double deff) {
	return 2 * sqrt(fc) * bw * deff;
}

// ACI 318-19 22.5.1.2
bool CrossSectionalDimensionsConfirmationForShear(double VuVe, double fc, double vc, double bw, double deff, double ForShear) {
	return VuVe <= ForShear * (vc + (8.0 * sqrt(fc) * bw * deff));
}

// 計算最小腳支數
int LegsMin(int Count) {
	if (Count == 2) return 2;
	else if (Count == 3)return 2;
	else if (Count % 4 == 0)return Count / 2 + 2;
	else return Count / 2 + 1;
}

// ACI 318-19 
double Av_s(double vuve, double Fyt, double deff, double ForShear) {
	return vuve / Fyt / deff / ForShear;
}

// ACI 318-19
double Vs(double vuve, double vc, bool isEarthquakeArea, double ForShear) {
	if (isEarthquakeArea == true) return vuve / ForShear;
	else return vuve / ForShear - vc;
}

// 
void Ve(RcData& Data) {
	Data.Mpr1_epsilon = Data.Mn1_epsilon;
	Data.Mpr2_epsilon = Data.Mn2_epsilon;
	Data.Mpr1 = Moment(Data.bw, Data.h, Data.fc, Data.MainBarGrade, Data.MainBar, Data.PerLayerDMn1, Data.Mn1_PerLayerCount, Data.TorsionBar, Data.HorizontalTorsionBarCount, true, Data.Mpr1_epsilon, Data.Mn1_CompressiveStressZone);
	Data.Mpr2 = -Moment(Data.beff, Data.h, Data.fc, Data.MainBarGrade, Data.MainBar, Data.PerLayerDMn2, Data.Mn2_PerLayerCount, Data.TorsionBar, Data.HorizontalTorsionBarCount, true, Data.Mpr2_epsilon, Data.Mpr2_CompressiveStressZone);
	Data.Ve = max({ abs((Data.Mpr1 - Data.Mpr2) / Data.ln + (Data.LoadFactorDL * Data.Dl + Data.LoadFactorLL * Data.Ll) * Data.ln / 2.), abs((Data.Mpr1 + Data.Mpr2) / Data.ln - (Data.LoadFactorDL * Data.Dl + Data.LoadFactorLL * Data.Ll) * Data.ln / 2.) });
}

double AlongTheLength(double bw, double h, double deff, double fc, double vs, int MainBar, int Stirrup, double MainBarGrade, bool isEarthquakeArea) {
	if (isEarthquakeArea == true) {
		if (MainBarGrade == 80000.) return min({ deff / 4., 48. * BarDiameter(Stirrup), 5. * BarDiameter(MainBar), 6. });
		else if (MainBarGrade == 60000.) return min({ deff / 4., 48. * BarDiameter(Stirrup), 6. * BarDiameter(MainBar), 6. });
	}
	else {
		if (vs <= 4 * sqrt(fc) * bw * deff) return min({ deff / 2., 24., bw, h, 16. * BarDiameter(MainBar), 48. * BarDiameter(Stirrup) });
		else min({ deff / 4., 24., bw, h, 16. * BarDiameter(MainBar), 48. * BarDiameter(Stirrup) });
	}
}

double AcrossTheWidth(double bw, double fc, double Vs, double d, int Stirrup, bool Ti) {
	if (Ti == true) {
		return min({ d / 2., 12., 48. * BarDiameter(Stirrup) });
	}
	else {
		if (Vs <= 4 * sqrt(fc) * bw * d) {
			return min({ d, 24. });
		}
		else {
			return min({ d / 2., 12. });
		}
	}
}

bool StirrupDesign(RcData& Data) {
	if (Data.isEarthquakeArea == true) {
		Ve(Data);
		Data.av_s = Av_s(Data.Ve, Data.StirrupGrade, Data.deff, Data.ReductionFactors.ForShear);
		Data.vsMin = Vs(Data.Ve, Data.vc, Data.isEarthquakeArea, Data.ReductionFactors.ForShear);
	}
	else {
		Data.av_s = Av_s(Data.Vu, Data.StirrupGrade, Data.deff, Data.ReductionFactors.ForShear);
		Data.vsMin = Vs(Data.Vu, Data.vc, Data.isEarthquakeArea, Data.ReductionFactors.ForShear);
	}
	
	//Check the maxS [ACI 318-19 18.6.4.4]
	Data.stirrupSpacingMax = AlongTheLength(Data.bw, Data.h, Data.deff, Data.fc, Data.vsMin, Data.MainBar, Data.Stirrup, Data.MainBarGrade, Data.isEarthquakeArea);

	if (Data.stirrupSpacing > Data.stirrupSpacingMax) {
		//warn user the program have change the space of stirrup
		Data.stirrupSpacing = Data.stirrupSpacingMax;
		Data.Warning = RcData::_Warning_::StirrupSpacingMoreThenStirrupSpacingMax;
	}

	//Find the min legs
	if (Data.OddAllowed == false) Data.legsMin += Data.legsMin % 2;
	Data.legs = Data.legsMin;

	
	//Check the need for torsion.
	if (Data.stirrupSpacing > BarArea(Data.Stirrup) / (Data.at_s)) {
		//Warn! Reduce s or increase stirrupArea
		Data.Warning = RcData::_Warning_::At_sInsufficient;		// HTML需要告訴使用者
		Data.stirrupSpacingSuggestion = BarArea(Data.Stirrup) / (Data.at_s);
		return false;
	}

	//Check the need for shear.
	for (;;) {
		if (Data.stirrupSpacing <= (BarArea(Data.Stirrup) * Data.legs) / (2 * Data.at_s + Data.av_s))break;
		if (Data.leading % 2 == 1)Data.legs++;
		else Data.legs += 2;

		if (Data.legs > Data.legsMax) {
			cout << "legs is max but, still can't design it" << endl;
			return false;
		}
	}

	//Check min transverse reinforcement ACI 318-19 9.6.3
	double MinAv_s = max(Data.ReductionFactors.ForShear * sqrt(Data.fc) * Data.bw / Data.StirrupGrade, 50. * Data.bw / Data.StirrupGrade);
	if (BarArea(Data.Stirrup) * Data.legs / Data.stirrupSpacing - Data.at_s < MinAv_s) {
		//Warn! Reduce s or increase stirrupArea
		Data.Warning = RcData::_Warning_::Av_sInsufficient;		// HTML需要告訴使用者
		return false;
	}

}