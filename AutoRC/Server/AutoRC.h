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
	double Mu1;							// 容許強度: 撓曲 lb-in		Allowable Strength: Bending Moment lb-in
	double Mu2;							// 容許強度: 撓曲 lb-in		Allowable Strength: Bending Moment lb-in
	double Vu1;							// 容許強度: 剪力 lb		Allowable Strength: Shear Force lb
	double Vu2;							// 容許強度: 剪力 lb		Allowable Strength: Shear Force lb
	double Vu;							// 容許強度: 剪力 lb		Allowable Strength: Shear Force lb
	double Ve;							// 容許強度: 剪力 lb		Allowable Strength: Shear Force lb
	double Tu1;							// 容許強度: 扭矩 lb-in		
	double Tu2;							// 容許強度: 扭矩 lb-in
	double Tu;							// 容許強度: 扭矩 lb-in

	double Sw;							// 尺寸資訊: 版寬 in
	double bw;							// 尺寸資訊: 梁寬 in
	double beff;						// 尺寸資訊: 有效翼板寬 in
	double bf = 84;						// 尺寸資訊: T梁翼板寬 in
	double h;							// 尺寸資訊: 梁深 in
	double hf;							// 尺寸資訊: 版厚 in
	double ln;							// 尺寸資訊: 梁跨 in
	double deffUpper, deffLower, deff;	// 尺寸資訊: 有效深度 in

	// 設計強度: 混凝土抗壓強度 psi
	double fc;
	// 設計強度: 混凝土抗剪強度 lb
	double vc;
	// 設計強度: 扭矩 lb-in
	double Tn, reductionTn;
	// 設計強度: 撓曲 lb-in
	double Mn1, Mn2, reductionMn1, reductionMn2;
	// Mpr 撓曲 lb-in
	double Mpr1, Mpr2;

	// 材料資訊: 鋼筋號數
	int MainBar, Stirrup, TorsionBar, SkinBar = 3;
	// 材料資訊: 鋼筋降伏強度 psi
	double MainBarGrade, StirrupGrade, TorsionBarGrade, SkinBarGrade = 60000.;
	// 材料資訊: 是否使用皮膚鋼筋
	bool HasSkinBar = false;
	// 材料資訊: 是否使用螺旋鋼筋
	bool isSpiralBar = false;

	// 扭矩主筋需求量 in^2
	double al;
	// 材料資訊: 扭矩主筋支數
	int TorsionBarCount, HorizontalTorsionBarCount, VerticalTorsionBarCount;

	// 撓曲鋼筋需求量 in^2
	double asMinUpper, asMaxUpper, asMinLower, asMaxLower;
	// 材料資訊: 撓曲主筋層數
	int UpperLayers = 1, LowerLayers = 1;
	// 材料資訊: 撓曲主筋總支數
	int AllUpperCount, AllLowerCount;
	// 材料資訊: 撓曲主筋每層鋼筋支數
	vector <int> UpperEachLayerCount, LowerEachLayerCount, Mn1_PerLayerCount, Mn2_PerLayerCount;
	// 材料資訊: 撓曲主筋鋼筋總支數限制
	int MainBarCountMinUpper, MainBarCountMaxUpper, MainBarCountMinLower, MainBarCountMaxLower;
	// 材料資訊: 撓曲主筋鋼筋每層總支數限制
	int HorizontalMainBarCountMinUpper, HorizontalMainBarCountMaxUpper, HorizontalMainBarCountMinLower, HorizontalMainBarCountMaxLower;
	// 材料資訊: 撓曲主筋鋼筋每層深度
	vector <double> PerLayerDUpper, PerLayerDLower, PerLayerDMn1, PerLayerDMn2;
	// 材料資訊: 撓曲主筋鋼筋每層應變
	vector <double> Mn1_epsilon, Mn2_epsilon, Mpr1_epsilon, Mpr2_epsilon;
	// 材料資訊: 撓曲壓力區深度
	double Mn1_CompressiveStressZone, Mn2_CompressiveStressZone, Mpr1_CompressiveStressZone, Mpr2_CompressiveStressZone;

	// 扭矩箍筋需求量 in^2/in
	double at_s;
	// 剪力箍筋需求量 in^2/in
	double av_s;
	// 材料資訊: 腳支數與限制
	int legs, legsMin, legsMax; bool OddAllowed;
	// 箍筋間距: in
	double stirrupSpacing, stirrupSpacingMax, stirrupSpacingSuggestion;
	// 設計強度: 箍筋抗剪強度 lb
	double vsMin;
	// 上下層筋哪一層主導
	int leading, depend;
	bool leader; // true 上層筋, false 下層筋

	// 材料資訊: 鋼筋保護層厚度
	double cc;
	// 材料資訊: 骨材粒徑大小 in
	double dagg = 3. / 4.;
	// 載重組合: 靜載重 psi/ft^3, 活載重 psi/ft^3, 靜載重因子, 活載重因子
	double Dl, Ll, LoadFactorDL = 1.2, LoadFactorLL = 1.;
	// 梁類型: 邊梁/中間梁
	enum class _BeamType_ { Middle, Side }BeamType;
	// 構架類型: SDC A, B, C, D, E, F
	enum class _SDC_ { A, B, C, D, E, F }SDC;
	// 警告資訊:
	enum class _Warning_ { ProblemNotFound, // 未找到錯誤
		InsufficientSectionCrossSectionalDimensionsForBw,		// 斷面尺寸不足: 梁寬
		InsufficientSectionCrossSectionalDimensionsForTorsion,  // 斷面尺寸不足: 扭矩
		InsufficientSectionCrossSectionalDimensionsForShear,	// 斷面尺寸不足: 剪力
		InsufficientSectionCrossSectionalDimensionsForAs,		// 斷面尺寸不足: 撓曲
		StirrupSpacingMoreThenStirrupSpacingMax,				// 使用者箍筋間距過大
		Av_sInsufficient,										// 剪力箍筋不足
		At_sInsufficient										// 扭矩箍筋不足
	}Warning = _Warning_::ProblemNotFound;

	// 參數
	// T梁翼板長 in
	double flanges;
	// 混凝土橫斷面外周圍成的面積 in^2
	double acp;
	// 混凝土橫斷面外周圍成的周長 in
	double pcp;
	// 最外側封閉橫向扭轉鋼筋中心線週長 in
	double ph;
	// 最外側封閉橫向扭轉鋼筋中心線圍成的面積 in^2
	double aoh;
	// 扭剪流路包圍的總面積 in^2
	double ao;
	// 臨界扭矩 in-lb
	double tth;
	

	// ACI 318-19 21.2.1
	struct {
		double ForTorsion = 0.75;
		double ForMoment_1, ForMoment_2;
		double ForShear = 0.75;
	}ReductionFactors;

	// 是否為強震區
	bool isEarthquakeArea;

	struct {
		vector <double> MainBar_H;
		int TorsionBar_H;
		vector <bool> legs_H;
		vector <bool> MainBarDepend_H;
		vector <double> MainBar_V, TorsionBar_V;
	}Coordinate;

	// 單位換算用, 未完成
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