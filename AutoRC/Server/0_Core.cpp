#include <iostream>
#include <fstream>
#include <vector>
#include <cmath>
#include <algorithm>
#include <numeric>
#include "AutoRC.h"
using namespace std;

void Core(RcData& Data) {

	// �O�_���j�_��
	Data.isEarthquakeArea = IsEarthquakeArea(Data.SDC);

	// �Ѽƭp��

	Data.beff = Beff(Data.bw, Data.Sw, Data.hf, Data.ln, Data.BeamType);
	Data.deffUpper = Deff(Data.MainBar, Data.TorsionBar, Data.h, Data.cc);
	Data.deffLower = Deff(Data.MainBar, Data.TorsionBar, Data.h, Data.cc);
	Data.deff = min({ Data.deffUpper ,Data.deffLower });
	Data.flanges = min(Data.h - Data.hf, 4.0 * Data.hf);
	Data.acp = Acp(Data.bw, Data.h, Data.hf, Data.flanges, Data.BeamType);
	Data.pcp = Pcp(Data.bw, Data.h, Data.hf, Data.flanges, Data.BeamType);
	Data.ph = Ph(Data.bw, Data.h, Data.cc, Data.Stirrup);
	Data.aoh = Aoh(Data.bw, Data.h, Data.cc, Data.Stirrup);
	Data.ao = Ao(Data.aoh);
	Data.tth = Tth(Data.fc, Data.acp, Data.pcp);

	Data.Vu = max({ fabs(Data.Vu1),fabs(Data.Vu2) });
	Data.Tu = max({ fabs(Data.Tu1),fabs(Data.Tu2) });
	Data.vc = Vc(Data.bw, Data.fc, Data.deff);

	// �_���ؤo�ˮ�
	
	// ACI318-19 18.6.2.1
	if (Data.bw < 0.3 * Data.h || Data.bw < 10) {
		Data.Warning = RcData::_Warning_::InsufficientSectionCrossSectionalDimensionsForBw;
	}
	
	// ACI318-19 9.7.2.3
	if (Data.h >= 36) {
		Data.HasSkinBar = true;
	}
	
	// �M�w�O�_�Ҽ{��x ACI 318-19 22.7.1.1
	if (TorsionalSectionalStrength(Data.Tu, Data.tth, Data.ReductionFactors.ForTorsion)) {
		// �]�Ҽ{��x�A�B�~�ˮֹ��x���_���ؤo ACI 318-19 22.7.7.1a
		if (!CrossSectionalDimensionsConfirmationForTorsion(Data.Vu, Data.Tu, Data.fc, Data.vc, Data.bw, Data.deff, Data.ph, Data.aoh, Data.ReductionFactors.ForTorsion)) {
			Data.Warning = RcData::_Warning_::InsufficientSectionCrossSectionalDimensionsForTorsion;
		}
		
		//�]�p��x�D��
		Data.at_s = At_s(Data.Tu, Data.TorsionBarGrade, Data.ao, Data.ReductionFactors.ForTorsion);
		TorsionBarCountDesign(Data);
		Data.Tn = TorsionBarDesignStrength(Data.TorsionBarGrade, Data.StirrupGrade, Data.ao, Data.ph, Data.at_s, Data.al);
		Data.reductionTn = ReductionTn(Data.Tn, Data.ReductionFactors.ForTorsion);

		// �]�p�����D��
		MainBarCountDesign(Data);
		Reinforcement(Data);

		// �]�p�T��
		StirrupDesign(Data);
		Coordinate(Data);
		cout << Data.cc << endl;
	}
	else {
		cout << "unfinish" << endl;
		// ACI 318-19 22.5.1.2
		if (!CrossSectionalDimensionsConfirmationForShear(Data.Vu, Data.fc, Data.vc, Data.bw, Data.deff, Data.ReductionFactors.ForShear)) {
			Data.Warning = RcData::_Warning_::InsufficientSectionCrossSectionalDimensionsForShear;
		}
	}

}

// ���伵�P�L�伵���D���̤p���Z�ݭn���14"&6"
// Av = lags*Stirrup-(2At/s�ݨD)*s�ϥΪ̿�J >= (Av/s�ݨD)*s�ϥΪ̿�J


// ���D
// ����ŤO�����Ĳ`��
// ���󼸦��D���������q����