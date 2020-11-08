#include <bits/stdc++.h>
#include "simulatorclass.cc"

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

using namespace std;

inline bool exists(string& name) {
    ifstream f(name.c_str());
    return f.good();
}
int f1(prog p)
{
	cout<<"single cycle simulator\n";
//	prog p(fil);
	singlecycle_simulator sim;
	sim.simulate(p);
}
int f2(prog p)
{
	cout<<"\n\nMIPS multicycle cycle simulator\n";
	pipelined_simulator sim2;
	sim2.simulate(p,false,false);
}
int f3(prog p)
{
	cout<<"\n\nMIPS r4000 cycle simulator\n";
//	prog p3(fil);
//	cout<<p.print();
	r4000_pipelined_simulator sim3;
	sim3.simulate(p,false,false);
}
int main(int arc,char *argv[])
{
	if(arc==2)
	{
		string fil(argv[1]);
		prog p(fil);
		if(!exists(fil))
		{
			cout<<"file doesnt exist\n";
			exit(0);
		}
		f1(p);f2(p);f3(p);
		return 0;
	}
	if(arc==3||arc==4)
	{
		string fil(argv[1]);
		if(!exists(fil))
		{
			cout<<"file doesnt exist\n";
			exit(0);
		}
		prog p(fil);
		//cout<<"["<<argv[2]<<"]"<<"\n";
		string a2(argv[2]);
		string a3(argv[3]);
		if(a2=="0")
		{
			cout<<"single cycle simluator\n";
			singlecycle_simulator sim;
			sim.simulate(p);
		}
		//delete &sim1;
		//delete &p1;
		else
		{
			if(a2=="1")
			{
				cout<<"MIPS multicycle simluator\n";
				//prog p2(fil);
				pipelined_simulator sim;
				if(a3=="n")
					sim.simulate(p,false,false);
				else
					sim.simulate(p,false,true);
			}
			//delete &sim2;
			//delete &p2;
			else
			{
				cout<<"MIPS R4000 cycle simluator\n";
				//prog p3(fil);
				r4000_pipelined_simulator sim;
				if(a3=="n")
					sim.simulate(p,false,false);
				else
					sim.simulate(p,false,true);
			}
		}
		return 0;
	}
	cout<<"enter the file name for program\n";
	string fil;
	cin>>fil;
	if(!exists(fil))
	{
		cout<<"file doesnt exist\n";
		exit(0);
	}
	prog p(fil);
	//prog1.print();
	cout<<"Do you want single cycle simulator(0) or MIPS multicycle/pipelined (1) or R4000 MIPS Pipelined(2)?\n";
	int ch;
	cin>>ch;
	if(ch==0)
	{
		singlecycle_simulator sim;
		sim.simulate(p);
	}
	else
	{
		if(ch==1)
		{
			pipelined_simulator sim;
			cout<<"do you want to print each step status to a file[steps.txt]?(y/n)\n";
			char x;
			cin>>x;
			if(x=='n')
				sim.simulate(p,false,false);
			else
				sim.simulate(p,false,true);
		}
		else
		{
			r4000_pipelined_simulator sim;
			//cout<<p.print();
			cout<<"do you want to print each step status to a file[steps.txt]?(y/n)\n";
			char x;
			cin>>x;
			if(x=='n')
				sim.simulate(p,false,false);
			else
				sim.simulate(p,false,true);
		}
	}
	//pipelined_simulator sim1;
	//sim1.simulate(prog1,true,true);
	//singlecycle_simulator sim2;
	//sim2.simulate(prog1);
	return 0;
}


