#include "includes.h"
class jalr_ret_rs
{
public:
	bool Busy;
	int Vj, Vk;
	string Qj, Qk;
	int A;
	int current_PC;
	int result=0;
	int waiting_time = 0;

	jalr_ret_rs()
	{
		Busy = 0;
		Vj = 0;
		Vk = 0;
		A = 0;
		Qj = "";
		Qk = "";
	};
	void issue(int reg_rs1, string rs1, string &R1, int PC) 
	{
		//rs1
		if (rs1 != "") {
			Qj = rs1;
		}
		else {
			Vj = reg_rs1;
			Qj = "";
		}

		R1 = "jalr_ret_rs";
		Busy = 1;
		current_PC = PC;
	};
	void execute(int &reg_R1, string instrtype)
	{
		if (instrtype == "jalr")
		{
			reg_R1 = current_PC + 1;
			result= current_PC + 1;
			A = Vj;
		}
		else {
			A = Vj;
		}
	};
	void write(int &PC, vector<bool>& wb_flag)
	{
		int n = PC;
		PC = A;
		for (int i = n; i <= PC; i++) {
			wb_flag[i] = true;
		}

		Busy = 0;
	};

	~jalr_ret_rs() {};

};

