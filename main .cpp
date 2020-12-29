#include "includes.h"
#include "instr.cpp"
#include "beq_rs.cpp"
#include "jalr_ret_rs.cpp"
#include "load_rs.cpp"
#include "store_rs.cpp"
#include "arithm_rs.cpp"
#include "div_rs.cpp"
#include "instr.cpp"

int PC = 0;
int cycles_counter = 0;
bool Branch_flag = false;
bool jalr_ret_flag = false;
bool issue_flag = false;
bool write_flag = false;
load_rs load[2];
store_rs store[2];
beq_rs beq;
jalr_ret_rs jalr_ret;
arithm_rs arithm[2];
div_rs division;
string RegisterStat[8];
int RF[8] = { 0 };
queue <int> load_store_queue; //push and pop PCs
double beq_counter = 0;
double mispredicted_count = 0;
double branch_misprediction_rate(int beq_counter, int mispredicted_count);
int execution_time = 0;
double IPC = 0;
void Tomasulo(string instrtype, vector<instr>& instr_table, int& j, int clk_cycle, vector<bool>& wb_flag);
bool instrComplete(int n, vector<bool> wb_flag);
void printTable(vector<instr> inst_table);
int memory[64];
vector<string> instr_file;

int main()
{
	string fname;
	memory[0] = 1;
	memory[1] = 3;
	memory[2] = 1;
	cout << "WELCOME!!!\n\n";
	cout << "Enter file name of Instructions: ";
	cin >> fname;


	//input program
	string instruction;
	// fname="file1.txt";
	ifstream infile;
	string instrtype;
	//cin >> fname;
	infile.open(fname);
	//creation of reservation station objects
	RF[0] = 0;

	
	int clk_cycle = 0;
	int n;
	vector<instr> instr_table;
	int k = 0;
	while (!infile.eof()) // To get you all the lines.
	{
		getline(infile, instruction);
		instr_file.push_back(instruction);
		k++;

	}

	infile.close();
	int size = instr_file.size();
	instr_table.resize(size);
	n = size;
	for (int i = 0; i < size; i++)
	{ 
		instr_table[i].intialize(instr_file[i]);

	}
	//string instrtype;
	vector<bool> wb_flag(size,0);
	
	while (!instrComplete(n, wb_flag))
	{

		issue_flag = false;
		write_flag = false;
		for (int i = 0; i < size; i++) 
		{
			
			Tomasulo(instr_table[i].type, instr_table, i, clk_cycle, wb_flag);
		}
		clk_cycle++;
		
	}
	int d = 0;
	for (int i = 0; i < instr_table.size(); i++)
	{
		
		if(d< instr_table[i].write_time)
			d = instr_table[i].write_time;;
	}
	execution_time = d;
	IPC = (double)instr_file.size()/ execution_time;
	printTable(instr_table);
	system("pause");
	return 0;
}

bool instrComplete(int n, vector<bool> wb_flag) {
	for (int i = 0; i < wb_flag.size(); i++)
	{

		if (wb_flag[i] == 0)
			return false;
	}
	return true;
}

void Tomasulo(string instrtype, vector<instr>& instr_table, int& j, int clk_cycle, vector<bool> &wb_flag)
{
	if (!wb_flag[j])
	{	//flushing step 2
		if (instrtype == "lw")
		{
			if (!instr_table[j].issue && !issue_flag && !jalr_ret_flag)//// I S S U E
			{ 
				

				for (int i = 0; i < 2; i++)
				{
					if (!load[i].Busy)
					{
						load[i].index = i;
						load[i].issue(RegisterStat[instr_table[j].rs1], RegisterStat[instr_table[j].rd], RF[instr_table[j].rs1], instr_table[j].rs2);			//passing the instruction to extract the operands
						instr_table[j].issue = 1;
						instr_table[j].issue_time = clk_cycle;
						instr_table[j].rs_name = "load" + to_string(i);
						load_store_queue.push(j);
						issue_flag = true;
						break;
					}
				}
			}
			else if (!instr_table[j].execute && instr_table[j].issue && !Branch_flag && (load_store_queue.front() == j))///E X E C U T E
			{
				

				int n = instr_table[j].rs_name[4] - '0';
				if (load[n].Qj == "")
				{///r is head of load-store queue????
					instr_table[j].execute = 1;
					load[instr_table[j].rs_index].execute(memory);  // check ?
					if (load[n].waiting_time != 0)
					{
						instr_table[j].exec_time_start = load[n].waiting_time;
						instr_table[j].exec_time_end = load[n].waiting_time + 1;
						load[n].waiting_time = 0;
					}
					else 
					{
						instr_table[j].exec_time_start = clk_cycle;
						instr_table[j].exec_time_end = clk_cycle + 1;
					}
				
					

				}
			}
			else if (!instr_table[j].write && instr_table[j].execute && !write_flag)
			{/// W R I T E
				instr_table[j].write_time = instr_table[j].exec_time_end + 1;


				int n = instr_table[j].rs_name[4] - '0';
				for (int i = 0; i < 8; i++)
				{
					if (RegisterStat[i] == (instr_table[j].rs_name))
					{
						RF[i] = load[n].result;
						RegisterStat[i] = "";
					}
				}

				if (load[0].Qj == instr_table[j].rs_name)
				{
					load[0].Vj = load[n].result;
					load[0].Qj = "";
					if (load[0].waiting_time < instr_table[j].write_time)
					load[0].waiting_time = instr_table[j].write_time +1;

				}
				if (load[1].Qj == instr_table[j].rs_name) {
					load[1].Vj = load[n].result;
					load[1].Qj = "";
				if (load[1].waiting_time < instr_table[j].write_time)
					load[1].waiting_time = instr_table[j].write_time + 1;
				}
				if (store[0].Qj == instr_table[j].rs_name)
				{
					store[0].Vj = load[n].result;
					store[0].Qj = "";
				if (store[0].waiting_time < instr_table[j].write_time)
					store[0].waiting_time = instr_table[j].write_time + 1;
				}
				if (store[1].Qj == instr_table[j].rs_name)
				{
					store[1].Vj = load[n].result;
					store[1].Qj = "";
				if (store[1].waiting_time < instr_table[j].write_time)
					store[1].waiting_time = instr_table[j].write_time + 1;
				}
				if (arithm[0].Qj == instr_table[j].rs_name)
				{
					arithm[0].Vj = load[n].result;
					arithm[0].Qj = "";
				if (arithm[0].waiting_time < instr_table[j].write_time)
					arithm[0].waiting_time = instr_table[j].write_time + 1;
				}

				if (arithm[1].Qj == instr_table[j].rs_name)
				{
					arithm[1].Vj = load[n].result;
					arithm[1].Qj = "";
				if (arithm[1].waiting_time < instr_table[j].write_time)
					arithm[1].waiting_time = instr_table[j].write_time + 1;
				}
				if (beq.Qj == instr_table[j].rs_name)
				{
					beq.Vj = load[n].result;
					beq.Qj = "";
				if (beq.waiting_time < instr_table[j].write_time)
					beq.waiting_time = instr_table[j].write_time + 1;
				}
				if (division.Qj == instr_table[j].rs_name)
				{
					division.Vj = load[n].result;
					division.Qj = "";
				if (division.waiting_time < instr_table[j].write_time)
					division.waiting_time = instr_table[j].write_time + 1;
				}
				if (jalr_ret.Qj == instr_table[j].rs_name)
				{
					jalr_ret.Vj = load[n].result;
					jalr_ret.Qj = "";
				if (jalr_ret.waiting_time < instr_table[j].write_time)
					jalr_ret.waiting_time = instr_table[j].write_time + 1;
				}

				if (store[0].Qk == instr_table[j].rs_name)
				{
					store[0].Vk = load[n].result;
					store[0].Qk = "";
				if (store[0].waiting_time < instr_table[j].write_time)
					store[0].waiting_time = instr_table[j].write_time + 1;

				}
				if (store[1].Qk == instr_table[j].rs_name)
				{
					store[1].Vk = load[n].result;
					store[1].Qk = "";
				if (store[1].waiting_time < instr_table[j].write_time)
					store[1].waiting_time = instr_table[j].write_time + 1;

				}

				if (arithm[0].Qk == instr_table[j].rs_name)
				{
					arithm[0].Vk = load[n].result;
					arithm[0].Qk = "";
				if (arithm[0].waiting_time < instr_table[j].write_time)
					arithm[0].waiting_time = instr_table[j].write_time + 1;

				}
				if (arithm[1].Qk == instr_table[j].rs_name)
				{
					arithm[1].Vk = load[n].result;
					arithm[1].Qk = "";
				if (arithm[1].waiting_time < instr_table[j].write_time)
					arithm[1].waiting_time = instr_table[j].write_time + 1;

				}
				if (beq.Qk == instr_table[j].rs_name)
				{
					beq.Vk = load[n].result;
					beq.Qk = "";
				if (beq.waiting_time < instr_table[j].write_time)
					beq.waiting_time = instr_table[j].write_time + 1;
				}
				if (division.Qk == instr_table[j].rs_name)
				{

					division.Vk = load[n].result;
					division.Qk = "";
			   if (division.waiting_time < instr_table[j].write_time)
					division.waiting_time = instr_table[j].write_time + 1;
				}
				if (jalr_ret.Qk == instr_table[j].rs_name)
				{
					jalr_ret.Vk = load[n].result;
					jalr_ret.Qk = "";
					if(jalr_ret.waiting_time< instr_table[j].write_time)
					jalr_ret.waiting_time = instr_table[j].write_time+1;
				}

				load[n].Busy = 0;
				instr_table[j].write = 1;
				load_store_queue.pop();
				write_flag = true; // to not have two instrs. writing in same clkcycle;
				wb_flag[j] = true;  // instruction finished
				
			}
		}
		else if (instrtype == "sw")
		{

			if (!instr_table[j].issue && !issue_flag && !jalr_ret_flag)//// I S S U E
			{
				for (int i = 0; i < 2; i++)
				{
					if (!store[i].Busy) {
						store[i].issue(RegisterStat[instr_table[j].rs1], RegisterStat[instr_table[j].rs2], RF[instr_table[j].rs1], RF[instr_table[j].rs2], instr_table[j].imm);			//passing the instruction to extract the operands
						instr_table[j].issue = 1;
						instr_table[j].issue_time = clk_cycle;
						instr_table[j].rs_name = "store" + to_string(i);
						load_store_queue.push(j);
						issue_flag = true;
						
						break;
					}
				}
			}
			else if (!instr_table[j].execute && instr_table[j].issue && !Branch_flag && (load_store_queue.front() == j))///E X E C U T E
			{
				int n = instr_table[j].rs_name[5] - '0';
				if (store[n].Qj == "" && store[n].Qk == "") 
				{///r is head of load-store queue????
					instr_table[j].execute = 1;
					store[instr_table[j].rs_index].execute();
					if (store[n].waiting_time != 0)
					{
						instr_table[j].exec_time_start = store[n].waiting_time;
						instr_table[j].exec_time_end = store[n].waiting_time + 1;
						store[n].waiting_time = 0;
					}
					else
					{
						instr_table[j].exec_time_start = clk_cycle;
						instr_table[j].exec_time_end = clk_cycle + 1;
					}
				
				}
			}
			else if (!instr_table[j].write && instr_table[j].execute && !write_flag)
			{/// W R I T E
				int n = instr_table[j].rs_name[5] - '0';
				/*for (int i = 0; i < 8; i++)
				{
					if (RegisterStat[i] == (instr_table[j].rs_name))
					{
						RF[i] = store[n].result;
						RegisterStat[i] = "";
					}
				}

				if (load[0].Qj == instr_table[j].rs_name) load[0].Vj = store[n].result;
				else if (load[1].Qj == instr_table[j].rs_name) load[1].Vj = store[n].result;
				else if (store[0].Qj == instr_table[j].rs_name) store[0].Vj = store[n].result;
				else if (store[1].Qj == instr_table[j].rs_name)  store[1].Vj = store[n].result;
				else if (arithm[0].Qj == instr_table[j].rs_name)  arithm[0].Vj = store[n].result;
				else if (arithm[1].Qj == instr_table[j].rs_name)  arithm[1].Vj = store[n].result;
				else if (beq.Qj == instr_table[j].rs_name)  beq.Vj = store[n].result;
				else if (division.Qj == instr_table[j].rs_name)  division.Vj = store[n].result;
				else if (jalr_ret.Qj == instr_table[j].rs_name) jalr_ret.Vj = store[n].result;

				if (store[0].Qk == instr_table[j].rs_name) store[0].Vk = store[n].result;
				else if (store[1].Qk == instr_table[j].rs_name)  store[1].Vk = store[n].result;
				else if (arithm[0].Qk == instr_table[j].rs_name)  arithm[0].Vk = store[n].result;
				else if (arithm[1].Qk == instr_table[j].rs_name)  arithm[1].Vk = store[n].result;
				else if (beq.Qk == instr_table[j].rs_name)  beq.Vk = store[n].result;
				else if (division.Qk == instr_table[j].rs_name)  division.Vk = store[n].result;
				else if (jalr_ret.Qk == instr_table[j].rs_name) jalr_ret.Vk = store[n].result;
				*/
				store[n].Busy = 0;
				instr_table[j].write = 1;
				instr_table[j].write_time = instr_table[j].exec_time_end+1;
				load_store_queue.pop();
				write_flag = true;
				wb_flag[j] = true;  // instruction finished
				
			}
		}
		else if (instrtype == "beq")
		{
			if (!instr_table[j].issue && !issue_flag && !jalr_ret_flag)
			{
				if (!beq.Busy) {
					beq.issue(RF[instr_table[j].rs1], RegisterStat[instr_table[j].rs1], RF[instr_table[j].rs2], RegisterStat[instr_table[j].rs2], j);//passing the instruction to extract the operands
					instr_table[j].issue = 1;
					instr_table[j].issue_time = clk_cycle;
					Branch_flag = true;
					instr_table[j].rs_name = "beq";
					beq_counter++;
					issue_flag = true;
				
				}
			}
			else if (!instr_table[j].execute && instr_table[j].issue)
			{
				if (beq.Qj == "" && beq.Qk == "")
				{
					instr_table[j].execute = true;
					
					beq.execute(instr_table[j].imm, mispredicted_count);
					if (beq.waiting_time != 0)
					{
						instr_table[j].exec_time_start = beq.waiting_time;
						instr_table[j].exec_time_end = beq.waiting_time;
						beq.waiting_time = 0;
					}
					else
					{
						instr_table[j].exec_time_start = clk_cycle;
						instr_table[j].exec_time_end = clk_cycle ;
					}
				}
			
			}
			else if (!instr_table[j].write && instr_table[j].execute ) 
			{
				instr_table[j].write_time = instr_table[j].exec_time_end +1;
				instr_table[j].write = true;
				wb_flag[j] = 1;

				//beq.write(j, wb_flag); //pass pc
				int start_index = beq.current_PC + 1;
				for (int i = start_index; i <= j; i++) 
				{	//flushing step 1
					if (instr_table[i].rs_name == "load0") load[0].Busy = 0;
					 if (instr_table[i].rs_name == "load1") load[1].Busy = 0;
					 if (instr_table[i].rs_name == "store0") store[0].Busy = 0;
					 if (instr_table[i].rs_name == "store1") store[1].Busy = 0;
					 if (instr_table[i].rs_name == "arithm0") arithm[0].Busy = 0;
					 if (instr_table[i].rs_name == "arithm1") arithm[1].Busy = 0;
					 if (instr_table[i].rs_name == "beq") beq.Busy = 0;
					 if (instr_table[i].rs_name == "jalr_ret") jalr_ret.Busy = 0;
					 if (instr_table[i].rs_name == "div") division.Busy = 0;
				}
				Branch_flag = false;
				write_flag = true;	//so that no instructions write at the same cc

				
				for (int i = j; i < beq.A; i++)
				{
						wb_flag[i] = true;
				}
				j = beq.A;
				beq.Busy = 0;
			
			}
		}
		else if (instrtype == "jalr" || instrtype == "ret") {
			if (!instr_table[j].issue && !issue_flag && !jalr_ret_flag) {   // I S S U E
				if (!jalr_ret.Busy) {
					if (instrtype == "jalr") 
					{
						jalr_ret.issue(RF[instr_table[j].rs1], RegisterStat[instr_table[j].rs1], RegisterStat[1], PC);
					}
					else {
						jalr_ret.issue(RF[1], RegisterStat[1], RegisterStat[1], PC);
					}
					instr_table[j].issue = 1;
					instr_table[j].issue_time = clk_cycle;
					instr_table[j].rs_name = "jalr_ret";
					issue_flag = true;
					jalr_ret_flag = true;
				}
				
			}
			else if (!instr_table[j].execute && instr_table[j].issue && !Branch_flag)
			{
				if (jalr_ret.Qj == "")
				{
					instr_table[j].execute = 1;
					jalr_ret.execute(RF[1], instrtype);
					if (jalr_ret.waiting_time != 0)
					{
						instr_table[j].exec_time_start = jalr_ret.waiting_time;
						instr_table[j].exec_time_end = jalr_ret.waiting_time;
						jalr_ret.waiting_time = 0;
					}
					else
					{
						instr_table[j].exec_time_start = clk_cycle;
						instr_table[j].exec_time_end = clk_cycle;
					}
				}
				
			}
			else if (!instr_table[j].write && instr_table[j].execute && instr_table[j].issue && !write_flag)
			{
				//int n = PC;
				
			
				//jalr_ret.write(PC, wb_flag);


				if (load[0].Qj == instr_table[j].rs_name)
				{
					load[0].Vj = jalr_ret.result;
					load[0].Qj = "";
					if (load[0].waiting_time < instr_table[j].write_time)
						load[0].waiting_time = instr_table[j].write_time + 1;

				}
				if (load[1].Qj == instr_table[j].rs_name) {
					load[1].Vj = jalr_ret.result;
					load[1].Qj = "";
					if (load[1].waiting_time < instr_table[j].write_time)
						load[1].waiting_time = instr_table[j].write_time + 1;
				}
				if (store[0].Qj == instr_table[j].rs_name)
				{
					store[0].Vj = jalr_ret.result;
					store[0].Qj = "";
					if (store[0].waiting_time < instr_table[j].write_time)
						store[0].waiting_time = instr_table[j].write_time + 1;
				}
				if (store[1].Qj == instr_table[j].rs_name)
				{
					store[1].Vj = jalr_ret.result;
					store[1].Qj = "";
					if (store[1].waiting_time < instr_table[j].write_time)
						store[1].waiting_time = instr_table[j].write_time + 1;
				}
				if (arithm[0].Qj == instr_table[j].rs_name)
				{
					arithm[0].Vj = jalr_ret.result;
					arithm[0].Qj = "";
					if (arithm[0].waiting_time < instr_table[j].write_time)
						arithm[0].waiting_time = instr_table[j].write_time + 1;
				}

				if (arithm[1].Qj == instr_table[j].rs_name)
				{
					arithm[1].Vj = jalr_ret.result;
					arithm[1].Qj = "";
					if (arithm[1].waiting_time < instr_table[j].write_time)
						arithm[1].waiting_time = instr_table[j].write_time + 1;
				}
				if (beq.Qj == instr_table[j].rs_name)
				{
					beq.Vj = jalr_ret.result;
					beq.Qj = "";
					if (beq.waiting_time < instr_table[j].write_time)
						beq.waiting_time = instr_table[j].write_time + 1;
				}
				if (division.Qj == instr_table[j].rs_name)
				{
					division.Vj = jalr_ret.result;
					division.Qj = "";
					if (division.waiting_time < instr_table[j].write_time)
						division.waiting_time = instr_table[j].write_time + 1;
				}
				if (jalr_ret.Qj == instr_table[j].rs_name)
				{
					jalr_ret.Vj = jalr_ret.result;
					jalr_ret.Qj = "";
					if (jalr_ret.waiting_time < instr_table[j].write_time)
						jalr_ret.waiting_time = instr_table[j].write_time + 1;
				}

				if (store[0].Qk == instr_table[j].rs_name)
				{
					store[0].Vk = jalr_ret.result;
					store[0].Qk = "";
					if (store[0].waiting_time < instr_table[j].write_time)
						store[0].waiting_time = instr_table[j].write_time + 1;

				}
				if (store[1].Qk == instr_table[j].rs_name)
				{
					store[1].Vk = jalr_ret.result;
					store[1].Qk = "";
					if (store[1].waiting_time < instr_table[j].write_time)
						store[1].waiting_time = instr_table[j].write_time + 1;

				}

				if (arithm[0].Qk == instr_table[j].rs_name)
				{
					arithm[0].Vk = jalr_ret.result;
					arithm[0].Qk = "";
					if (arithm[0].waiting_time < instr_table[j].write_time)
						arithm[0].waiting_time = instr_table[j].write_time + 1;

				}
				if (arithm[1].Qk == instr_table[j].rs_name)
				{
					arithm[1].Vk = jalr_ret.result;
					arithm[1].Qk = "";
					if (arithm[1].waiting_time < instr_table[j].write_time)
						arithm[1].waiting_time = instr_table[j].write_time + 1;

				}
				if (beq.Qk == instr_table[j].rs_name)
				{
					beq.Vk = jalr_ret.result;
					beq.Qk = "";
					if (beq.waiting_time < instr_table[j].write_time)
						beq.waiting_time = instr_table[j].write_time + 1;
				}
				if (division.Qk == instr_table[j].rs_name)
				{

					division.Vk = jalr_ret.result;
					division.Qk = "";
					if (division.waiting_time < instr_table[j].write_time)
						division.waiting_time = instr_table[j].write_time + 1;
				}
			
				instr_table[j].write = 1;
				instr_table[j].write_time = instr_table[j].exec_time_end+1;
				write_flag = true;
				jalr_ret_flag = false;
				wb_flag[j] = true;  // instruction finished
				RegisterStat[1] = "";
				
				
				for (int i = j; i < jalr_ret.A; i++)
				{
					wb_flag[i] = true;
				}
				j = jalr_ret.A;
				jalr_ret.Busy = 0;
			}
		}

		else if (instrtype == "add" || instrtype == "addi" || instrtype == "neg")
		{
			if (!instr_table[j].issue && !issue_flag && !jalr_ret_flag)
			{
				for (int i = 0; i < 2; i++)
				{
					if (!arithm[i].Busy)
					{
						arithm[i].op_type = instrtype;
						arithm[i].index = i;
						arithm[i].issue(RegisterStat[instr_table[j].rs1], RegisterStat[instr_table[j].rs2], RegisterStat[instr_table[j].rd], RF[instr_table[j].rs1], RF[instr_table[j].rs2]);
						instr_table[j].issue = 1;
						instr_table[j].issue_time = clk_cycle;
						issue_flag = true;
						instr_table[j].rs_name = "arithm" + to_string(i);
					
						break;
					}
				}
			}
			else if (!instr_table[j].execute && instr_table[j].issue && !Branch_flag)
			{

				int n = instr_table[j].rs_name[6] - '0';
				if ((arithm[n].Qj == "") && (arithm[n].Qk == ""))
				{
					instr_table[j].execute = 1;
					arithm[n].execute(instr_table[j].imm);

					if (arithm[n].waiting_time != 0)
					{
						instr_table[j].exec_time_start = arithm[n].waiting_time;
						instr_table[j].exec_time_end = arithm[n].waiting_time + 1;
						arithm[n].waiting_time = 0;
					}
					else
					{
						instr_table[j].exec_time_start = clk_cycle;
						instr_table[j].exec_time_end = clk_cycle + 1;
					}
					
				}
			}
			else if (!instr_table[j].write && instr_table[j].execute && instr_table[j].issue && !write_flag && !Branch_flag)
			{
				instr_table[j].write_time = instr_table[j].exec_time_end + 1;

				int n = instr_table[j].rs_name[6] - '0';
				for (int i = 0; i < 8; i++)
				{
					if (RegisterStat[i] == (instr_table[j].rs_name))
					{
						RF[i] = arithm[n].result;
						RegisterStat[i] = "";
					}
				}

				if (load[0].Qj == instr_table[j].rs_name)
				{
					load[0].Vj = arithm[n].result;
					load[0].Qj = "";
					if (load[0].waiting_time < instr_table[j].write_time)
						load[0].waiting_time = instr_table[j].write_time + 1;

				}
				if (load[1].Qj == instr_table[j].rs_name) {
					load[1].Vj = arithm[n].result;
					load[1].Qj = "";
					if (load[1].waiting_time < instr_table[j].write_time)
						load[1].waiting_time = instr_table[j].write_time + 1;
				}
				if (store[0].Qj == instr_table[j].rs_name)
				{
					store[0].Vj = arithm[n].result;
					store[0].Qj = "";
					if (store[0].waiting_time < instr_table[j].write_time)
						store[0].waiting_time = instr_table[j].write_time + 1;
				}
				if (store[1].Qj == instr_table[j].rs_name)
				{
					store[1].Vj = arithm[n].result;
					store[1].Qj = "";
					if (store[1].waiting_time < instr_table[j].write_time)
						store[1].waiting_time = instr_table[j].write_time + 1;
				}
				if (arithm[0].Qj == instr_table[j].rs_name)
				{
					arithm[0].Vj = arithm[n].result;
					arithm[0].Qj = "";
					if (arithm[0].waiting_time < instr_table[j].write_time)
						arithm[0].waiting_time = instr_table[j].write_time + 1;
				}
				if (arithm[1].Qj == instr_table[j].rs_name)
				{
					arithm[1].Vj = arithm[n].result;
					arithm[1].Qj = "";
					if (arithm[1].waiting_time < instr_table[j].write_time)
						arithm[1].waiting_time = instr_table[j].write_time + 1;
				}
				if (beq.Qj == instr_table[j].rs_name)
				{
					beq.Vj = arithm[n].result;
					beq.Qj = "";
					if (beq.waiting_time < instr_table[j].write_time)
						beq.waiting_time = instr_table[j].write_time + 1;
				}
				if (division.Qj == instr_table[j].rs_name)
				{
					division.Vj = arithm[n].result;
					division.Qj = "";
					if (division.waiting_time < instr_table[j].write_time)
						division.waiting_time = instr_table[j].write_time + 1;
				}
				if (jalr_ret.Qj == instr_table[j].rs_name)
				{
					jalr_ret.Vj = arithm[n].result;
					jalr_ret.Qj = "";
					if (jalr_ret.waiting_time < instr_table[j].write_time)
						jalr_ret.waiting_time = instr_table[j].write_time + 1;
				}

				if (store[0].Qk == instr_table[j].rs_name)
				{
					store[0].Vk = arithm[n].result;
					store[0].Qk = "";
					if (store[0].waiting_time < instr_table[j].write_time)
						store[0].waiting_time = instr_table[j].write_time + 1;

				}
				if (store[1].Qk == instr_table[j].rs_name)
				{
					store[1].Vk = arithm[n].result;
					store[1].Qk = "";
					if (store[1].waiting_time < instr_table[j].write_time)
						store[1].waiting_time = instr_table[j].write_time + 1;

				}
				if (arithm[0].Qk == instr_table[j].rs_name)
				{
					arithm[0].Vk = arithm[n].result;
					arithm[0].Qk = "";
					if (arithm[0].waiting_time < instr_table[j].write_time)
						arithm[0].waiting_time = instr_table[j].write_time + 1;

				}
				if (arithm[1].Qk == instr_table[j].rs_name)
				{
					arithm[1].Vk = arithm[n].result;
					arithm[1].Qk = "";
					if (arithm[1].waiting_time < instr_table[j].write_time)
						arithm[1].waiting_time = instr_table[j].write_time + 1;

				}
				if (beq.Qk == instr_table[j].rs_name)
				{
					beq.Vk = arithm[n].result;
					beq.Qk = "";
					if (beq.waiting_time < instr_table[j].write_time)
						beq.waiting_time = instr_table[j].write_time + 1;
				}
				if (division.Qk == instr_table[j].rs_name)
				{

					division.Vk = arithm[n].result;
					division.Qk = "";
					if (division.waiting_time < instr_table[j].write_time)
						division.waiting_time = instr_table[j].write_time + 1;
				}
			

				arithm[n].Busy = 0;
				instr_table[j].write = 1;
				write_flag = true; 
				 wb_flag[j] = true;  // instruction finished
				
			}
		}
		else if (instrtype == "div")
		{
			if (!instr_table[j].issue && !issue_flag && !jalr_ret_flag)
			{

				if (!division.Busy)
				{
					division.issue(RegisterStat[instr_table[j].rs1], RegisterStat[instr_table[j].rs2], RegisterStat[instr_table[j].rd], instr_table[j].rs1, instr_table[j].rs2);

					instr_table[j].issue = 1;
					instr_table[j].issue_time = clk_cycle;
					issue_flag = true;
					instr_table[j].rs_name = "div";
				
				}

			}
			else if (!instr_table[j].execute && instr_table[j].issue && !Branch_flag)
			{
				if ((division.Qj == "") && (division.Qk == ""))
				{
					instr_table[j].execute = 1;
					division.execute();
					if (division.waiting_time != 0)
					{
						instr_table[j].exec_time_start = division.waiting_time;
						instr_table[j].exec_time_end = division.waiting_time + 7;
						division.waiting_time = 0;
					}
					else
					{
						instr_table[j].exec_time_start = clk_cycle;
						instr_table[j].exec_time_end = clk_cycle + 7;
					}
					
				}
			}
			else if (!instr_table[j].write && instr_table[j].execute && instr_table[j].issue && !write_flag && !Branch_flag)
			{
				instr_table[j].write_time = instr_table[j].exec_time_end + 1;


				for (int i = 0; i < 8; i++)
				{
					if (RegisterStat[i] == (instr_table[j].rs_name))
					{
						RF[i] = division.result;
						RegisterStat[i] = "";
					}
				}

				if (load[0].Qj == instr_table[j].rs_name)
				{
					load[0].Vj = division.result;
					load[0].Qj = "";
					if (load[0].waiting_time < instr_table[j].write_time)
						load[0].waiting_time = instr_table[j].write_time + 1;

				}
				if (load[1].Qj == instr_table[j].rs_name) {
					load[1].Vj = division.result;
					load[1].Qj = "";
					if (load[1].waiting_time < instr_table[j].write_time)
						load[1].waiting_time = instr_table[j].write_time + 1;
				}

				if (store[0].Qj == instr_table[j].rs_name)
				{
					store[0].Vj = division.result;
					store[0].Qj = "";
					if (store[0].waiting_time < instr_table[j].write_time)
						store[0].waiting_time = instr_table[j].write_time + 1;
				}
				if (store[1].Qj == instr_table[j].rs_name)
				{

					store[1].Vj = division.result;
					store[1].Qj = "";
					if (store[1].waiting_time < instr_table[j].write_time)
						store[1].waiting_time = instr_table[j].write_time + 1;
				}
				if (arithm[0].Qj == instr_table[j].rs_name)
				{
					arithm[0].Vj = division.result;
					arithm[0].Qj = "";
					if (arithm[0].waiting_time < instr_table[j].write_time)
						arithm[0].waiting_time = instr_table[j].write_time + 1;
				}
				if (arithm[1].Qj == instr_table[j].rs_name)
				{
					arithm[1].Vj = division.result;
					arithm[1].Qj = "";
					if (arithm[1].waiting_time < instr_table[j].write_time)
						arithm[1].waiting_time = instr_table[j].write_time + 1;
				}
				if (beq.Qj == instr_table[j].rs_name)
				{
					beq.Vj = division.result;
					beq.Qj = "";
					if (beq.waiting_time < instr_table[j].write_time)
						beq.waiting_time = instr_table[j].write_time + 1;
				}
				if (division.Qj == instr_table[j].rs_name)
				{
					division.Vj = division.result;
					division.Qj = "";
					if (division.waiting_time < instr_table[j].write_time)
						division.waiting_time = instr_table[j].write_time + 1;
				}
				if (jalr_ret.Qj == instr_table[j].rs_name)
				{
					jalr_ret.Vj = division.result;
					jalr_ret.Qj = "";
					if (jalr_ret.waiting_time < instr_table[j].write_time)
						jalr_ret.waiting_time = instr_table[j].write_time + 1;
				}


				if (store[0].Qk == instr_table[j].rs_name)
				{
					store[0].Vk = division.result;
					store[0].Qk = "";
					if (store[0].waiting_time < instr_table[j].write_time)

					{
						store[0].waiting_time = instr_table[j].write_time + 1;
					}

				}

				if (store[1].Qk == instr_table[j].rs_name)
				{
					store[1].Vk = division.result;
					store[1].Qk = "";
					if (store[1].waiting_time < instr_table[j].write_time)
						store[1].waiting_time = instr_table[j].write_time + 1;

				}
				if (arithm[0].Qk == instr_table[j].rs_name)
				{
					arithm[0].Vk = division.result;
					arithm[0].Qk = "";
					if (arithm[0].waiting_time < instr_table[j].write_time)
						arithm[0].waiting_time = instr_table[j].write_time + 1;

				}
				if (arithm[1].Qk == instr_table[j].rs_name)
				{
					arithm[1].Vk = division.result;
					arithm[1].Qk = "";
					if (arithm[1].waiting_time < instr_table[j].write_time)
						arithm[1].waiting_time = instr_table[j].write_time + 1;

				}
				if (beq.Qk == instr_table[j].rs_name)
				{
					beq.Vk = division.result;
					beq.Qk = "";
					if (beq.waiting_time < instr_table[j].write_time)
						beq.waiting_time = instr_table[j].write_time + 1;
				}
				if (division.Qk == instr_table[j].rs_name)
				{

					division.Vk = division.result;
					division.Qk = "";
					if (division.waiting_time < instr_table[j].write_time)
						division.waiting_time = instr_table[j].write_time + 1;
				}
				if (jalr_ret.Qk == instr_table[j].rs_name)
				{
					jalr_ret.Vk = division.result;
					jalr_ret.Qk = "";
					if (jalr_ret.waiting_time < instr_table[j].write_time)
						jalr_ret.waiting_time = instr_table[j].write_time + 1;
				}

				division.Busy = 0;
				instr_table[j].write = 1;
				write_flag = true;
				wb_flag[j] = true;  // instruction finished
				

			}
		}

	}
 }

double branch_misprediction_rate(int beq_counter, int mispredicted_count)
{
	if (beq_counter != 0)
	{
		double rate = (mispredicted_count / beq_counter) * 100;
		return rate;
	}
	else return 0.0;
}

void printTable(vector<instr> inst_table)
{
	cout << endl;
	cout << "Issue time    Execution time     Write time\n";
	for (int i = 0; i < inst_table.size(); i++)
	{
		cout << inst_table[i].issue_time << "                 " << inst_table[i].exec_time_start << ":" << inst_table[i].exec_time_end << "                " << inst_table[i].write_time << endl;
	}
	cout << endl;
	cout << "Total execution time: " << execution_time<<endl;
	cout << "IPC: " << IPC << endl;
	cout << "Branch misprediction percentage: " << branch_misprediction_rate(beq_counter,  mispredicted_count) << endl;




}