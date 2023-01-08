	#include <iostream>
	#include <fstream>
	#include <string>
	#include <cstdlib>
	#include <cstring>
	#include <sstream>
	
	using namespace std;
	//check for type of instruction
	bool typeOfIns(string s ) {
		int size = s.length(); 
		char a[size];
		strcpy(a, s.c_str());
		for(int i=0;i<size;i++)
			if(a[i] == '@'){
	return true;
			break;}
	return false;
	}
	// chek for pre defined symbols
	bool checkPredefined(string b){
	if(b=="R0" ||  b=="R1" || b== "R2" || b== "R3" || b== "R4" || b== "R5" || b== "R6" || b== "R7" || b== "R8" || b== "R9" || b== "R10" || b== "R11" || b== "R12" || b== "R13" || b== "R14" || b=="R15" || b== "SP" || b== "LCL" || b== "ARG" || b== "THIS"|| b== "THAT" || b=="SCREEN" || b == "KBD")
		return true;
		return  false;
	}
	//convert predefined
	int predefined(string b){
	int num;
		if(b=="R0" || b== "SP") num = 0;
		else if(b=="R1" || b== "LCL") num = 1;
		else if(b=="R2" || b== "ARG") num = 2;
		else if(b=="R3" || b== "THIS") num = 3;
		else if(b=="R4"|| b== "THAT") num = 4;
		else if(b=="R5") num = 5;
		else if(b=="R6") num = 6;
		else if(b=="R7") num = 7;
		else if(b=="R8") num = 8;
		else if(b=="R9") num = 9;
		else if(b=="R10") num = 10;
		else if(b=="R11") num = 11;	
		else if(b=="R12") num = 12;
		else if(b=="R13") num = 13;	
		else if(b=="R14") num = 14;
		else if(b=="R15") num = 15;	
		else if(b=="SCREEN") num = 16384;	
		else if(b=="KBD") num = 24576;	
	return num;
	}
	
	
	// convert to string 
	string convertToString(string b) {
		int size = b.length();
		char a[size];
		strcpy(a, b.c_str());
		int i; 
		string s = ""; 
		if(typeOfIns(b)){
			for (i=0; i < size; i++){
			if(a[i] == ' ' || a[i] == '@');
			else if(a[i] == '/') break; 
			else   s = s + a[i]; 
		}
		}
		else{
			for (i = 0; i < size; i++)
			s = s + a[i]; 
		} 
			return s; 
		}
	//convert to A asm to maxhine ins
	void convertToBinary(int num,ofstream &out){
		int i, b[16];
		for(i=0;i<16;i++){
			b[i]= num%2;
			num=num/2;
		}
		for( i=i-1;i>=0;i--)
			out << b[i];
			out <<'\n';
	}
	
	//	Return maxhine code of A instruction
	void returnMachineInsOfA(string s, ofstream &out){
		int num;
		string b =  convertToString(s); 
		if(checkPredefined(b)){
			num = predefined(b);
		}
		else{
			stringstream ss; 
			ss << b;  
			ss >> num;
		}
		convertToBinary(num , out);
	}
	
	//check for destination instruction
	bool typeOfDest(string s ) {
		int size = s.length(); 
		char a[size];
		strcpy(a, s.c_str());
		for(int i=0;i< size;i++)
			if(a[i] == '=') {
				return true;
				break;}
		return false;
	}
	// check comment line
	bool checkCommentline(string s){
		int size = s.length(); 
		char a[size];
		strcpy(a, s.c_str());
		if(a[0] == '/') 	return true;
		return false;	
	}
	// destination instruction
	int checkDest(string c){
	int num;
		if(c == "M") 	num = 1;
		else if(c == "D") 	num = 2;
		else if(c == "MD") 	num = 3;
		else if(c == "A") 	num = 4;
		else if(c == "AM") 	num = 5;
		else if(c == "AD") 	num = 6;
		else if(c == "AMD") 	num = 7;
		else num =0;
	return num;
	} 
	// jump insrtuction
	int checkJump(string c){
	int num;
	if(c == "JGT") 	num = 1;
		else if(c == "JEQ") 	num = 2;
		else if(c == "JGE") 	num = 3;
		else if(c == "JLT") 	num = 4;
		else if(c == "JNE") 	num = 5;
		else if(c == "JLE") 	num = 6;
		else if(c == "JMP") 	num = 7;
		else num =0;
	return num;
	} 
	// Compostion
	int checkCom(string s){
	int num;
		if(s == "0") 			num = 42;
		else if(s == "1") 		num = 63;
		else if(s == "-1" )		num = 58;
		else if(s == "D") 		num = 12;
		else if(s == "A") 		num = 48;
		else if(s == "!D") 		num = 13;
		else if(s == "!A") 		num = 49;
		else if(s == "-D") 		num = 15;
		else if(s == "-A") 		num = 51;
		else if(s == "D+1") 	num = 31;
		else if(s == "A+1") 	num = 55;
		else if(s == "D-1") 	num = 14;
		else if(s == "A-1") 	num = 50;
		else if(s == "D+A") 	num = 2;
		else if(s == "D-A") 	num = 19;
		else if(s == "A-D") 	num = 7;
		else if(s == "D&A") 	num = 0;
		else if(s == "D|A") 	num = 21;
		else if(s == "M") 		num = 112;
		else if(s == "!M") 		num = 113;
		else if(s == "-M") 		num = 115;
		else if(s == "M+1") 	num = 119;
		else if(s == "M-1") 	num = 114;
		else if(s == "D+M") 	num = 66;
		else if(s == "D-M") 	num = 83;
		else if(s == "M-D") 	num = 71;
		else if(s == "D&M") 	num = 64;
		else if(s == "D|M") 	num = 85;
	return num;
	} 
	//convert to binary
	void  convToBinary(int num, ofstream &out){
		int b[3];
		int i;
		for(i=0;i<3;i++){
			b[i]= num%2;
			num=num/2;
		}	
		for(i = i-1;i >=0;i--)
			out << b[i];
		}
		
	void  convToBinaryCom(int num,ofstream &out){
		int b[7];
		int i;
		for(i=0;i<7;i++){
			b[i]= num%2;
			num=num/2;
		}	
		for(i = i-1;i >=0;i--)
			out << b[i];
	}
	// print instruction
	void  printInst(string s1, string s2, string s3, ofstream &out){
	convToBinaryCom(checkCom(s2), out);
	convToBinary(checkDest(s1), out);
	convToBinary(checkJump(s3), out);
	out << '\n';
	}
	//Instruction C
	void returnMachineInsOfC(string s, ofstream &out){
		if(typeOfDest(s)){
		out <<"111";
		int size = s.length();
		char a[size];
		strcpy(a, s.c_str());
		string s1 = "";
		string s2 = "";
		for(int i=0;i<size;i++){
			if(a[i] == ' ' || a[i] == '\t');
			else if(a[i] == '/') break;
			else if(!(a[i] == '='))		s1 = s1 + a[i]; 
			else {
				for(int j=i+1;j<size;j++){
					if(a[j] == ' ' || a[j] == '\t');
					else if(a[j] == '/') break;
					else s2 = s2 + a[j];
		}	
		break;
		}
		}
		printInst(s1, s2,"", out);
		}
		else if (!typeOfDest(s)){
		out <<"111";
		int size = s.length();
		char a[size];
		strcpy(a, s.c_str());
		string s1 = "";
		string s2 = "";
		
		for(int i=0;i<size;i++){
		if(a[i] == ' ' || a[i] == '\t');
		else if(a[i] == '/') break;
		else if(!(a[i] == ';'))		s1 = s1 + a[i]; 
		else {
			for(int j=i+1;j<size;j++){
			if(a[j] == ' ' || a[j] == '\t');				
				else if(a[j] == '/') break;
				else s2 = s2 + a[j];
		}
		break;
	}
	}
	printInst("", s1,s2, out);
	}
	}
	// check for blank line
	bool blank(string s){
		int size = s.length();
		if(size == 0)  return true;
		return false;
		}
	bool symbols(string w){
		int size = w.length();
		char c[size];
		strcpy(c, w.c_str());
		for(int i=0;i<size ;i++)
		if((c[0]>='a'&& c[0]<='z' )|| (c[0]>='A'&& c[0]<='Z' ))	{
		return true;
		break;
		}
		return false;
	}
	// declared label
	bool decLabel(string s){
		int size = s.length(); 
		char a[size];
		strcpy(a, s.c_str());
		for(int i=0;i< size;i++)
		if(a[i] == '('){ return true;
		break;}
		return false;
	}	
		
	string convertToLabel(string s){
		int size = s.length();
		char d[size];
		strcpy(d,s.c_str());
		string label = "";
		for(int k=1;d[k]!=')';k++)
		label = label + d[k];
		return label;
	}	
	
	bool labelORvar(int count , string *s, string w, fstream &inout){
			int no=0;
		for(int j=1;j <=count ;j++){
		if(decLabel(s[j])){
		no++;
		string label = convertToLabel(s[j]);
		if(label == w)	{
		inout << "@" << (j - no)+1 << '\n';
		return true;
		break;
		}
		}
		}
		return false;		
		}
	

	int main(){
	char filename[20];
	ifstream in;
	cout << "Entre File Name: ";
	cin.getline(filename, 20);
	in.open(filename);
	 if(!in) 
	   { 
	       cout<<"No Such File!!!";
		   return 0; 
	   }	
	fstream inout("INTER.asm", ios :: out);
	ofstream out("swap.hack");
	string s;
	string a[100];
	int r, count =0;
	while(true){
	getline(in,s);
	if(!blank(s)){
	if(!checkCommentline(s)){
	a[r] = s;
	r++; 
	count++;
	}
	}
	if(in.eof()) break;
	}
	in.close();
	
	int varCount = 16;
	for(int i=0;i<count ;i++){ 
	if(blank(a[i]));
	else if(!(typeOfIns(a[i])) && !(decLabel(a[i])))
	inout << a[i] <<'\n';
	else if (decLabel(a[i]));
	else{
		string  w = convertToString(a[i]);
	if(checkPredefined(w))
	inout << a[i] << '\n';	
	else if(symbols(w)){
	if(labelORvar(count , a, w, inout));
	else if(!labelORvar(count , a ,w, inout)){
	inout << "@" <<varCount << '\n';
	varCount++;
		
	}
	}
	
	else inout <<a[i]<< '\n';
	}
	}
	
	inout.close();
	fstream read("INTER.asm", ios :: in);
	string b;
	while(true){
	getline(read,b);
	if(blank(b) || checkCommentline(b));
	else {
	if(typeOfIns(b)){	returnMachineInsOfA(b, out); }
	else if (!typeOfIns(b)){	returnMachineInsOfC(b, out);	}
	}
	if(read.eof()) break;
	}
	read.close();
	out.close();
	
	return 0;
	}