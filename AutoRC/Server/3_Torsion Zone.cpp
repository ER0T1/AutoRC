#include <iostream>
#include <cmath>
#include "AutoRC.h"
using namespace std;

// ACI 318-19 9.2.4.4
double Acp(double bw, double h, double hf, double flanges, RcData::_BeamType_& BeamType) {
	if (BeamType == RcData::_BeamType_::Side) {
		return bw * h + flanges * hf;
	}
	else {
		return bw * h + flanges * hf * 2.0;
	}
}

// ACI 318-19 9.2.4.4
double Pcp(double bw, double h, double hf, double flanges, RcData::_BeamType_& BeamType) {
	if (BeamType == RcData::_BeamType_::Side) {
		return 2.0 * (bw + flanges + h);
	}
	else {
		return 2.0 * (bw + flanges * 2.0 + h);
	}
}

// ACI 318-19 2.2
double Ph(double bw, double h, double cc, double Stirrup) {
	return 2.0 * ((bw - 2.0 * cc - BarDiameter(Stirrup) + (h - 2.0 * cc - BarDiameter(Stirrup))));
}

// ACI 318-19 2.2
double Aoh(double bw, double h, double cc, int Stirrup) {
	return (bw - 2.0 * cc - BarDiameter(Stirrup)) * (h - 2.0 * cc - BarDiameter(Stirrup));
}

// ACI 318-19 2.2
double Ao(double aoh) {
	return 0.85 * aoh;
}

// ACI 318-19 22.7.4.1a
double Tth(double fc, double acp, double pcp) {
	return sqrt(fc)* (pow(acp, 2) / pcp);
}

// ACI 318-19 22.7.1.1
bool TorsionalSectionalStrength(double Tu, double tth, double ForTorsion) {
	return Tu >= ForTorsion * tth;
}

// ACI 318-19 22.7.7.1a
bool CrossSectionalDimensionsConfirmationForTorsion(double Vu, double Tu, double fc, double vc, double bw, double deff, double ph, double aoh, double ForTorsion) {
	// 目前只有soild，尚未建立hollow
	auto TestValue_1 = sqrt(pow(Vu / (bw * deff), 2) + pow((Tu * ph / (1.7 * pow(aoh, 2))), 2));
	auto TestValue_2 = ForTorsion * (vc / (bw * deff) + (8 * sqrt(fc)));
	return TestValue_1 <= TestValue_2;
}

// ACI 318-19 22.7.6.1a 反算
double At_s(double Tu, double fyt, double ao, double ForTorsion) {
	return Tu / ForTorsion / 2. / ao / fyt * tan(PI / 4.);
}

// 扭矩主筋設計量
double Al(double Tu, double fc, double fy, double fyt, double bw, double ph, double acp, double ao, double at_s, double ForTorsion) {
	auto al_1 = (5. * sqrt(fc) * acp / fy) - (at_s * ph * fyt / fy); // ACI 318-19 9.6.4.3a
	auto al_2 = (5. * sqrt(fc) * acp / fy) - (25.0 * bw / fyt) * ph * (fyt / fy); // ACI 318-19 9.6.4.3b
	auto al = min({ al_1, al_2 }); // ACI 318-19 9.6.4.3
	auto check = Tu * ph / ForTorsion / 2. / ao / fy; // ACI 318-19 22.7.6.1b 反算
	return max({ al, check });
}

// ACI 318-19 9.7.6.3.3 橫向扭轉鋼筋的間距不應超過 ph/8 和 12 英吋中的較小者。
// ACI 318-19 9.7.6.4.3 橫向鋼筋的間距不得超過(a)至(c)中的最小值：
//						(a) 16db 縱向鋼筋
//						(b) 48db 橫向鋼筋
//						(c) 樑的最小尺寸
double TorsionBarMaxSpacing(double bw, double h, double ph, int Stirrup, int TorsionBar) {
	return min({ ph / 8., 12., 16 * BarDiameter(TorsionBar), 48 * BarDiameter(Stirrup),bw , h });
}

void TorsionBarCountDesign(RcData& Data) {
	// 從量算需要幾支鋼筋
	auto al = Al(Data.Tu, Data.fc, Data.TorsionBarGrade, Data.StirrupGrade, Data.bw, Data.ph, Data.acp, Data.ao, Data.at_s, Data.ReductionFactors.ForTorsion);
	auto Count = int(ceil(al / BarArea(Data.TorsionBar))) + int(ceil(al / BarArea(Data.TorsionBar))) % 2;
	// 從間距算需要幾支鋼筋
	auto Spacing = TorsionBarMaxSpacing(Data.bw, Data.h, Data.ph, Data.Stirrup, Data.TorsionBar);
	auto Horizontal = int(ceil((Data.bw - 2. * Data.cc - 2. * BarDiameter(Data.Stirrup) + Spacing) / (BarDiameter(Data.TorsionBar) + Spacing)));
	auto Vertical = int(ceil((Data.h - 2. * Data.cc - 2. * BarDiameter(Data.Stirrup) + Spacing) / (BarDiameter(Data.TorsionBar) + Spacing)));
	// 綜合計算
	if (Count > 2 * (Horizontal + Vertical) - 4) {
		Horizontal += int(ceil((Count - (2 * (Horizontal + Vertical) - 4)) / 2.));
		Vertical += int(ceil((Count - (2 * (Horizontal + Vertical) - 4)) / 2.));
	}
	Data.TorsionBarCount = 2 * (Horizontal + Vertical) - 4;
	Data.HorizontalTorsionBarCount = Horizontal;
	Data.VerticalTorsionBarCount = Vertical;
	Data.al = Data.TorsionBarCount * BarArea(Data.TorsionBar);
}

// ACI 318-19 22.7.6.1
double TorsionBarDesignStrength(double fy, double fyt, double ao, double ph, double at_s, int al) {
	auto a = 2 * ao * at_s * fyt / tan(PI / 4.);
	auto b = 2 * ao * al * fy / ph * tan(PI / 4.);
	return min({ a, b });
}

double ReductionTn(double Tn, double ForTorsion) {
	return Tn * ForTorsion;
}