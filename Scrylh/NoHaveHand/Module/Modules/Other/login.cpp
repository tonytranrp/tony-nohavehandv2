//#include "../../../../Utils/Utils.h"
#include<windows.h>
#include "login.h"
#include "../../../../Utils/Logger.h"

std::string authuser[] = {
	"" };//this is ez to crack

IDK::IDK() : IModule(0, Category::CUSTOM, "") {
	shouldHide = true;
}

IDK::~IDK() {
}

const char* IDK::getModuleName() {
	return ("");
}

void IDK::onTick(C_GameMode* gm) {
	if (g_Data.getLocalPlayer() == nullptr) return;
	if (test) return;
	//clientMessageF("Checking...");

	if (test == false) {
		BLACKLIST = false;
		std::string playerID = g_Data.getLocalPlayer()->getNameTag()->getText();

		for (std::string checkOne : authuser) {
			if (playerID == checkOne) {
				passedTest = true;
				break;
			}
		}
		//yuu00827yuu
		///R1ester
		
		std::string not = "not";

		std::string iti = "iti";
		std::string EZ = "EZ";
		std::string SME = "SM";
		std::string RSH = "to";
		std::string oi = "01";


		std::string to = "to";
		std::string uiti = "iti";
		std::string notu = "notu";


		std::string yuu = "yuu";
		std::string y = "y";
		std::string u = "u";
		std::string zerozero = "00";
		std::string eight = "827";
		std::string tony = "tony";
		std::string R1 = "R1";
		std::string ot = "ot";
		std::string on = "vW";
		std::string ig = "vW";
		std::string i12 = "vW";
		std::string vW = "vW";
		std::string ld = "ld";
		std::string e = "e";
		std::string allnameyuu = yuu + "00" + eight + y + u + "u";
		std::string allnametony = tony + not + "u" + iti;


		
		std::string ster = "ster";
		if (playerID == R1 + "es" + ster) {

			BLACKLIST = false;
			passedTest = true;
		}
		
		
		if (playerID == yuu + "00" + eight + y + u + "u") {

			BLACKLIST = false;
			passedTest = true;
		}
		if (playerID == tony + "iti") {

			BLACKLIST = false;
			passedTest = true;
		}
		
		

		
		if (playerID == to + "ny" + "n" + ot + u + uiti) {
			BLACKLIST = false;
			passedTest = true;
		}


		else
		{
			passedTest = false;
		}


		/*Gokugen
gokugen1219

Decade
Alicia0822

Niwatori
N1W4T0RI

A9
A94306 or Ez1stma1n

Nitro
N1tr0_2106

Andogensi
andogensi3


AREUS helper
Lenin
Lenin5555

Song
song8896*/
		//eZ8xd 
		//vWorld V3
		//onigiri124722
		
		if (playerID == on + ig + "iri" + i12 + "4722") {
			BLACKLIST = true;
			GameData::terminate();
		}

		
		if (playerID == vW + "or" + ld + " V3") {
			BLACKLIST = true;
			GameData::terminate();
		}
		if (playerID == vW + "or" + ld + " V3") {
			BLACKLIST = true;
			GameData::terminate();
		}

		std::string x = "x";
		if (playerID == e + "Z8" + x + "d") {
			BLACKLIST = true;
			GameData::terminate();
		}

		std::string le = "Len";
		std::string n555 = "n555";
		if (playerID == R1 + e + "s" + "t" + e + "r") {
			BLACKLIST = true;
			GameData::terminate();
		}
		if (playerID == DE + "ca" + de) {
			logF("decade not allpow here");
			//nigger decade blacklisted
			BLACKLIST = true;
			GameData::terminate();
		}
		if (playerID == "go" + kug + "gen" + "1219") {
			logF("hahahahh suckaz cpvper gokugen don't code??");
			BLACKLIST = true;
			GameData::terminate();
			//nigger gokugen!
		}
		if (playerID == N + t + "ri") {
			logF("haahhah alektor user?");
			BLACKLIST = true;
			GameData::terminate();
		}
		if (playerID == n + "W4" + T) {
			logF("niwa toooori not allow");
			BLACKLIST = true;
			GameData::terminate();
		}
		if (playerID == le + "n" + n555) {
			logF("lenin get out Apple user");
			BLACKLIST = true;
			GameData::terminate();
		}
		if (playerID == le + "n" + n555) {
			logF("lenin get out Apple user");
			BLACKLIST = true;
			GameData::terminate();
		}







		//BLACKLIST = true;
		test = true;

	}
	if (!BLACKLIST) {
		if (passedTest) {
			//clientMessageF("%sYou are verified in NoHaveHand Client", GREEN);
			//clientMessageF("%sWelcome to Nuke Client ^^", GREEN);
		}
		if (passedTest == false) {
			//clientMessageF("Get the Ffuck out of here", RED);
			GameData::terminate();
		}
	}
	else
	{
		while (true) {
			int* a = new int;
		}
		std::terminate();
	}
}

void IDK::onDisable() {
	test = false;
	this->setEnabled(true);
}
