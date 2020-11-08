#include <bits/stdc++.h>
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
bool isRType(string opcode){
  return opcode=="add" || opcode=="sub"  || opcode=="mul";
}

bool isIType(string opcode){
  return opcode=="addi" || opcode=="lw" || opcode=="sw" || opcode=="beq";
}

bool isAValidCharacter(char c){
  return isalnum(c) || c == '$' || c == ',' || c == '-' || c == '(' || c == ')';
}

bool isAValidReg(char c){
  return isalnum(c) || c == '$';
}

string trimInstruction(string instruction){
  int stage = 0;
  string temp;
  int i;
  bool lastCharWasSpace = true;
  for(i = 0; i<sz(instruction); i++){
    if(isAValidCharacter(instruction[i])){ 
      temp.push_back(instruction[i]);
      lastCharWasSpace = false;
    }else{
      if(!lastCharWasSpace){
        stage++;
        if(stage == 1){
          temp.push_back(' ');
        }
      }
      lastCharWasSpace = true;
    }
  }
	return temp;
}




string extractOpcode(string instruction){
  string opcode;
  int i;
  for(i = 0; instruction[i] != ' '&&i<sz(instruction); i++){
    opcode.push_back(tolower(instruction[i]));
  }
  return opcode;
}


int regValue(string c){
   string rega;
  fo(i,sz(c))
	{
		if(c[i]!='$')
			rega.push_back(c[i]);
	}
c= rega;
  if(isalpha(c[0])){
    int regIndex = c[1] - '0';
    switch(c[0]){
      case 'a':
        if(regIndex >= 0 && regIndex <= 3 ){
          return 4 + regIndex;
        }else if(c[1] == 't'){
          return 1;
        }
        break;
      case 'g':
        if(c[1] == 'p'){
          return 28;
        }
        break;
      case 'k':
        if(regIndex >= 0 && regIndex <= 1 ){
          return 26 + regIndex;
        }
        break;
      case 'r':
        if(c[1] == 'a'){
          return 31;
        }
        break;
      case 's':
        if(regIndex >= 0 && regIndex <= 7 ){
          return 16 + regIndex;
        }else if(regIndex == 8){
          return 30;
        }else if(c[1] == 'p'){
          return 29;
        }
        break;
      case 't':
        if(regIndex >= 0 && regIndex <= 7 ){
          return 8 + regIndex;
        }else if(regIndex >= 8 && regIndex <= 9 ){
          return 16 + regIndex;
        }
        break;
      case 'v':
        if(regIndex >= 0 && regIndex <= 1 ){
          return 2 + regIndex;
        }
        break;
      case 'z':
        if(c=="zero"){
          return 0;
        }
        break;
    }
  }else{
    /*If it doesn't have a letter in it's first position, we have to check if it uses the $# format
      First we check if all the characters after the dollar sign are numbers
      If they are, then we convert it to a number and check if its between 0 and 31
      Otherwise we just fail it (return -1)
      */
    int i = 0;
    char currentChar = c[i];
    while(currentChar != '\0'){
      if(!isdigit(currentChar)){
        return -1;
      }
      currentChar = c[++i];
    }
    //If we've made it here, the string is a number
    int regIndex = stoi(c);
    if(regIndex >= 0 && regIndex <= 31){
      return regIndex;
    }else{
      assert(!"Register number out of bounds");
    }
  }

  return -1;
}



int extractRegister(string instruction, int index){
  int i;
  int regIndex = 0;
  bool readOpcode = false;
  string reg; 

  for(i = 0; i<sz(instruction); i++){
    if(readOpcode && instruction[i] == ','){
      regIndex++;
    }
    if(readOpcode && isAValidReg(instruction[i]) && index == regIndex){
      reg.push_back(instruction[i]);
    }
    if(instruction[i] == ' '){
      readOpcode = true;
    }
  }
  //
  //Checking if it's a valid register
  if(reg[0] != '$'){
    assert(!"Register didn't start with a dollar sign");
  }
  //Set regValue to  the string without the dollar sign
  int regVal = regValue(reg);
  //Register is invalid
  if(regVal == -1){
    assert(!"Unrecognized register name");
  }
  return regVal;
}

int extractBase(string instruction){
  int i;
  int regIndex = 0;
  bool readOpenParen = false;
  bool readCloseParen = false;
  string reg;

  for(i = 0;i<sz( instruction); i++){
    if(readOpenParen && !readCloseParen){
      reg.push_back(instruction[i]);
    }
    if(instruction[i] == '('){
      readOpenParen = true;
    }
    if(instruction[i] == ')'){
      readCloseParen = true;
    }
  }

  if(!readCloseParen || !readOpenParen){
    assert(!"Invalid parentheses");
  }
  if(reg[0] != '$'){
    assert(!"Missing dollar sign on offset");
  }
  return regValue(reg);
}

int extractImmediate(string instruction,int index){
  int i;
  int regIndex = 0;
  bool readOpcode = false;
  string reg; //To be able to hold $zero, which is 5 characters and the null character

  for(i = 0; i<sz(instruction) && instruction[i] != '('; i++){
    if(readOpcode && instruction[i] == ','){
      regIndex++;
    }
    if(readOpcode && instruction[i] != ',' && regIndex == index){
      reg.push_back(instruction[i]);
    }
    if(instruction[i] == ' '){
      readOpcode = true;
    }
  }
  //Make sure everything is a digit, or its the first character and its -
  for(i = 0; i<sz(reg); i++){
    if(!isdigit(reg[i])){
      if(!(i == 0 && reg[i] == '-')){
        assert(!"Immediate field contained incorrect value");
      }
    }
  }
  int imm = stoi(reg);
  if(imm > 32767 || imm < -32768){
    assert(!"Immediate field too large");
  }
  return imm;
}

