#include "includes.h"
class store_rs //SW rs2, imm(rs1)
{
private:

public:
	bool Busy;
	int Vj, Vk;
	string Qj, Qk;
	int A;
	int full_A;
	int index;	// no. of the load reservation station
	int waiting_time = 0;

	//load_rs();										//default constructor
	store_rs()
	{
		Busy = 0;
		Vj = 0;
		A = 0;
		full_A = 0;
		Qj = "";
		Vk = 0;
		Qk = "";
		//result = 0;
	};
	void issue(string rs1, string rs2, int reg_rs1, int reg_rs2, int imm)
	{
		if (rs1 != "") Qj = rs1;		//checks if the source register is available (no data dependencies)
		else 
		{
			Vj = reg_rs1;
			Qj = "";
		}
		Busy = 1;
		A = imm;
		if (rs2 != "") Qk = rs2;		//checks if the 2nd source register is available (no data dependencies)
		else
		{
			Vk = reg_rs2;
			Qk = "";
		}
	};		//takes the source resgister of load instruction
	void execute()
	{
		full_A = Vj + A;

	};
	void writeBack_store(int memory[64]) 
	{
		memory[full_A] = Vk;
		Busy = 0;
	};
	~store_rs() {};

};

