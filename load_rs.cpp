#include "includes.h"
class load_rs //LW rd, imm(rs1)
{
private:

public:
	bool Busy;
	int Vj;
	string Qj;
	int A;
	int index;	// no. of the load reservation station
	int result;
	int waiting_time = 0;

	//load_rs();										//default constructor
	load_rs() {
		Busy = 0;
		Vj = 0;
		A = 0;
		Qj = "";

		//result=0
	};
	void issue(string rs1, string& rd, int reg_rs1, int imm) //takes the source resgister of load instruction
	{
		if (rs1 != "")
			Qj = rs1;		//checks if the source register is available (no data dependencies)
		else
		{
			Vj = reg_rs1;
			Qj = "";
		}
		Busy = 1;
		A = imm + Vj;
		rd = "load" + to_string(index);
	};		
	void execute(int memory[64])	//actual reading from memory
	{

		result = memory[A];

	};
	//void writeBack_load(RegisterStat x[8], int reg_x[8]);
	~load_rs() {};
	
};

