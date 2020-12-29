#ifndef DATE_H
#define DATE_H
#include "includes.h"
class instr
{
public:
	int issue_time, exec_time_start, exec_time_end, write_time;
	bool issue, execute, write;
	int rs_index;
	int rd=0;  // incase of jalr this is rs1
	string type;
	int rs1=0;
	int rs2=0,imm=0; // incase of beq or addi this is imm and in neg this is zero and should be ignored
	string rs_name;
	int executed_cycles;

	
	instr()
	{
		executed_cycles = 0;
		issue = 0;
		execute = 0;
		write = 0;
		issue_time = 0;
		exec_time_start = 0;
		exec_time_end = 0;
		write_time = 0;
		rs_index = -1;//the index that identifies which reservation station we are using
		rs_name = "";


		//string inst = "jalr r6";


	};
	void intialize(string inst)
	{
		string part;
		vector<string> instruction;
		string rdreg, rs1str, rs2str;
		for (int i = 0; i < inst.size(); i++)
		{
			if (inst[i] == ',')
			{
				inst.erase(inst.begin() + i);
			}
		}
		stringstream m(inst);
		while (m >> part)
		{
			instruction.push_back(part);
		}
		
		type = instruction[0];
		if (instruction[0] == "add" || instruction[0] == "div" || instruction[0] == "addi" || instruction[0] == "beq" || instruction[0] == "neg")
		{

			rdreg = instruction[1];
			rd = rdreg[1] - '0';
			rs1str = instruction[2];
			rs1 = rs1str[1] - '0';

			if (instruction[0] != "neg")
			{
				rs2str = instruction[3];

				if (instruction[0] != "addi" && instruction[0] != "beq")
				{
					rs2 = rs2str[1] - '0';

				}
				else
				{
					stringstream geek(rs2str);
					geek >> imm;

				}
			}
			if (instruction[0] == "beq")
			{
				int temp = rs1;
				rs1 = rd;
				rs2 = temp;
			}

		}
		else if (instruction[0] == "lw" || instruction[0] == "sw")
		{

			rdreg = instruction[1];
			if(instruction[0] != "sw")
			rd = rdreg[1] - '0';  
			else rs2 = rdreg[1] - '0';// or rs2 for sw
			rs1str = instruction[2];

			rs1str.pop_back();
			rs1 = rs1str[rs1str.size() - 1] - '0';
			rs1str.pop_back();
			rs1str.pop_back();

			stringstream geek(rs1str);
			geek >> imm;
		}
		else if (instruction[0] == "jalr")
		{
			rs1str = instruction[1];
			rs1 = rs1str[1] - '0';

		}


	};
};
#endif







//instr::~instr(){}

