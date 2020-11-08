#include <bits/stdc++.h>
#include "otherclasses.cc"
#define pp pair<ll,ll>
#define ll long long int
#define tr(a,x) for(auto a:x)
#define tri(a,x) for(auto a=x.begin();a!=x.end();a++)
#define all(x) x.begin(),x.end()
#define sz(x) (int)(x).size()
#define fo(i,x) for(int i=0;i<x;i++)
#define p_b(x,i) x.push_back(i)
#define printe(i) cout<<i<<" "
#define printed(x,i) cout<< x  <<i<<"\n"
#define ip(s) vector <s>vec;for(int i=0;i<n;i++){s tmp;cin>>tmp;vec.push_back(tmp);}
#define op(s) for(auto i:s) cout<<i<<" ";cout<<"\n";
#define vi vector<ll>
#define vpp vector<pair<ll,ll>>
#define newl cout<<"\n"
#define opa(s) tr(a,s){op(a);newl;}
#define C 100
#define N 10
#define M 15
using namespace std;
class singlecycle_simulator
{
	public:
	int registers[32];
	int data[512];
	void execute(instructions inst,prog &p)
	{
		if(inst.type=='R')
		{
			if(inst.rs>31||inst.rt>31||inst.dest>31)
			{
				assert(!"Invalid register location");
			}
			if(inst.op=="add")
					registers[inst.dest]=registers[inst.rs]+registers[inst.rt];
			else if(inst.op=="sub")
					registers[inst.dest]=registers[inst.rs]-registers[inst.rt];
			else if(inst.op=="mul")
					registers[inst.dest]=registers[inst.rs]*registers[inst.rt];
			else
			{
				inst.print();
				assert(!" wrong opcode r type");
			}
		}
		else
		{
			if(inst.rs>31)
			{	
				assert(!"Invalid register location");
			}
			if(inst.op== "addi")
				registers[inst.dest]=registers[inst.rs]+inst.i;
			else if(inst.op=="lw")
			{
				int y;
				if(inst.i % 4 == 0)
				{
					y = registers[inst.rs] + inst.i/4;
				}
				else
				{
					assert(!"Misaligned memory access");
				}
				if(&data[y] == NULL)
				{
					assert(!"Read from invalid data memory location");
				}
				registers[inst.rt] = data[y];
			}
			else if(inst.op=="sw")
			{
				int y;
				if(inst.i % 4 == 0)
				{
					y = registers[inst.rs] + inst.i/4;
				}
				else
				{
					assert(!"Misaligned memory access");
				}
				if(&data[y] == NULL)
				{
					assert(!"Read from invalid data memory location");
				}
				data[y]=registers[inst.rt];
			}
			else if(inst.op=="beq")
			{
				if(registers[inst.rs] == registers[inst.rt])
				{
					p.pc= p.pc + inst.i;
					if(p.pc >sz(p.instlist)-1)
					{
						assert(!"Branched out of program");
					}
				}
			}
			else
			{
				inst.print();
				assert(!"wrong opcode");
			}
		}
	}
	void simulate(prog p)
	{
		using namespace std::chrono; 
		auto start = high_resolution_clock::now();
		fo(i,sz(p.instlist)-1)
		{
			instructions tmp=p.instlist[i];
			execute(tmp,p);
		}
		auto stop = high_resolution_clock::now(); 
		auto duration = duration_cast<microseconds>(stop - start); 
		cout<<"total cycles is "<<sz(p.instlist)-1<<"\n";
		cout<<"time taken is "<<duration.count()<<" microseconds\n";
	}
};

class pipelined_simulator
{
	public:
	latch l1,l2,l3,l4;
	int registers[32];
	int data[512];
	int rawHazard(prog &p)
	{
		instructions inst = l1.inst;
		if(l1.inst.type != 'B')
		{
			if(l2.warmed_up && inst.rs == l2.inst.dest && l2.inst.op != "sw")
			{
				if(inst.rs != 0)
				{
					p.hazard=p.hazard+1;
					return inst.rs;
				}
			}

			if(l3.warmed_up && inst.rs == l3.inst.dest && l3.inst.op != "sw")
			{
				if(inst.rs != 0)
				{
					return inst.rs;
				}
			}

			if(l4.warmed_up && inst.rs == l4.inst.dest && l4.inst.op != "sw")
			{
				if(inst.rs != 0)
				{
					return inst.rs;
				}
			}


			if(inst.type == 'R' || inst.op == "beq")
			{
				if(l2.warmed_up && inst.rt == l2.inst.dest && l2.inst.op != "sw")
				{
					if(inst.rt != 0)
					{
						p.hazard=p.hazard+1;	
						return inst.rt;
					}
				}

				if(l3.warmed_up && inst.rt == l3.inst.dest && l3.inst.op != "sw")
				{
					if(inst.rt != 0)
					{
						return inst.rt;
					}
				}

				if(l4.warmed_up && inst.rt == l4.inst.dest && l4.inst.op != "sw")
				{
					if(inst.rt != 0)
					{
						return inst.rt;
					}
				}
			}
		}
		return -1;
	}

	void fetch(prog &p)
	{
		if(!p.branchpending)
		{
			if(!l1.valid)
			{
				p.totalcycles++;
				//cout<<"executed fetch\n";
				//cout<<"pc is "<<p.pc<<"\n";
				l1.valid=true;
				l1.inst=p.instlist[p.pc];
				//l1.inst.print();
				if(p.pc<sz(p.instlist)-1)
				{
					//cout<<"incremented pc\n";
					p.pc=p.pc+1;
				}
				p.ifutil++;
				if(!l1.warmed_up)
				{
					l1.warmed_up=true;
				}
			}
		}

	}
	void decode(prog &p)
	{
		if(l1.valid && l1.warmed_up && !l2.valid)
		{
			p.totalcycles++;
			//cout<<"executed decode\n";
			//l1.inst.print();
			if(rawHazard(p) == -1)//change this function
			{
				if(l1.inst.op == "beq")
				{
					p.branchpending = true;
				}
				l1.valid = false;
				l2.valid = true;
				l2.inst = l1.inst;
				if(l2.inst.type != 'B')
				{
					p.idutil++;
				}
				if(!l2.warmed_up)
				{
					l2.warmed_up = true;
				}
			}
			else
			{
				l2.valid = true;
				l2.inst = bubble;
			}
		}
	}
	void execute(prog &p)
	{
		if(l2.warmed_up && l2.valid)
		{
			p.totalcycles++;
			//cout<<"executed execute\n";
			//l2.inst.print();
			static int e_cycles = 0;
			if(l2.inst.type == 'B')
			{
				if(!l3.valid)
				{
					l2.valid = false;
					l3.valid = true;
					l3.inst = l2.inst;
				}
			}
			else
			{
				if(!l3.valid  && ((e_cycles == M && l2.inst.op == "mul") || (e_cycles == N && l2.inst.op != "mul")) )
				{
					if(l2.inst.rs > 31 || l2.inst.rt > 31)
					{
						cout<<l2.inst.print()<<"\n";
						assert(!"Invalid register location");
					}
					if(l2.inst.op == "add")
					{
						l3.data = registers[l2.inst.rs] + registers[l2.inst.rt];
					}
					else if(l2.inst.op == "addi")
					{
						l3.data = registers[l2.inst.rs] + l2.inst.i;
					}
					else if(l2.inst.op == "sub")
					{
						l3.data = registers[l2.inst.rs] - registers[l2.inst.rt];
					}
					else if(l2.inst.op == "mul")
					{
						l3.data = registers[l2.inst.rs] * registers[l2.inst.rt];
					}
					else if(l2.inst.op  == "beq")
					{
						if(registers[l2.inst.rs] == registers[l2.inst.rt])
						{
							p.pc= p.pc + l2.inst.i;
							if(p.pc >sz(p.instlist)-1)
							{
								assert(!"Branched out of program");
							}
						}
						p.branchpending = false;
					}
					else if(l2.inst.op == "lw" || l2.inst.op == "sw")
					{
						if(l2.inst.i % 4 == 0)
						{
							l3.data = registers[l2.inst.rt];
							l3.inst.dest = registers[l2.inst.rs] + l2.inst.i/4;
						}
						else
						{
							assert(!"Misaligned memory access");
						}
					}
					else 
					{
						cout<<"instruction is\n";
						l2.inst.print();
						cout<<" opcode is "<<l2.inst.op<<"\n";
						assert(!"Unrecognized instruction");
					}
					e_cycles = 0;
					l2.valid = false;
					l3.valid = true;
					l3.inst = l2.inst;
					if(!l3.warmed_up)
					{
						l3.warmed_up = true;
					}
				}
				else if((e_cycles < M && l2.inst.op == "mul") || (e_cycles < N && l2.inst.op != "mul"))
				{
					e_cycles++;
				}
				p.exutil++;
			}	
		}
	}

	void mem(prog &p)
	{
		if(l3.warmed_up && l3.valid)
		{
			p.totalcycles++;
			//cout<<"executed mem\n";
			//l3.inst.print();
			if(l3.inst.type == 'B')
			{
				if(!l4.valid)
				{
					l3.valid = false;
					l4.valid = true;
					l4.inst = l3.inst;
				}
			}
			else
			{
				static int m_cycles = 0;
				bool is_lw = l3.inst.op == "lw";
				bool is_sw = l3.inst.op == "sw";
				if(is_lw || is_sw)
				{
					if(m_cycles == C && !l4.valid)
					{
						m_cycles = 0;
						l3.valid = false;
						l4.valid = true; 
						l4.inst = l3.inst;
						if(!l4.warmed_up)
						{
							l4.warmed_up = true;
						}
						if(is_lw)
						{
							if(&data[l3.data] == NULL)
							{
								assert(!"Read from invalid data memory location");
							}
							l4.data = data[l3.inst.dest];
						}
						if(is_sw)
						{
							if(&data[l3.data] == NULL)
							{
								assert(!"Attempted to write to invalid data memory location");
							}
							data[l3.inst.dest] = l3.data;
						}
					}
					else if(m_cycles < C)
					{
						m_cycles++;
					}
				}
				else
				{
					l3.valid = false;
					l4.valid = true; 
					l4.inst = l3.inst;
					l4.data = l3.data;
					if(!l4.warmed_up)
					{
					l4.warmed_up = true;
					}
				}
				if(l3.inst.type != 'B')
				{
					p.memutil++;
				}
			}
		}
	}  

	void wb(prog &p)
	{
		if(l4.valid && l4.warmed_up)
		{
			p.totalcycles++;
			//cout<<"executed wb\n";
			//l4.inst.print();
			if(l4.inst.op != "sw" && l4.inst.op != "beq" && l4.inst.op != "haltsimulation" && l4.inst.dest != 0 && l4.inst.type != 'B')
			{
				registers[l4.inst.dest] = l4.data;
				p.wbutil++;
			}   
			if(l4.inst.type == 'B' && l4.inst.op=="haltsimulation")
			{
				p.totally_done = true;
			}
			l4.valid = false; 
		}
	}
	void simulate(prog p,bool print,bool regout)
	{
		int totalcycles=0;
		ofstream co;
		co.open("steps.txt");
		while(!p.totally_done)
		{
			if(regout)
			{
				co<<"NEXT Instructions in \n";
				co<<"Fetch state\n";
				co<<p.instlist[p.pc].print();
				if(l1.warmed_up&&l1.valid)
				{
					co<<"Decode state\n";
					co<<l1.inst.print();
				}
				if(l2.warmed_up&&l2.valid)
				{
					co<<"Execute state\n";
					co<<l2.inst.print();
				}
				if(l3.warmed_up&&l3.valid)
				{					
					co<<"Memory state\n";
					co<<l3.inst.print();
				}
				if(l4.warmed_up&&l4.valid)
				{
					co<<"WriteBack state\n";
					co<<l4.inst.print();
				}
				co<<"\nRegisters\n";				
				int i;
				for(i = 0; i < 32; i++)
				{
					co<<"Register $"<<i<<": "<<registers[i]<<"\n";
				}
				co<<"\n----------------------------------\n";				
			}
			wb(p);
			mem(p);
			execute(p);
			decode(p);
			fetch(p);
			//cout<<"cycle done\n";
			totalcycles++;
		}
		co.close();
		cout<<"total no of instruction"<<p.count()<<"\n";
		cout<<"number of hazards "<<p.hazard<<"\n";
		//cout<<"total cycles is "<<p.totalcycles<<"\n";
		cout<<"CPI "<<p.cpi(false)<<"\n";
		cout<<"total number of latches 3	\n";
		if(print)
		{
			cout<<"Instruction Fetch Utilization"<<1.0*p.ifutil/totalcycles*100<<"\n";
			cout<<"Instruction Decode Utilization"<<1.0*p.idutil/totalcycles*100<<"\n";
			cout<<"Execute Utilization"<<1.0*p.exutil/totalcycles*100<<"\n";
			cout<<"Memory Utilization"<<1.0*p.memutil/totalcycles*100<<"\n";
			cout<<"Write Back Utilization"<<1.0*p.wbutil/totalcycles*100<<"\n";
		}
	}
};
class latch1 
{
	public:
		latch1()
		{
			warmed_up=false;
		}
	int instructioncount,tmp;
	
	bool valid;
	int data;
	bool warmed_up;
	instructions inst;
};
class r4000_pipelined_simulator
{
	public:
	latch1 l1,l2,l3,l4,l5,l6,l7;
	int registers[32];
	int data[512];
	int rawHazard(prog &p)
	{
		instructions inst = l2.inst;
		if(l2.inst.type != 'B')
		{
			if(l3.warmed_up && inst.rs == l3.inst.dest && l3.inst.op != "sw")
			{
				if(inst.rs != 0)
				{
					p.hazard=p.hazard+1;
					return inst.rs;
				}
			}

			if(l4.warmed_up && inst.rs == l4.inst.dest && l4.inst.op != "sw")
			{
				if(inst.rs != 0)
				{
					p.hazard=p.hazard+1;
					return inst.rs;
				}
			}

			if(l7.warmed_up && inst.rs == l7.inst.dest && l7.inst.op != "sw")
			{
				if(inst.rs != 0)
				{
					return inst.rs;
				}
			}


			if(inst.type == 'R' || inst.op == "beq")
			{
				if(l3.warmed_up && inst.rt == l3.inst.dest && l3.inst.op != "sw")
				{
					if(inst.rt != 0)
					{
						p.hazard=p.hazard+1;
						return inst.rt;
					}
				}

				if(l4.warmed_up && inst.rt == l4.inst.dest && l4.inst.op != "sw")
				{
					if(inst.rt != 0)
					{
						p.hazard=p.hazard+1;
						return inst.rt;
					}
				}

				if(l7.warmed_up && inst.rt == l7.inst.dest && l7.inst.op != "sw")
				{
					if(inst.rt != 0)
					{
						return inst.rt;
					}
				}
			}
		}
		return -1;
	}
	void ifs(prog &p)
	{
		//cout<<"hi1\n";
		if(!p.branchpending)
		{
			if(!l1.valid)
			{
				p.totalcycles++;
				//int add=p.pc;
				//instructoon cache 100 % there fore no miss
				if(p.pc<sz(p.instlist))
				{
					l1.instructioncount=p.pc;
					//cout<<"incremented pc\n";
					p.pc=p.pc+1;
					l1.valid=true;
					if(!l1.warmed_up)
						l1.warmed_up=true;

				}
			}
		}
	}
	void is(prog &p)
	{
		//cout<<"hi2\n";
		if(!p.branchpending)
		{
			if(l1.valid &&l1.warmed_up&& !l2.valid)
			{
				p.totalcycles++;
				//cout<<"executed fetch\n";
				//cout<<"pc is "<<p.pc<<"\n";
				l1.valid=false;
				l2.valid=true;
				l2.inst=p.instlist[l1.instructioncount];
				//l1.inst.print();
				//p.ifutil++;
				if(!l2.warmed_up)
				{
					l2.warmed_up=true;
				}
			}
		}

	}
	void rf(prog &p)
	{
		//cout<<"hi3\n";
		if(l2.valid && l2.warmed_up && !l3.valid)
		{
			p.totalcycles++;
			//cout<<"executed decode\n";
			//l1.inst.print();
			if(rawHazard(p) == -1)//change this function
			{
				if(l2.inst.op == "beq")
				{
					p.branchpending = true;
				}
				l2.valid = false;
				l3.valid = true;
				l3.inst = l2.inst;
				//if(l3.inst.type != 'B')
				//{
				//	p.idutil++;
				//}
				if(!l3.warmed_up)
				{
					l3.warmed_up = true;
				}
			}
			else
			{
				l3.valid = true;
				l3.inst = bubble;
			}
		}
	}
	void execute(prog &p)
	{
				//cout<<"hi4\n";
		if(l3.warmed_up && l3.valid)
		{
			p.totalcycles++;
			//cout<<"executed execute\n";
			//l3.inst.print();
			if(l3.inst.type == 'B')
			{
				if(!l4.valid)
				{
					l3.valid = false;
					l4.valid = true;
					l4.inst = l3.inst;
				}
			}
			else
			{
				if(!l4.valid )
				{
					if(l3.inst.rs > 31 || l3.inst.rt > 31)
					{
						assert(!"Invalid register location");
					}
					if(l3.inst.op == "add")
					{
						l4.data = registers[l3.inst.rs] + registers[l3.inst.rt];
					}
					else if(l3.inst.op == "addi")
					{
						l4.data = registers[l3.inst.rs] + l3.inst.i;
					}
					else if(l3.inst.op == "sub")
					{
						l4.data = registers[l3.inst.rs] - registers[l3.inst.rt];
					}
					else if(l3.inst.op == "mul")
					{
						l4.data = registers[l3.inst.rs] * registers[l3.inst.rt];
					}
					else if(l3.inst.op  == "beq")
					{
						if(registers[l3.inst.rs] == registers[l3.inst.rt])
						{
							p.pc= p.pc + l3.inst.i;
							if(p.pc >sz(p.instlist)-1)
							{
								assert(!"Branched out of program");
							}
						}
						p.branchpending = false;
					}
					else if(l3.inst.op == "lw" || l3.inst.op == "sw")
					{
						if(l3.inst.i % 4 == 0)
						{
							l4.data = registers[l3.inst.rt];
							l4.inst.dest = registers[l3.inst.rs] + l3.inst.i/4;
						}
						else
						{
							assert(!"Misaligned memory access");
						}
					}
					else 
					{
						cout<<"instruction is\n";
						l3.inst.print();
						cout<<" opcode is "<<l3.inst.op<<"\n";
						assert(!"Unrecognized instruction");
					}
					//e_cycles = 0;
					l3.valid = false;
					l4.valid = true;
					l4.inst = l3.inst;
					if(!l4.warmed_up)
					{
						l4.warmed_up = true;
					}
					//cout<<"set true\n";
					
				}
				//p.exutil++;
			}	
		}
	}
	void df(prog &p)
	{
		//cout<<"hi5\n";
		if(l4.warmed_up && l4.valid)
		{
			p.totalcycles++;
			//cout<<"executed mem\n";
			//l6.inst.print();
			if(l4.inst.type == 'B')
			{
				if(!l5.valid)
				{
					l4.valid = false;
					l5.valid = true;
					l5.inst = l4.inst;
				}
			}
			else
			{
				//static int m_cycles = 0;
				bool is_lw = l4.inst.op == "lw";
				if(is_lw)
				{
					if(!l5.valid)
					{
						//m_cycles = 0;
						l4.valid = false;
						l5.valid = true; 
						l5.inst = l4.inst;
						if(!l5.warmed_up)
						{
							l5.warmed_up = true;
						}
						if(is_lw)
						{
							if(&data[l4.data] == NULL)
							{
								assert(!"Read from invalid data memory location");
							}
							l5.tmp = l4.inst.dest;
						}
					}
				}
				else
				{
					l4.valid = false;
					l5.valid = true; 
					l5.inst = l4.inst;
					l5.data = l4.data;
					if(!l5.warmed_up)
					{
					l5.warmed_up = true;
					}
					l5.tmp= l4.tmp;
				}
				//if(l6.inst.type != 'B')
				//{
				//	p.memutil++;
				//}
			}
		}
	
	}
	void ds(prog &p)
	{
		//cout<<"hi6\n";
		if(l5.warmed_up && l5.valid)
		{
			p.totalcycles++;
			//cout<<"executed mem\n";
			//l6.inst.print();
			if(l5.inst.type == 'B')
			{
				if(!l6.valid)
				{
					l5.valid = false;
					l6.valid = true;
					l6.inst = l5.inst;
				}
			}
			else
			{
				//static int m_cycles = 0;
				bool is_lw = l5.inst.op == "lw";
				if(is_lw)
				{
					if(!l6.valid)
					{
						//m_cycles = 0;
						l5.valid = false;
						l6.valid = true; 
						l6.inst = l5.inst;
						if(!l6.warmed_up)
						{
							l6.warmed_up = true;
						}
						if(is_lw)
						{
							if(&data[l5.data] == NULL)
							{
								assert(!"Read from invalid data memory location");
							}
							l6.tmp = data[l5.tmp];
						}
					}
				}
				else
				{
					l5.valid = false;
					l6.valid = true; 
					l6.inst = l5.inst;
					l5.tmp= l4.tmp;
					l6.data = l5.data;
					if(!l6.warmed_up)
					{
					l6.warmed_up = true;
					}
				}
				//if(l6.inst.type != 'B')
				//{
				//	p.memutil++;
				//}
			}
		}
	}
	void tc(prog &p)
	{
		//cout<<"hi7\n";
		int x=rand()%20;
		if(x!=1)//condition for cache miss
		{
			if(l6.warmed_up && l6.valid)
			{
				p.totalcycles++;
				//cout<<"executed mem\n";
				//l6.inst.print();
				if(l6.inst.type == 'B')
				{
					if(!l7.valid)
					{
						l6.valid = false;
						l7.valid = true;
						l7.inst = l6.inst;
					}
				}
				else
				{
					bool is_lw = l6.inst.op == "lw";
					bool is_sw = l6.inst.op == "sw";
					if(is_lw || is_sw)
					{
						if(!l7.valid)
						{
							//m_cycles = 0;
							l6.valid = false;
							l7.valid = true; 
							l7.inst = l6.inst;
							if(!l7.warmed_up)
							{
								l7.warmed_up = true;
							}
							if(is_lw)
							{
								if(&data[l6.data] == NULL)
								{
									assert(!"Read from invalid data memory location");
								}
								l7.data = l6.tmp;
							}
							if(is_sw)
							{
								if(&data[l6.data] == NULL)
								{
									assert(!"Attempted to write to invalid data memory location");
								}
								data[l6.inst.dest] = l6.data;
							}
						}
					}
					else
					{
						l6.valid = false;
						l7.valid = true; 
						l7.inst = l6.inst;
						l7.data = l6.data;
						if(!l7.warmed_up)
						{
						l7.warmed_up = true;
						}
						//cout<<"set\n";
					}
					//if(l6.inst.type != 'B')
					//{
					//	p.memutil++;
					//}
				}
			}
		}
		else
		{
			p.hazard=p.hazard+1;
		}
	}  

	void wb(prog &p)
	{
		//cout<<"hi8\n";
		if(l7.valid && l7.warmed_up)
		{
			p.totalcycles++;
			//cout<<"executed wb\n";
			//l7.inst.print();
			if(l7.inst.op != "sw" && l7.inst.op != "beq" && l7.inst.op != "haltsimulation" && l7.inst.dest != 0 && l7.inst.type != 'B')
			{
				registers[l7.inst.dest] = l7.data;
				//p.wbutil++;
			}   
			if(l7.inst.type == 'B' && l7.inst.op=="haltsimulation")
			{
				p.totally_done = true;
			}
			l7.valid = false; 
		}
	}
	void simulate(prog p,bool print,bool regout)
	{
		int totalcycles=0;
		ofstream co;
		co.open("steps.txt");
		//cout<<"hi\n";
		while(!p.totally_done)
		{
			if(regout)
			{
				co<<"NEXT Instructions in \n";
				if(l1.warmed_up&&l1.valid)
				{
					co<<"First Fetch state\n";
					co<<p.instlist[l1.instructioncount].print();
				}
				////cout<<"hi\n";				
				if(l2.warmed_up&&l2.valid)
				{
					co<<"Register Decode state\n";
					co<<l2.inst.print();
				}
				if(l3.warmed_up&&l3.valid)
				{					
					co<<"Execute state\n";
					co<<l3.inst.print();
				}

				if(l4.warmed_up&&l4.valid)
				{
					co<<"Data Fetch first state\n";
					co<<l4.inst.print();
				}

				if(l5.warmed_up&&l5.valid)
				{
					co<<"Data Fetch Second state\n";
					co<<l5.inst.print();
				}

				if(l6.warmed_up&&l6.valid)
				{
					co<<"Tag Check state\n";
					co<<l6.inst.print();
				}

				if(l7.warmed_up&&l7.valid)
				{
					co<<"WriteBack state\n";
					co<<l7.inst.print();
				}
				co<<"\nRegisters\n";				
				int i;
				for(i = 0; i < 32; i++)
				{
					co<<"Register $"<<i<<": "<<registers[i]<<"\n";
				}
				co<<"\n----------------------------------\n";	
				co.flush();			
				////cout<<"hi\n";
			}	
			wb(p);
			tc(p);
			ds(p);
			df(p);
			execute(p);
			rf(p);
			is(p);
			ifs(p);
			//cout<<"cycle done\n";
			totalcycles++;
		}
		cout<<"total no of instruction "<<p.count()<<"\n";
		cout<<"number of hazards "<<p.hazard<<"\n";
		cout<<"CPI "<<p.cpi(true)<<"\n";
		//cout<<"total cycles is "<<p.totalcycles<<"\n";
		cout<<"total number of latches 7\n";
		co.close();
	}
};

