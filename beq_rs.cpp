#include "includes.h"
class beq_rs //BEQ rs1, rs2, imm
{
private:

public:
	bool Busy;
	int Vj, Vk;
	string Qj, Qk;
	int A;
	int current_PC;
	int write_time;
	int waiting_time = 0;


	beq_rs() 
	{
		Busy = 0;
		Vj = 0;
		Vk = 0;
		A = 0;
		Qj = "";
		Qk = "";
	};
	void issue (int reg_rs1, string rs1, int reg_rs2, string rs2,  int PC) 
	{

		//rs1
		if (rs1 != "") {
			Qj = rs1;
		}
		else {
			Vj = reg_rs1;
			Qj = "";
		}
		//rs2
		if (rs2 != "") {
			Qk = rs2;
		}
		else {
			Vk = reg_rs2;
			Qk = "";
		}

		Busy = 1;
		current_PC = PC;
	};
	void execute(int imm, double& mispredicted_count)
	{
		if (Vk == Vj) {
			mispredicted_count++;
			A = current_PC + imm; //since we increase it by one in the main function
		}
	};
	void write(int &PC, vector<bool>& wb_flag)
	{
		int n = PC;
		PC = A;
		for (int i = n; i <= PC; i++)
		{
			wb_flag[i] = true;
		}

		Busy = 0;
	};
	~beq_rs() {};
};


