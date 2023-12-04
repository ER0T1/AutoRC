// v 0.23.11.09

#define _WIN32_WINNT 0x0600
#pragma warning( disable : 4267 4244 26495)
#include <iostream>
#include "crow.h"
#include "AutoRC.h"
using namespace std;
using namespace crow;

// This program access location https://github.com/ER0T1/AutoRC

/*
#define _WIN32_WINNT_NT4                    0x0400 // Windows NT 4.0
#define _WIN32_WINNT_WIN2K                  0x0500 // Windows 2000
#define _WIN32_WINNT_WINXP                  0x0501 // Windows XP
#define _WIN32_WINNT_WS03                   0x0502 // Windows Server 2003
#define _WIN32_WINNT_WIN6                   0x0600 // Windows Vista
#define _WIN32_WINNT_VISTA                  0x0600 // Windows Vista
#define _WIN32_WINNT_WS08                   0x0600 // Windows Server 2008
#define _WIN32_WINNT_LONGHORN               0x0600 // Windows Vista
#define _WIN32_WINNT_WIN7                   0x0601 // Windows 7
#define _WIN32_WINNT_WIN8                   0x0602 // Windows 8
#define _WIN32_WINNT_WINBLUE                0x0603 // Windows 8.1
#define _WIN32_WINNT_WINTHRESHOLD           0x0A00 // Windows 10
#define _WIN32_WINNT_WIN10                  0x0A00 // Windows 10
*/
// https://learn.microsoft.com/zh-tw/cpp/porting/modifying-winver-and-win32-winnt?view=msvc-170
// use C++20 to compile to avoid various warnings... 

// vcpkg install crow
// API documentation https://crowcpp.org/master/reference/classcrow_1_1query__string.html

const auto port = 18080;

int main() {
	SimpleApp app;
	mustache::set_global_base("Website");

	CROW_ROUTE(app, "/")([]() {
		auto Page = mustache::load_text("HTMLPage.htm");
		return Page;
		});

	CROW_ROUTE(app, "/StyleSheet.css")([]() {
		auto StyleSheetPage = mustache::load_text("StyleSheet.css");
		return StyleSheetPage;
		});

	//input imformation from html
	CROW_ROUTE(app, "/Initialization").methods(HTTPMethod::POST)([](const request& req) {
		auto data = query_string("?" + req.body);

		RcData Data; // 創建資料
		// 輸入幾何參數:
		auto bw = data.get("bw");
		auto h = data.get("h");
		auto ln = data.get("ln");
		auto Sw = data.get("Sw");
		auto hf = data.get("hf");
		auto bf = data.get("bf");
		auto BeamType = data.get("BeamType");
		auto SDC = data.get("SDC");
		auto cc = data.get("cc");
		//cout << BeamType << endl << SDC << endl;
		if (bw) Data.bw = strtod(bw, NULL);
		if (h) Data.h = strtod(h, NULL);
		if (ln) Data.ln = strtod(ln, NULL) * ft2in;
		if (Sw) Data.Sw = strtod(Sw, NULL) * ft2in;
		if (hf) Data.hf = strtod(hf, NULL);
		if (bf) Data.bf = strtod(bf, NULL);
		if (BeamType) {
			auto beam_type = atoi(BeamType);
			if (beam_type == 0) { Data.BeamType = RcData::_BeamType_::Middle; }
			else if (beam_type == 1) { Data.BeamType = RcData::_BeamType_::Side; }
		}
		if (SDC) {
			auto sdc = atoi(SDC);
			if (sdc == 0) { Data.SDC = RcData::_SDC_::A; }
			else if (sdc == 1) { Data.SDC = RcData::_SDC_::B; }
			else if (sdc == 2) { Data.SDC = RcData::_SDC_::C; }
			else if (sdc == 3) { Data.SDC = RcData::_SDC_::D; }
			else if (sdc == 4) { Data.SDC = RcData::_SDC_::E; }
			else if (sdc == 5) { Data.SDC = RcData::_SDC_::F; }
		}
		if (cc) Data.cc = strtod(cc, NULL);
		// 輸入設計強度:
		auto Mu1 = data.get("Mu1");
		auto Mu2 = data.get("Mu2");
		auto Vu1 = data.get("Vu1");
		auto Vu2 = data.get("Vu2");
		auto Tu1 = data.get("Tu1");
		auto Tu2 = data.get("Tu2");
		if (Mu1) Data.Mu1 = strtod(Mu1, NULL) * kft2lbin;
		if (Mu2) Data.Mu2 = strtod(Mu2, NULL) * kft2lbin;
		if (Vu1) Data.Vu1 = strtod(Vu1, NULL) * kips2lb;
		if (Vu2) Data.Vu2 = strtod(Vu2, NULL) * kips2lb;
		if (Tu1) Data.Tu1 = strtod(Tu1, NULL) * kft2lbin;
		if (Tu2) Data.Tu2 = strtod(Tu2, NULL) * kft2lbin;
		// 輸入鋼筋號數:
		auto MainBar = data.get("MainBar");
		auto Stirrup = data.get("Stirrup");
		auto TorsionBar = data.get("TorsionBar");
		auto stirrupSpacing = data.get("SpacementOfStirrups");
		if (MainBar) Data.MainBar = atoi(MainBar);
		if (Stirrup) Data.Stirrup = atoi(Stirrup);
		if (TorsionBar) Data.TorsionBar = atoi(TorsionBar);
		if (stirrupSpacing) Data.stirrupSpacing = strtod(stirrupSpacing, NULL);
		// 輸入材料強度:
		auto MainBarGrade = data.get("MainBarGrade");
		auto StirrupGrade = data.get("StirrupGrade");
		auto TorsionBarGrade = data.get("TorsionBarGrade");
		auto fc = data.get("fc");
		if (MainBarGrade) Data.MainBarGrade = atoi(MainBarGrade) * 1000.;
		if (StirrupGrade) Data.StirrupGrade = atoi(StirrupGrade) * 1000.;
		if (TorsionBarGrade) Data.TorsionBarGrade = atoi(TorsionBarGrade) * 1000.;
		if (fc) Data.fc = atoi(fc) * 1000.;
		// 輸入載重組合:
		auto Dl = data.get("Dl");
		auto Ll = data.get("Ll");
		if (Dl) Data.Dl = strtod(Dl, NULL);
		if (Ll) Data.Ll = strtod(Ll, NULL);

		// 設計階段:
		Core(Data);
		auto Design = Struct2Json(Data);
		Design["SVG"] = Drawing(Data);
		return Design;
		});

	app.port(port).run();
	return 0;
}

crow::json::wvalue Struct2Json(RcData& Data) {
	crow::json::wvalue WebData;
	WebData["Mu1"] = Data.Mu1;
	WebData["Mu2"] = Data.Mu2;
	WebData["Vu1"] = Data.Vu1;
	WebData["Vu2"] = Data.Vu2;
	WebData["Tu"] = Data.Tu;
	WebData["Sw"] = Data.Sw;
	WebData["bw"] = Data.bw;
	WebData["hf"] = Data.hf;
	WebData["h"] = Data.h;
	WebData["bf"] = Data.bf;
	WebData["ln"] = Data.ln;
	WebData["fc"] = Data.fc;
	WebData["Dl"] = Data.Dl;
	WebData["Ll"] = Data.Ll;
	WebData["LoadFactorDL"] = Data.LoadFactorDL;
	WebData["LoadFactorLL"] = Data.LoadFactorLL;
	WebData["MainBar"] = Data.MainBar;
	WebData["Stirrup"] = Data.Stirrup;
	WebData["TorsionBar"] = Data.TorsionBar;
	WebData["cc"] = Data.cc;
	WebData["MainBarGrade"] = Data.MainBarGrade;
	WebData["StirrupGrade"] = Data.StirrupGrade;
	WebData["TorsionBarGrade"] = Data.TorsionBarGrade;
	WebData["dagg"] = Data.dagg;
	WebData["ReduceMn1"] = Data.reductionMn1;
	WebData["ReduceMn2"] = Data.reductionMn2;
	WebData["ReductionFactorMn1"] = Data.ReductionFactors.ForMoment_1;
	WebData["ReductionFactorMn2"] = Data.ReductionFactors.ForMoment_2;
	WebData["ReduceTn"] = Data.reductionTn;
	return WebData;
}