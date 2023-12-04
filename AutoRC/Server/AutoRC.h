#pragma once
#include <iostream>
#include <vector>
#include "crow.h"
using namespace std;

const auto PI = 3.14159265358979;
const auto kft2lbin = 12000.;
const auto lbin2kft = 1. / 12. * 1E-3;
const auto kips2lb = 1000.;
const auto lb2kips = 1E-3;
const auto ft2in = 12.;
const auto in2ft = 1. / 12.;
const auto Ec = 29000000.;
const auto enlarge = 15.;

struct RcData {
	double Mu1;							// �e�\�j��: ���� lb-in		Allowable Strength: Bending Moment lb-in
	double Mu2;							// �e�\�j��: ���� lb-in		Allowable Strength: Bending Moment lb-in
	double Vu1;							// �e�\�j��: �ŤO lb		Allowable Strength: Shear Force lb
	double Vu2;							// �e�\�j��: �ŤO lb		Allowable Strength: Shear Force lb
	double Vu;							// �e�\�j��: �ŤO lb		Allowable Strength: Shear Force lb
	double Ve;							// �e�\�j��: �ŤO lb		Allowable Strength: Shear Force lb
	double Tu1;							// �e�\�j��: ��x lb-in		
	double Tu2;							// �e�\�j��: ��x lb-in
	double Tu;							// �e�\�j��: ��x lb-in

	double Sw;							// �ؤo��T: ���e in
	double bw;							// �ؤo��T: ��e in
	double beff;						// �ؤo��T: �����l�O�e in
	double bf = 84;						// �ؤo��T: T���l�O�e in
	double h;							// �ؤo��T: ��` in
	double hf;							// �ؤo��T: ���p in
	double ln;							// �ؤo��T: ��� in
	double deffUpper, deffLower, deff;	// �ؤo��T: ���Ĳ`�� in

	// �]�p�j��: �V���g�����j�� psi
	double fc;
	// �]�p�j��: �V���g�ܰűj�� lb
	double vc;
	// �]�p�j��: ��x lb-in
	double Tn, reductionTn;
	// �]�p�j��: ���� lb-in
	double Mn1, Mn2, reductionMn1, reductionMn2;
	// Mpr ���� lb-in
	double Mpr1, Mpr2;

	// ���Ƹ�T: ��������
	int MainBar, Stirrup, TorsionBar, SkinBar = 3;
	// ���Ƹ�T: ��������j�� psi
	double MainBarGrade, StirrupGrade, TorsionBarGrade, SkinBarGrade = 60000.;
	// ���Ƹ�T: �O�_�ϥΥֽ�����
	bool HasSkinBar = false;
	// ���Ƹ�T: �O�_�ϥ����ۿ���
	bool isSpiralBar = false;

	// ��x�D���ݨD�q in^2
	double al;
	// ���Ƹ�T: ��x�D�����
	int TorsionBarCount, HorizontalTorsionBarCount, VerticalTorsionBarCount;

	// ���������ݨD�q in^2
	double asMinUpper, asMaxUpper, asMinLower, asMaxLower;
	// ���Ƹ�T: �����D���h��
	int UpperLayers = 1, LowerLayers = 1;
	// ���Ƹ�T: �����D���`���
	int AllUpperCount, AllLowerCount;
	// ���Ƹ�T: �����D���C�h�������
	vector <int> UpperEachLayerCount, LowerEachLayerCount, Mn1_PerLayerCount, Mn2_PerLayerCount;
	// ���Ƹ�T: �����D�������`��ƭ���
	int MainBarCountMinUpper, MainBarCountMaxUpper, MainBarCountMinLower, MainBarCountMaxLower;
	// ���Ƹ�T: �����D�������C�h�`��ƭ���
	int HorizontalMainBarCountMinUpper, HorizontalMainBarCountMaxUpper, HorizontalMainBarCountMinLower, HorizontalMainBarCountMaxLower;
	// ���Ƹ�T: �����D�������C�h�`��
	vector <double> PerLayerDUpper, PerLayerDLower, PerLayerDMn1, PerLayerDMn2;
	// ���Ƹ�T: �����D�������C�h����
	vector <double> Mn1_epsilon, Mn2_epsilon, Mpr1_epsilon, Mpr2_epsilon;
	// ���Ƹ�T: �������O�ϲ`��
	double Mn1_CompressiveStressZone, Mn2_CompressiveStressZone, Mpr1_CompressiveStressZone, Mpr2_CompressiveStressZone;

	// ��x�T���ݨD�q in^2/in
	double at_s;
	// �ŤO�T���ݨD�q in^2/in
	double av_s;
	// ���Ƹ�T: �}��ƻP����
	int legs, legsMin, legsMax; bool OddAllowed;
	// �T�����Z: in
	double stirrupSpacing, stirrupSpacingMax, stirrupSpacingSuggestion;
	// �]�p�j��: �T���ܰűj�� lb
	double vsMin;
	// �W�U�h�����@�h�D��
	int leading, depend;
	bool leader; // true �W�h��, false �U�h��

	// ���Ƹ�T: �����O�@�h�p��
	double cc;
	// ���Ƹ�T: �����ɮ|�j�p in
	double dagg = 3. / 4.;
	// �����զX: �R���� psi/ft^3, ������ psi/ft^3, �R�����]�l, �������]�l
	double Dl, Ll, LoadFactorDL = 1.2, LoadFactorLL = 1.;
	// ������: ���/������
	enum class _BeamType_ { Middle, Side }BeamType;
	// �c�[����: SDC A, B, C, D, E, F
	enum class _SDC_ { A, B, C, D, E, F }SDC;
	// ĵ�i��T:
	enum class _Warning_ { ProblemNotFound, // �������~
		InsufficientSectionCrossSectionalDimensionsForBw,		// �_���ؤo����: ��e
		InsufficientSectionCrossSectionalDimensionsForTorsion,  // �_���ؤo����: ��x
		InsufficientSectionCrossSectionalDimensionsForShear,	// �_���ؤo����: �ŤO
		InsufficientSectionCrossSectionalDimensionsForAs,		// �_���ؤo����: ����
		StirrupSpacingMoreThenStirrupSpacingMax,				// �ϥΪ��T�����Z�L�j
		Av_sInsufficient,										// �ŤO�T������
		At_sInsufficient										// ��x�T������
	}Warning = _Warning_::ProblemNotFound;

	// �Ѽ�
	// T���l�O�� in
	double flanges;
	// �V���g���_���~�P�򦨪����n in^2
	double acp;
	// �V���g���_���~�P�򦨪��P�� in
	double pcp;
	// �̥~���ʳ���V����������߽u�g�� in
	double ph;
	// �̥~���ʳ���V����������߽u�򦨪����n in^2
	double aoh;
	// ��Ŭy���]���`���n in^2
	double ao;
	// �{�ɧ�x in-lb
	double tth;
	

	// ACI 318-19 21.2.1
	struct {
		double ForTorsion = 0.75;
		double ForMoment_1, ForMoment_2;
		double ForShear = 0.75;
	}ReductionFactors;

	// �O�_���j�_��
	bool isEarthquakeArea;

	struct {
		vector <double> MainBar_H;
		int TorsionBar_H;
		vector <bool> legs_H;
		vector <bool> MainBarDepend_H;
		vector <double> MainBar_V, TorsionBar_V;
	}Coordinate;

	// ��촫���, ������
	// string ForceMultLength{}; string Force{}; string Length{};
};

// Main Zone
void Core(RcData& Data);
crow::json::wvalue Struct2Json(RcData& Data);

// Moment Zone

double Beff(double bw, double Sw, double hf, double ln, double bf, RcData::_BeamType_& BeamType);
double Deff(int MainBar, int TorsionBar, double h, double cc);
double MinMainBarQuantity(double bw, double deff, double fc, double fy);
double MaxMainBarQuantity(double bw, double deff, double ReinforceRatio);
void MainBarCountDesign(RcData& Data);
void SetReinforce(int& Layers, vector <int>& EachLayerCount, int& HorizontalMainBarCountMax, int& MainBarCountMax, int& AllCount, int& Count1, int& Count2, RcData::_Warning_& Warning);
double Moment(double bwbeff, double h, double fc, double fy, int MainBar, vector <double> PerLayerD, vector <int> PerLayerCount, int TorsionBar, int HorizontalTorsionBarCount, bool Ei, vector <double>& epsilon, double& CompressiveStressZone);
void Reinforcement(RcData& Data);
double ReductionFactorMforNomal(double fy, vector <double> epsilon);
double ReductionFactorMforSpiral(double fy, vector <double> epsilon);

// Shear Zone

double Vc(double bw, double fc, double deff);
bool CrossSectionalDimensionsConfirmationForShear(double VuVe, double fc, double vc, double bw, double deff, double ForShear);
int LegsMin(int Count);
double Av_s(double vuve, double Fyt, double deff, double ForShear);
double Vs(double vuve, double vc, bool isEarthquakeArea, double ForShear);
bool StirrupDesign(RcData& Data);

// Torsion Zone

double Acp(double bw, double h, double hf, double flanges, RcData::_BeamType_& BeamType);
double Pcp(double bw, double h, double hf, double flanges, RcData::_BeamType_& BeamType);
double Ph(double bw, double h, double cc, double Stirrup);
double Aoh(double bw, double h, double cc, int Stirrup);
double Ao(double aoh);
double Tth(double fc, double acp, double pcp);
bool TorsionalSectionalStrength(double Tu, double tth, double ForTorsion);
bool CrossSectionalDimensionsConfirmationForTorsion(double Vu, double Tu, double fc, double vc, double bw, double deff, double ph, double aoh, double ForTorsion);
double At_s(double Tu, double fyt, double ao, double ForTorsion);
double Al(double Tu, double fc, double fy, double fyt, double bw, double ph, double acp, double ao, double at_s, double ForTorsion);
double TorsionBarMaxSpacing(double bw, double h, double ph, int Stirrup, int TorsionBar);
void TorsionBarCountDesign(RcData& Data);
double TorsionBarDesignStrength(double fy, double fyt, double ao, double ph, double at_s, int al);
double ReductionTn(double Tn, double ForTorsion);

// Other Zone

double BarDiameter(int BarsSize);
double BarArea(int BarsSize);
double ReinforceRatio(double Grade);
bool IsEarthquakeArea(RcData::_SDC_& SDC);
void Coordinate(RcData& Data);
string Drawing(RcData& Data);