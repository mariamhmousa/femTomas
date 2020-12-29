#include "includes.h"
class div_rs //DIV rd, rs1, rs2		
{
private:

public:
	bool Busy;
	int Vj;
	int Vk;
	string Qj="", Qk="";
	int result;
	int waiting_time = 0;

	div_rs() 
	{
		Busy = 0;
		Vj = 0;
		Vk = 0;
		Qj = "";
		Qk = "";
		result = 0;
	};	
	void issue(string rs1, string rs2, string& rd, int reg_rs1, int reg_rs2)
	{
		if (rs1 != "") {
			Qj = rs1;
		}
		else {
			Vj = reg_rs1; Qj = "";
		}
		if (rs2 != "")
		{
			Qk = rs2;
		}
		else {
			Vk = reg_rs2; Qk = "";
		}
		Busy = 1;
		rd = "div";
	};
	void execute() 
	{
		if (Qj == "" && Qk == "")
		{
			result = Vj / Vk;
		}
	};	//compute values
	//void writeBack_load(RegisterStat x[8], int reg_x[8]);
	~div_rs() {};

};

