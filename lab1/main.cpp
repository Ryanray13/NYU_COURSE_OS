#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <algorithm>
#include <cstdlib>
using namespace std;

#define MachineSize 512
ifstream inFile;             //file input stream
int linenum=1;               //the line number
int lineoffset=1;            //offset of the line
int moduleBase=0;           
int moduleCount=0;          

vector<string> symTable;     //to store all the symbols 
vector<int> symAddr;         //to store all the symbol address
vector<int> symFlag;         //to store whether the symbol is multiple times defined in first pass or whether it is used in second pass
vector<int> moduleNum;       //to store the module number of each symbol;
vector<string> moduleSym;    //to store the symbols of each module before those symbols stored in the program symbol table.
vector<int> moduleSymAddr;   //to store the symbols address of each module in first pass and symbol index in symbol table during second pass
vector<int> warnings;        //to store warnings  

void dealSpace();            //to deal the space between token
int parseDefine();           // main function for two passes...
int parseUse();              //...
int parseInstr();
void parseDefine_2();
void parseUse_2();
void parseInstr_2();

int main(int argc, char* argv[])
{
	int flag=0;
	int i=0;
	inFile.open(argv[1],ios_base::binary);

	if(argc<2)
		cout<<"Argument Error!"<<endl;

   	if(!inFile){
		cout<<"Error! Cannot open the input file!"<<endl;
		inFile.close();
		return 1;	
	}

//**************************Pass one*******************************************************
	/*Each module consists three parts, if all three parts can be successfully parsed 
	  we go to the next module till the end of the file*/
    dealSpace();
	do{
       moduleCount++;
	   if(parseDefine())
          if(parseUse())
	         if(parseInstr())
				 flag=1;
	   if(flag==0)                             //flag equals to 0 means we encountered a error
	   {
		   inFile.close();
		   return 0;
	   }
	   flag=0;
	}while(!inFile.eof());
	
	//output warnings(rule 5) and the symbol table
	for(i=0;i<warnings.size();i+=4)
	{
		cout<<"Warning: Module "<<warnings[i]<<": "<<symTable[warnings[i+1]]<<" to big "<<warnings[i+2]
			   <<" (max="<<warnings[i+3]<<") assume zero relative"<<endl;
	}
	cout<<"Symbol Table"<<endl;
	for(i=0;i<symTable.size();i++)
	{
		cout<<symTable[i]<<"="<<symAddr[i];
		if(symFlag[i]==1)
		{
			cout<<" Error: This variable is multiple times defined; first value used";
			symFlag[i]=0;   //
		}
		cout<<endl;
		
	}
	cout<<endl;

//**************************Pass two*******************************************************
	inFile.clear();
    inFile.seekg(0,ios::beg);
    i=moduleCount;
	moduleCount=0;
	moduleBase=0;
    cout<<"Memory Map"<<endl;
    
	while(i-->0)
	{
		moduleCount++;
		parseDefine_2();
		parseUse_2();
		parseInstr_2();
	}

	//output warnings(rule 4)
	cout<<endl;
	for(i=0;i<symTable.size();i++)
	{
		if(symFlag[i]==0)
			cout<<"Warning: Module "<<moduleNum[i]<<": "<<symTable[i]<<" was defined but never used"<<endl;
	}

    inFile.close();
    return 0;
}

void _parseerror(int errcode) {
    static string errstr[] = {
        "NUM_EXPECTED",				// Number expect
        "SYM_EXPECTED",				// Symbol Expected
        "ADDR_EXPECTED",			// Addressing Expected
        "SYM_TOLONG",				// Symbol Name is to long
        "TO_MANY_DEF_IN_MODULE",    //...
        "TO_MANY_USE_IN_MODULE",
        "TO_MANY_INSTR",
	};
   
	cout<<"Parse Error line "<<linenum<<" offset "<<lineoffset<<": "<<errstr[errcode]<<endl;
 }

//deal with the space including '\n',' ','\t', and count the offset
void dealSpace()
{
	int lastoffset=lineoffset;
	char chTemp;
    chTemp=inFile.peek();
	while(chTemp==' '||chTemp=='\n'||chTemp=='\t')
    {
		inFile.get();
		if(chTemp=='\n')
		{		
			linenum++;
			lastoffset=lineoffset;                       //store the last correct offset in case we end file with a '\n'
		    lineoffset=1;
		}
		else
			lineoffset++;
		chTemp=inFile.peek();

	}
	if(chTemp==EOF)
	{
		inFile.get();
		if(lineoffset==1)
		{
			lineoffset=lastoffset;
		    linenum--;
		}
	}
}

//check whether the token is a valid symbol
int checkSym(string sym)
{
	if((sym[0]<=90 && sym[0]>=65) || ( sym[0]<=122 && sym[0]>=97) )
		for(int i=1;i<sym.length();i++)
		{
			if((sym[i]>=97 && sym[i]<=122 )||(sym[i]>=48 && sym[i]<=57) ||(sym[i]>=65 && sym[i]<=90))
			;
			else
				return 0;
		}
	else
		return 0;
	return 1;
}

//check whether the token is an integer
int checkInt(string token)
{
	for(int i=0;i<token.length();i++)
		if(token[i]<48 || token[i]>57)
			return 0;
	return 1;
}

//parse the define list part, succeed return 1 else return 0
int parseDefine()
{
	int defCount=0;
	int symaddr;
	string symbol;
	string token;
	
	//get the define count.
	if(inFile>>token && checkInt(token))
	{
		defCount=atoi(token.c_str());
		if(defCount>16)                 
		{
			_parseerror(4);
			return 0;
		}
		else
		    lineoffset += token.length();
	}
	else
	{
		_parseerror(0);
		return 0;
	}

    moduleSym.clear();
	moduleSymAddr.clear();                
	for(int i=0;i<defCount;i++)
	{
		//get the symbol
        dealSpace();
		if(inFile>>symbol && checkSym(symbol))
		{
			if(symbol.length()>16)                              
			{
				_parseerror(3);
				return 0;
			}
			else
				lineoffset += symbol.length();
		}
		else
		{
			_parseerror(1);
			return 0;
		}			
		
        //get the symbol address
	    dealSpace();
        if(inFile>>token && checkInt(token))
		{
			symaddr=atoi(token.c_str());
			lineoffset += token.length();
		}
		else
		{
			_parseerror(0);
			return 0;
		}

		//store the symbol in module symbol table(not the symbol table for the program since we cannot be sure whether it is multiple times defined)
		moduleSym.push_back(symbol);
		moduleSymAddr.push_back(symaddr);   
	}
	dealSpace();
	return 1;
}

//parse the use list
int parseUse()
{
    int useCount=0;
	string token;

    //get the use count
	if(inFile>>token && checkInt(token))
	{
		useCount=atoi(token.c_str());
		if(useCount>16)                                  
		{
			_parseerror(5);
			return 0;
		}
		else
			lineoffset += token.length();
	}
	else
	{
		_parseerror(0);
		return 0;
	}

	for(int i=0;i<useCount;i++)
	{
		//get the symbol and we do not care whether it is defined right now.
        dealSpace();
		if(inFile>>token && checkSym(token))
		{
			if(token.length()>16)
			{
				_parseerror(3);
				return 0;
			}
			else
				lineoffset += token.length();
		}
		else
		{
			_parseerror(1);
			return 0;
		}			
	}
	dealSpace();
	return 1;
}

//parse the program text part
int parseInstr()
{
    int codeCount=0;
	int i=0;
	string token;
	vector<string>::iterator v;

	//get the instruction count
	if(inFile>>token && checkInt(token))
	{
		codeCount=atoi(token.c_str());
		if(codeCount>MachineSize-moduleBase)
		{
			_parseerror(6);
			return 0;
		}
		else
			lineoffset += token.length();
	}
	else
	{
		_parseerror(0);
		return 0;
	}

	for(i=0;i<moduleSym.size();i++)
	{
		v=find(symTable.begin(),symTable.end(),moduleSym[i]); //find whether the symbol is already in the program symbol table
		if(symTable.end() != v)                               //when a symbol is defined multiple times
		{
			symFlag[v-symTable.begin()]=1;   
		}
		else                                                  //when first encounter a symbol
		{	
			symTable.push_back(moduleSym[i]);
			if(moduleSymAddr[i]>=codeCount)                   //check whether the symbol's value is larger then instruction count
			{
				warnings.push_back(moduleCount);              //store the warning information for rule 5
				warnings.push_back(symTable.size()-1);
				warnings.push_back(moduleSymAddr[i]);
				warnings.push_back(codeCount-1);
				moduleSymAddr[i]=0;
			}
     		symAddr.push_back(moduleSymAddr[i]+moduleBase);
			symFlag.push_back(0);
			moduleNum.push_back(moduleCount);
		}
	}
	moduleBase += codeCount;                                   //update the module base.

	for(i=0;i<codeCount;i++)
	{
		//get the Address type
        dealSpace();
		if(inFile>>token && (token=="R" || token == "E" || token =="I" || token == "A" ))
			lineoffset++;
		else
		{
			_parseerror(2);
			return 0;
		}
		
		//get the 4-digit number
		dealSpace();
        if(inFile>>token && checkInt(token))
			lineoffset += token.length();
		else
		{
			_parseerror(0);
			return 0;
		}
	}	
	dealSpace();
	return 1;
}


//second pass for define list
void parseDefine_2()
{
	int defCount;
	string token;
	inFile>>defCount;

	//just get the symbol and do nothing
	for(int i=0;i<defCount;i++)
	{
		inFile>>token;
		inFile>>token;
	}
}

//second pass for use list
void parseUse_2()
{
	moduleSym.clear();
	moduleSymAddr.clear();

	int useCount;
	string token;
	vector<string>::iterator v;

	//get the symbol that should be used
	inFile>>useCount;
	for(int i=0;i<useCount;i++)
	{
		inFile>>token;
		moduleSym.push_back(token);
		v=find(symTable.begin(),symTable.end(),token);      //find whether symbol appeared in the use list has been defined
		if(v != symTable.end())                             //if it's been defined, store the its index in symbol table
			moduleSymAddr.push_back(v-symTable.begin());
		else                                                //if it hasn't, store -1.
			moduleSymAddr.push_back(-1);
	}
}

void parseInstr_2()
{
	int codeCount,instr,operand,i;
	string addr;
	string token;
	
	vector<int> useFlag;                                    //store whether the symbol in uselist is used.
    for(i=0;i<moduleSym.size();i++)
		useFlag.push_back(0);

	inFile>>codeCount;
	for(i=0;i<codeCount;i++)
	{
		inFile>>token;
		inFile>>addr;
		instr=atoi(addr.c_str());
		operand=instr%1000;
		cout.width(3);
		cout.fill('0');
		cout<<i+moduleBase<<": ";
        cout.width(4);
		cout.fill('0');

		//deal with different errors.
		if(addr.length()>4)                           
		{
			if(token != "I")
				cout<<"9999"<<" Error: Illegal opcode; treated as 9999"<<endl;
			else
				cout<<"9999"<<" Error: Illegal immediate value; treated as 9999"<<endl;
		}
		else if(token=="R")
		{
			if(operand>=codeCount)
				cout<<instr-operand+moduleBase<<" Error: Relative address exceeds module size; zero used"<<endl;
			else
				cout<<instr+moduleBase<<endl;
		}
		else if(token=="E")
		{
			if(operand>=moduleSym.size())
				cout<<instr<<" Error: External address exceeds length of uselist; treated as immediate"<<endl;
			else if(moduleSymAddr[operand] == -1)
			{
				cout<<instr-operand<<" Error: "<<moduleSym[operand]<<" is not defined; zero used"<<endl;
				useFlag[operand]=1;
			}
			else 
			{
				cout<<instr-operand+symAddr[moduleSymAddr[operand]]<<endl;
			    useFlag[operand]=1;    
			    symFlag[moduleSymAddr[operand]]=1;
			}
		}
		else if(token=="A")
		{
			if(operand>=MachineSize)
				cout<<instr-operand<<" Error: Absolute address exceeds machine size; zero used"<<endl;
			else
				cout<<instr<<endl;
		}
		else
			cout<<instr<<endl;
	}

	//check whether all the symbol in uselist are used.
	for(i=0;i<moduleSym.size();i++)
	{
		if(useFlag[i]==0)
			cout<<"Warning: Module "<<moduleCount<<": "<<moduleSym[i]
			       <<" appeared in the uselist but was not actually used"<<endl;
	}
	moduleBase += codeCount;
}