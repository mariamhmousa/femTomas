#include "includes.h"

class arithm_rs //ADD rd, rs1, rs2		NEG rd, rs1			ADDI rd, rs1, imm
{
private:

public:
	bool Busy;
	int Vj=0;
	int Vk=0;
	string Qj="", Qk="";
	string op_type;
	int index;	// no. of the load reservation station
	int result;
	int waiting_time = 0;
	arithm_rs()
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
		if (rs2 != "") {
			Qk = rs2;
		}
		else {
			Vk = reg_rs2; Qk = "";
		}
		Busy = 1;
		rd = "arithm" + to_string(index);
	};

	void execute(int imm)
	{
			if (Qj == "" && Qk == "")
			{
				if (op_type == "add")
				{
					result = Vj + Vk;
				}
				else if (op_type == "addi")
				{
					result = Vj + imm;
				}
				else { 
					result = ~Vj; 
				}
			}

	};	//compute values
	//void writeBack_load(RegisterStat x[8], int reg_x[8]);
	~arithm_rs() {};

};



