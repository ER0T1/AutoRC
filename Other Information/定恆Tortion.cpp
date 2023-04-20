#include <iostream>
using namespace std;

float main(){

	bool FlagReSize = 1;

	while (FlagReSize) {
		
		float b = 14, h = 24, Acp , Pcp, fc = 4000, fi1 = 0.75, Tu = 28, fiTth, hf = 0, hb = h - hf ;  //2
		float Vu = 57.1, Vc, d, Aoh, Ph, clearCover = 1.5, Stirrup = 0.5, func1, func2;  //3
		float Vs, fy = 60, AvcrossS, fi2 = 0.75; //4
		float AtcrossS, fi3 = 0.75, Ao, theta1 = 45 * 3.1415 / 180, fyt = 60;//5
		float needStirrup, func3, func4;//6
		float Al, At = 0.5, SofT = 5, func5, func6;//8 這裡先給At,橫向間距 之後再加入自行配筋程式

		//寫一張單位表不然換算頭痛死

		//Step2 
		//先假設為邊梁
		if (hb >= 4 * hf) {
			Acp = b * h + hf * 4 * hf;
		    Pcp = 2 * (b + h) + 8 * hf;
		}
		else {
			Acp = b * h + hf * hb;
			Pcp = 2 * (b + h) + 2 * hb;
		}
		
		fiTth = fi1 * sqrt(fc) * (Acp * Acp / Pcp) / 12000;
		cout << "Acp = " << Acp << " Pcp = " << Pcp << "\n" << "fi*Tth = " << fiTth << " k.ft\n";
		if (Tu >= fiTth)
			cout << "Tu >= fiTth  Torion need to be design\n";
		else {
			cout << "The Torsion didn't need to be design";
			return 0;
		}

		//Step3 Is the section large enough?
		d = h - 2.5; //這裡d先設成h-2.5 方便檢核 之後要改成 d-3 較保守
		Vc = 2 * sqrt(fc) * b * d / 1000;
		cout << "Vc = " << Vc << "k\n";

		Aoh = (b - 2 * clearCover - Stirrup) * (h - 2 * clearCover - Stirrup);
		Ph = 2 * (b - 2 * clearCover - Stirrup + h - 2 * clearCover - Stirrup);
		cout << "Aoh = " << Aoh << " in^2  Ph = " << Ph << " in\n";
		//這裡要考慮版的面積

		func1 = sqrt(Vu * 1000 * Vu * 1000 / (b * d) / (b * d) + pow(Tu * Ph * 12000 / 1.7 / Aoh / Aoh, 2));
		func2 = 0.75 * (Vc * 1000 / b / d + 8 * sqrt(fc));
		cout << "Fun1 = " << func1 << " Fuc2= " << func2 << "\n";

		if (func1 <= func2) {
			cout << "the cross section is OK!\n";
			FlagReSize = 0;
		}
		else {
			cout << "the cross section isn't large enoutgh!\n";
			FlagReSize = 1;
		}
		//斷面重新設置
		
		//Step4 Stirrup for shear
		//強震區Vc= 0
		Vs = Vu / fi2 - Vc;
		AvcrossS = Vs / fy / d;
		cout << "Av/s must >= " << AvcrossS << "\n";

		//Step5 Compute the stirrup require for torsion 
		Ao = 0.85 * Aoh;
		AtcrossS = Tu * 12 / fi3 / 2 / Ao / fyt * tan(theta1);
		cout << "At/s must >= " << AtcrossS << "\n";

		//Step6 Design stirrup for shear & torsion
		needStirrup = AvcrossS + 2 * AtcrossS;
		func3 = 0.75 * sqrt(fc) * b / fyt / 1000;
		func4 = 50 * b / fy / 1000;
		cout << "func3 = " << func3 << " func4 = " << func4 << "\n";

		if (needStirrup >= func3 && needStirrup >= func4) {
			;
		}
		else if (func3 >= func4) {
			needStirrup = func3;
		}
		else {
			needStirrup = func4;
		}

		cout << "The need of the stirrup is " << needStirrup << "in^2/in\n";

		//Step7 Check spacing

		//Step8 Design for longtitudinal reinforcement
		Al = Tu * 12 / fi3 * Ph / 2 / Ao / fy;
		func5 = 5 * sqrt(fc) * Acp / fy / 1000 - At / SofT * Ph * fyt / fy;
		func6 = 5 * sqrt(fc) * Acp / fy / 1000 - 25 * b / fyt / 1000 * Ph * fyt / fy;

		if (func5 >= Al && func6 >= Al) {
			if (func5 >= func6)
				Al = func5;
			else
				Al = func6;
		}

		cout << "The need of the longitudinal reinforcement for torsion is" << Al << "in^2\n";


		return 0;
	}
}