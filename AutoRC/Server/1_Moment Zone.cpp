#include <iostream>
#include <cmath>
#include <vector>
#include <numeric>
#include <algorithm>
#include "AutoRC.h"
using namespace std;

// ACI 318-19 6.3.2.1
double Beff(double bw, double Sw, double hf, double ln, double bf, RcData::_BeamType_& BeamType) {
	//�W��T��
	//if (hf >= 0.5 * bw) {
	//	return min({ bf, 4 * bw });
	//}
	//else {
	//	
	//}

	if (BeamType == RcData::_BeamType_::Side) return bw + min({ Sw / 2.0, 6.0 * hf, ln / 12.0 });
	else return bw + 2 * min({ Sw / 2.0, 8.0 * hf, ln / 12.0 });
}

// ACI 318-19 2.2 �w�p���
double Deff(int MainBar, int TorsionBar, double h, double cc) {
	return h - cc - BarDiameter(TorsionBar) - (BarDiameter(MainBar) / 2);
}

// ACI 318-19 9.6.1.2
// As ���� (a) �M (b) �����j�̡A���D 9.6.1.3 �t���W�w�C ���㦳�����l�t���R�w��Abw �������� bf �M 2bw �����p���@�ӡC fy ����������b�̤j 80,000 psi�C
double MinMainBarQuantity(double bw, double deff, double fc, double fy) {
	if (fy >= 80000)fy = 80000;
	auto a = (3. * sqrt(fc) / fy) * bw * deff;
	auto b = (200. / fy) * bw * deff;
	return max({ a, b });
}

// ACI 318-19 18.6.3.1
// �٪������M�������ܤ֦���ڳs������C �b�����_���A�����M�����������t���q���ܤֲŦX9.6.1.2���n�D�A�t���v�l���60�ſ��������W�L0.025�A���80�ſ��������W�L0.02�C
double MaxMainBarQuantity(double bw, double deff, double ReinforceRatio) {
	return bw * deff * ReinforceRatio;
}

void MainBarCountDesign(RcData& Data) {
	// ACI 318-19 25.2.1
	auto MainBarSpaceMin = min({ 1., BarDiameter(Data.MainBar), 4. / 3. * Data.dagg });
	// ACI 318-19 24.3.2
	auto MainBarSpaceMax = min({ 15. * (40000. / (2. / 3. * Data.MainBarGrade)) - (2.5 * Data.cc), 12. * (40000. / (2. / 3. * Data.MainBarGrade)) });
	// �Q�βb���Z�p��������
	Data.HorizontalMainBarCountMinUpper = ceil((Data.bw - 2. * Data.cc - 2. * BarDiameter(Data.Stirrup) + MainBarSpaceMax) / (BarDiameter(Data.MainBar) + MainBarSpaceMax));
	Data.HorizontalMainBarCountMaxUpper = floor((Data.bw - 2. * Data.cc - 2. * BarDiameter(Data.Stirrup) + MainBarSpaceMin) / (BarDiameter(Data.MainBar) + MainBarSpaceMin));
	Data.HorizontalMainBarCountMinLower = Data.HorizontalMainBarCountMinUpper;
	Data.HorizontalMainBarCountMaxLower = Data.HorizontalMainBarCountMaxUpper;

	// �W�h�������q
	Data.asMinUpper = MinMainBarQuantity(Data.bw, Data.deffUpper, Data.fc, Data.MainBarGrade);
	Data.asMaxUpper = MaxMainBarQuantity(Data.bw, Data.deffUpper, ReinforceRatio(Data.MainBarGrade));
	// �U�h�������q
	Data.asMinLower = MinMainBarQuantity(Data.bw, Data.deffLower, Data.fc, Data.MainBarGrade);
	Data.asMaxLower = MaxMainBarQuantity(Data.bw, Data.deffLower, ReinforceRatio(Data.MainBarGrade));
	// �Q�ο����q�p���`���
	Data.MainBarCountMinUpper = ceil(Data.asMinUpper / BarArea(Data.MainBar));
	Data.MainBarCountMaxUpper = floor(Data.asMaxUpper / BarArea(Data.MainBar));
	Data.MainBarCountMinLower = ceil(Data.asMinLower / BarArea(Data.MainBar));
	Data.MainBarCountMaxLower = floor(Data.asMaxLower / BarArea(Data.MainBar));

	// �Y�βb���Z�Һ�X�Ӫ���Ƥp������q�Һ�X�Ӫ����
	// �h�N�̤p������� = �����q�Һ�X�Ӫ����
	Data.HorizontalMainBarCountMinUpper = max({ Data.HorizontalMainBarCountMinUpper, Data.MainBarCountMinUpper });
	Data.HorizontalMainBarCountMinLower = max({ Data.HorizontalMainBarCountMinLower, Data.MainBarCountMinLower });
}

void SetReinforce(int& Layers, vector <int>& EachLayerCount, int& HorizontalMainBarCountMax, int& MainBarCountMax, int& AllCount, int& Count1, int& Count2, RcData::_Warning_& Warning) {
	if (Layers == 1) {
		EachLayerCount[0] = Count1;
	}
	else {
		for (int UpperLayer = 0; UpperLayer < Layers - 1; UpperLayer++) {
			EachLayerCount.resize(Layers);
			EachLayerCount[UpperLayer] = HorizontalMainBarCountMax;
		}
		EachLayerCount[Layers - 1] = Count2;
	}
	// �ˮֵ������q
	AllCount = accumulate(EachLayerCount.begin(), EachLayerCount.end(), 0);
	if (AllCount > MainBarCountMax) {
		Warning = RcData::_Warning_::InsufficientSectionCrossSectionalDimensionsForAs;
	}
}

// �qC
double Moment(double bwbeff, double h, double fc, double fy, int MainBar, vector <double> PerLayerD, vector <int> PerLayerCount, int TorsionBar, int HorizontalTorsionBarCount, bool Ei, vector <double>& epsilon, double& CompressiveStressZone) {
	double UpperBound = 0., LowerBound = h;
	double EpsilonY = fy / Ec;
	double Beta = 0.85 - 0.05 * ((fc - 4000.) / 1000.);
	CompressiveStressZone = h / 2.;
	double TotalForce;
	epsilon.resize(PerLayerCount.size());
	vector <double> Fy(PerLayerCount.size());
	// ����qC
	do {
		TotalForce = 0.85 * fc * Beta * CompressiveStressZone * bwbeff;
		for (int i = 0; i < PerLayerCount.size(); i++) {
			epsilon[i] = 0.003 - 0.003 / CompressiveStressZone * PerLayerD[i];
			if (abs(epsilon[i]) >= EpsilonY) {
				if (epsilon[i] >= 0) {
					if (Ei == true) Fy[i] = 1.25 * fy;
					else Fy[i] = fy;
				}
				else {
					if (Ei == true) Fy[i] = -1.25 * fy;
					else Fy[i] = -fy;
				}
			}
			else Fy[i] = epsilon[i] * Ec;
			if (i == 0 || i == PerLayerCount.size() - 1) TotalForce += (PerLayerCount[i] * BarArea(MainBar) - HorizontalTorsionBarCount * BarArea(TorsionBar)) * Fy[i];
			else TotalForce += PerLayerCount[i] * BarArea(MainBar) * Fy[i];
		}
		if (TotalForce > 0) LowerBound = CompressiveStressZone;
		else UpperBound = CompressiveStressZone;
		CompressiveStressZone = (UpperBound + LowerBound) / 2.;
	} while (abs(TotalForce) > 1E-8);
	// �p�⼸���]�p��
	double Moment = 0.85 * fc * pow(Beta * CompressiveStressZone, 2) * bwbeff / 2;
	for (int i = 0; i < PerLayerCount.size(); i++) {
		if (i == 0 || i == PerLayerCount.size() - 1) Moment += (PerLayerCount[i] * BarArea(MainBar) - HorizontalTorsionBarCount * BarArea(TorsionBar)) * Fy[i] * PerLayerD[i];
		else Moment += PerLayerCount[i] * BarArea(MainBar) * Fy[i] * PerLayerD[i];
	}
	return -Moment;
}

// �]�p�������Y�� �@���T��
double ReductionFactorMforNomal(double fy, vector <double> epsilon) {
	if (abs(epsilon[epsilon.size() - 1]) <= fy / Ec) return 0.65;
	else if (abs(epsilon[epsilon.size() - 1]) >= fy / Ec + 0.003) return 0.9;
	else return 0.65 + (abs(epsilon[epsilon.size() - 1]) - (3. / 1450.)) * (0.25 / 0.003);
}

// �]�p�������Y�� �����T��
double ReductionFactorMforSpiral(double fy, vector <double> epsilon) {
	if (abs(epsilon[epsilon.size() - 1]) <= fy / Ec) return 0.75;
	else if (abs(epsilon[epsilon.size() - 1]) >= fy / Ec + 0.003) return 0.9;
	else return 0.75 + (abs(epsilon[epsilon.size() - 1]) - (3. / 1450.)) * (0.15 / 0.003);
}

void Reinforcement(RcData& Data) {
	// �إߨC�h��������x�s�Ŷ�
	Data.UpperEachLayerCount.resize(Data.UpperLayers);
	Data.LowerEachLayerCount.resize(Data.LowerLayers);
	int UpperCount1 = Data.HorizontalMainBarCountMinUpper;
	int LowerCount1 = Data.HorizontalMainBarCountMinLower;
	int UpperCount2 = 0;
	int LowerCount2 = 0;
	do {
		// �W�h��
		SetReinforce(Data.UpperLayers, Data.UpperEachLayerCount, Data.HorizontalMainBarCountMaxUpper, Data.MainBarCountMaxUpper, Data.AllUpperCount, UpperCount1, UpperCount2, Data.Warning);
		// �U�h��
		SetReinforce(Data.LowerLayers, Data.LowerEachLayerCount, Data.HorizontalMainBarCountMaxLower, Data.MainBarCountMaxLower, Data.AllLowerCount, LowerCount1, LowerCount2, Data.Warning);
		// �N�C�h����ƦX�֬��@�ӦV�q
		Data.Mn2_PerLayerCount = Data.UpperEachLayerCount;
		Data.Mn2_PerLayerCount.insert(Data.Mn2_PerLayerCount.end(), Data.LowerEachLayerCount.rbegin(), Data.LowerEachLayerCount.rend());
		// �p��C�h���Ĳ`��
		// ACI 318-19 25.2.2 �����̤p�b���Z�� 1 in�C
		Data.PerLayerDUpper.resize(Data.UpperLayers);
		Data.PerLayerDLower.resize(Data.LowerLayers);
		Data.PerLayerDUpper[0] = Data.cc + BarDiameter(Data.Stirrup) + (BarDiameter(Data.MainBar) / 2.);
		Data.PerLayerDLower[0] = Data.h - Data.cc - BarDiameter(Data.Stirrup) - (BarDiameter(Data.MainBar) / 2.);
		for (int layer = 1; layer <= Data.UpperLayers - 1; layer++) {
			Data.PerLayerDUpper[layer] = Data.PerLayerDUpper[layer - 1] + BarDiameter(Data.MainBar) + 1;
		}
		for (int layer = 1; layer <= Data.LowerLayers - 1; layer++) {
			Data.PerLayerDLower[layer] = Data.PerLayerDLower[layer - 1] - BarDiameter(Data.MainBar) - 1;
		}
		// �X�֤W�U�h���Ĳ`��
		Data.PerLayerDMn2 = Data.PerLayerDUpper;
		Data.PerLayerDMn2.insert(Data.PerLayerDMn2.end(), Data.PerLayerDLower.rbegin(), Data.PerLayerDLower.rend());
		Data.PerLayerDMn1 = Data.PerLayerDMn2;
		reverse(Data.PerLayerDMn1.begin(), Data.PerLayerDMn1.end());
		// �إ������x�s�Ŷ�
		Data.Mn1_epsilon.resize(Data.Mn2_PerLayerCount.size());
		Data.Mn1_PerLayerCount.resize(Data.Mn2_PerLayerCount.size());
		for (int i = 0; i < Data.PerLayerDMn1.size(); i++) {
			Data.PerLayerDMn1[i] = Data.h - Data.PerLayerDMn1[i];
		}
		Data.Mn1_PerLayerCount.resize(Data.Mn2_PerLayerCount.size());
		Data.Mn1_PerLayerCount = Data.Mn2_PerLayerCount;
		reverse(Data.Mn1_PerLayerCount.begin(), Data.Mn1_PerLayerCount.end());
		Data.Mn2_epsilon.resize(Data.Mn2_PerLayerCount.size());

		Data.Mn1 = Moment(Data.bw, Data.h, Data.fc, Data.MainBarGrade, Data.MainBar, Data.PerLayerDMn1, Data.Mn1_PerLayerCount, Data.TorsionBar, Data.HorizontalTorsionBarCount, false, Data.Mn1_epsilon, Data.Mn1_CompressiveStressZone);
		Data.Mn2 = -Moment(Data.beff, Data.h, Data.fc, Data.MainBarGrade, Data.MainBar, Data.PerLayerDMn2, Data.Mn2_PerLayerCount, Data.TorsionBar, Data.HorizontalTorsionBarCount, false, Data.Mn2_epsilon, Data.Mn2_CompressiveStressZone);
		// �p�⼸�����Y��
		if (Data.isSpiralBar == true) {
			Data.ReductionFactors.ForMoment_1 = ReductionFactorMforSpiral(Data.MainBarGrade, Data.Mn1_epsilon);
			Data.ReductionFactors.ForMoment_2 = ReductionFactorMforSpiral(Data.MainBarGrade, Data.Mn2_epsilon);
		}
		else {
			Data.ReductionFactors.ForMoment_1 = ReductionFactorMforNomal(Data.MainBarGrade, Data.Mn1_epsilon);
			Data.ReductionFactors.ForMoment_2 = ReductionFactorMforNomal(Data.MainBarGrade, Data.Mn2_epsilon);
		}
		Data.reductionMn1 = Data.ReductionFactors.ForMoment_1 * Data.Mn1;
		Data.reductionMn2 = Data.ReductionFactors.ForMoment_2 * Data.Mn2;
		// ���X�j��
		if (Data.reductionMn1 >= Data.Mu1 && Data.reductionMn2 <= Data.Mu2) {
			if (Data.UpperEachLayerCount[0] >= Data.LowerEachLayerCount[0]) {
				Data.legsMin = LegsMin(Data.UpperEachLayerCount[0]);
				Data.leading = Data.UpperEachLayerCount[0];
				Data.depend = Data.LowerEachLayerCount[0];
				if (Data.UpperEachLayerCount[0] % 2 != 0) {
					if (Data.LowerEachLayerCount[0] >= Data.legsMin) {
						Data.legsMax = Data.LowerEachLayerCount[0];
						Data.OddAllowed = true;
						break;
					}
					else LowerCount1 = Data.legsMin;
				}
				else {
					if (Data.LowerEachLayerCount[0] >= Data.legsMin) {
						if (Data.LowerEachLayerCount[0] % 2 == 0) {
							Data.legsMax = Data.LowerEachLayerCount[0];
							Data.OddAllowed = false;
							break;
						}
						else LowerCount1 += 1;
					}
					else LowerCount1 = Data.legsMin;
				}
			}
			else {
				Data.legsMin = LegsMin(Data.LowerEachLayerCount[0]);
				Data.leading = Data.LowerEachLayerCount[0];
				Data.depend = Data.UpperEachLayerCount[0];
				if (Data.LowerEachLayerCount[0] % 2 != 0) {
					if (Data.UpperEachLayerCount[0] >= Data.legsMin) {
						Data.legsMax = Data.UpperEachLayerCount[0];
						Data.OddAllowed = true;
						break;
					}
					else UpperCount1 = Data.legsMin;
				}
				else {
					if (Data.UpperEachLayerCount[0] >= Data.legsMin) {
						if (Data.UpperEachLayerCount[0] % 2 == 0) {
							Data.legsMax = Data.UpperEachLayerCount[0];
							Data.OddAllowed = false;
							break;
						}
						else UpperCount1 += 1;
					}
					else UpperCount1 = Data.legsMin;
				}
			}
		}
		// �[��ư�
		if (Data.reductionMn1 < Data.Mu1) {
			if (Data.UpperLayers == 1 && UpperCount1 < Data.HorizontalMainBarCountMaxUpper) {
				UpperCount1 += 1;
			}
			else if ((Data.UpperLayers == 1 && UpperCount1 == Data.HorizontalMainBarCountMaxUpper)
				|| (Data.UpperLayers > 1 && Data.HorizontalMainBarCountMaxUpper == UpperCount2)) {
				Data.UpperLayers += 1; UpperCount2 = 2;
			}
			else if (Data.UpperLayers > 1 && Data.HorizontalMainBarCountMaxUpper - UpperCount2 > 1) {
				UpperCount2 += 2;
			}
			else if (Data.UpperLayers > 1 && Data.HorizontalMainBarCountMaxUpper - UpperCount2 == 1) {
				UpperCount2 += 1;
			}
		}
		if (Data.reductionMn2 > Data.Mu2) {
			if (Data.LowerLayers == 1 && LowerCount1 < Data.HorizontalMainBarCountMaxLower) {
				LowerCount1 += 1;
			}
			else if ((Data.LowerLayers == 1 && LowerCount1 == Data.HorizontalMainBarCountMaxLower)
				|| (Data.LowerLayers > 1 && Data.HorizontalMainBarCountMaxLower == LowerCount2)) {
				Data.LowerLayers += 1; LowerCount2 = 2;
			}
			else if (Data.LowerLayers > 1 && Data.HorizontalMainBarCountMaxLower - LowerCount2 > 1) {
				LowerCount2 += 2;
			}
			else if (Data.LowerLayers > 1 && Data.HorizontalMainBarCountMaxLower - LowerCount2 == 1) {
				LowerCount2 += 1;
			}
		}
	} while (true);
}

// �����D���̤p���Z
// 25.2.1 �����̤p�b���Z�� 1 in�B 1 db �M (4 / 3) dagg �����̤j�ȡC
// 25.2.2 �����̤p�b���Z�� 1 in�C