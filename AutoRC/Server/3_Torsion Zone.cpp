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
	// �ثe�u��soild�A�|���إ�hollow
	auto TestValue_1 = sqrt(pow(Vu / (bw * deff), 2) + pow((Tu * ph / (1.7 * pow(aoh, 2))), 2));
	auto TestValue_2 = ForTorsion * (vc / (bw * deff) + (8 * sqrt(fc)));
	return TestValue_1 <= TestValue_2;
}

// ACI 318-19 22.7.6.1a �Ϻ�
double At_s(double Tu, double fyt, double ao, double ForTorsion) {
	return Tu / ForTorsion / 2. / ao / fyt * tan(PI / 4.);
}

// ��x�D���]�p�q
double Al(double Tu, double fc, double fy, double fyt, double bw, double ph, double acp, double ao, double at_s, double ForTorsion) {
	auto al_1 = (5. * sqrt(fc) * acp / fy) - (at_s * ph * fyt / fy); // ACI 318-19 9.6.4.3a
	auto al_2 = (5. * sqrt(fc) * acp / fy) - (25.0 * bw / fyt) * ph * (fyt / fy); // ACI 318-19 9.6.4.3b
	auto al = min({ al_1, al_2 }); // ACI 318-19 9.6.4.3
	auto check = Tu * ph / ForTorsion / 2. / ao / fy; // ACI 318-19 22.7.6.1b �Ϻ�
	return max({ al, check });
}

// ACI 318-19 9.7.6.3.3 ��V������������Z�����W�L ph/8 �M 12 �^�T�������p�̡C
// ACI 318-19 9.7.6.4.3 ��V���������Z���o�W�L(a)��(c)�����̤p�ȡG
//						(a) 16db �a�V����
//						(b) 48db ��V����
//						(c) �٪��̤p�ؤo
double TorsionBarMaxSpacing(double bw, double h, double ph, int Stirrup, int TorsionBar) {
	return min({ ph / 8., 12., 16 * BarDiameter(TorsionBar), 48 * BarDiameter(Stirrup),bw , h });
}

void TorsionBarCountDesign(RcData& Data) {
	// �q�q��ݭn�X�����
	auto al = Al(Data.Tu, Data.fc, Data.TorsionBarGrade, Data.StirrupGrade, Data.bw, Data.ph, Data.acp, Data.ao, Data.at_s, Data.ReductionFactors.ForTorsion);
	auto Count = int(ceil(al / BarArea(Data.TorsionBar))) + int(ceil(al / BarArea(Data.TorsionBar))) % 2;
	// �q���Z��ݭn�X�����
	auto Spacing = TorsionBarMaxSpacing(Data.bw, Data.h, Data.ph, Data.Stirrup, Data.TorsionBar);
	auto Horizontal = int(ceil((Data.bw - 2. * Data.cc - 2. * BarDiameter(Data.Stirrup) + Spacing) / (BarDiameter(Data.TorsionBar) + Spacing)));
	auto Vertical = int(ceil((Data.h - 2. * Data.cc - 2. * BarDiameter(Data.Stirrup) + Spacing) / (BarDiameter(Data.TorsionBar) + Spacing)));
	// ��X�p��
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