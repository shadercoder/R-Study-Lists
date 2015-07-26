#include<iostream>
#include<sstream>
#include<fstream>
#include<queue>
#include<map>
#include<vector>
#include<utility>
#include<stdio.h>
#include<string>
#include<algorithm>

#define SIZE 100

using namespace std;

//Store the variable information in map
struct VariableInfo{
	string type;
	string value;
};

//Extract comma parameters from parameter string into queue
class CommaSeperation{
	private:
		//Store the parameter strings
		queue<string> paramQueue;
	public:
		//Extract the parameter strings
		void extractParams(string _params){
			
			if(!_params.empty()){
				
				size_t pos = _params.find(',', 0);
				size_t temp = 0;
				paramQueue.push(_params.substr(0,pos-temp));
				while(pos != string::npos){
					temp = pos+1;
					pos = _params.find(',',pos+1);
					paramQueue.push(_params.substr(temp,pos-temp));
				}
				
			}
			
		}
		queue<string> getQueue(){
			return paramQueue;
		}		
};

//Extract plus parameters from parameter string into queue
class PlusSeperation{
	private:
		//Store the parameter strings
		queue<string> paramQueue;
	public:
		//Extract the parameter strings
		void extractParams(string _params){
			
			if(!_params.empty()){
				
				size_t pos = _params.find('+', 0);
				size_t temp = 0;
				paramQueue.push(_params.substr(0,pos-temp));
				while(pos != string::npos){
					temp = pos+1;
					pos = _params.find('+',pos+1);
					paramQueue.push(_params.substr(temp,pos-temp));
				}
				
			}
			
		}
		queue<string> getQueue(){
			return paramQueue;
		}		
};

//Seperate the string describing about function
class FuncSeperation{
	private:
		//Variable name
		string varName;
		//Function name
		string funcName;
		//Parameter names
		CommaSeperation params;
	public:
		void FuncSepInit(string _order){
			
			//Positions of the conserved signs
			int equalSite = _order.find_first_of('=');
			int leftParen = _order.find_first_of('(');
			int rightParen = _order.find_last_of(')');
			//Extract the strings
			varName = _order.substr(9,equalSite-9);
			funcName = _order.substr(equalSite+1,leftParen-equalSite-1);
			//Convert parameter string into ParamSeperation object
			params.extractParams(_order.substr(leftParen+1,rightParen-leftParen-1));
			
		}
		string getFuncName(){
			return funcName;
		}
		string getOutputVarName(){
			return varName;
		}
		CommaSeperation getParams(){
			return params;
		}

};

//Let WordSepSys and seeVariables know each other
class WordSepSys;
void seeVariables(WordSepSys sys);
//Enable using seeQueue function
template <typename T>
void seeQueue(queue<T> q);
//Convert string to integer
int str2int(string _str);
//Convert string to float
float str2float(string _str);
//Convert integer to string
string int2str(int i);
//Convert float to string
string float2str(float f);

//For word seperation system object to process the loop
class ForWordSepSys{
	private:
		//Counter variable name
		string counterName;
		//Counter start value
		string startVal;
		//Counter and value
		string endVal;
		//Store order information, command(string), I(queue<string>), O(queue<string>)
		queue<pair<string,pair<queue<string>,queue<string> > > > orderStrQueue;
		//Store the modified global variables information, name(string), type and value(VariableInfo)
		map<string,VariableInfo> modifiedGlobalVars;
	public:
		//Store the local variable
		map<string,VariableInfo> varMap;
		map<string,VariableInfo>::iterator mi;
		//Store the loop information
		void storeLoopInfo(string _counterName,string _startVal,string _endVal){
			counterName = _counterName;
			startVal = _startVal;
			endVal = _endVal;
			cout<<"counter "<<counterName<<", start value "<<startVal<<", end value "<<endVal<<endl;
		}
		//Start loop processing
		void startLoop(){
			
			printf("A loop start...\n");
			//Store start and end value
			pair<string,VariableInfo> startValVar;
			startValVar.first = "startVal";
			startValVar.second.type = "int";
			pair<string,VariableInfo> endValVar;
			endValVar.first = "endVal";
			endValVar.second.type = "int";
			//Pair to store counter variable information
			pair<string,VariableInfo> counterVar;
			counterVar.first = counterName;
			counterVar.second.type = "int";
			//Check if start value is global input variable
			if(startVal[0] - '0'<49){
				counterVar.second.value = startVal;
				//Declare start value
				startValVar.second.value = startVal;
			} else{
				mi = modifiedGlobalVars.find(startVal);
				counterVar.second.value = (*mi).second.value;
				//Declare start value
				startValVar.second.value = (*mi).second.value;
			}
			//Check if end value is global input variable
			if(endVal[0] - '0'<49){
				//Declare start value
				endValVar.second.value = endVal;
			} else{
				mi = modifiedGlobalVars.find(endVal);
				//Declare start value
				endValVar.second.value = (*mi).second.value;
			}
			//Declare counter
			varMap.insert(counterVar);
			//Declare start value
			varMap.insert(startValVar);
			//Declare end value
			varMap.insert(endValVar);
			//For loop start
			for(int i=str2int(startValVar.second.value);i<=str2int(endValVar.second.value);i++){
				//Decode and process the order
				orderDecode();
				//Update counter value
				mi = varMap.find(counterName);
				//Loop if not violating the conditions
				if(str2int((*mi).second.value)<=str2int(endValVar.second.value)){
					pair<string,VariableInfo> newCounter;
					newCounter.first = (*mi).first;
					newCounter.second.type = (*mi).second.type;
					newCounter.second.value = int2str(str2int((*mi).second.value)+1);
					varMap.erase(counterName);
					varMap.insert(newCounter);
				}
			}
			cout<<"For variables: "<<endl;
			for(map<string,VariableInfo>::iterator seeVarMI=varMap.begin();seeVarMI!=varMap.end();seeVarMI++){
				cout<<"	"<<(*seeVarMI).first<<" "<<(*seeVarMI).second.type<<" "<<(*seeVarMI).second.value<<endl;
			}
		}
		//Extract the I/O variable names(pair<queue<string>,queue<string> >) and command types(string)
		pair<string,pair<queue<string>,queue<string> > > popAndExtractOrderInfo(string _order){
			//Extract the variable
			size_t pos = _order.find_first_of(' ');
			string command = _order.substr(0,pos-0);
			//Store the variable names(I/O)
			pair<queue<string>,queue<string> > varNames;
			int commandType = 0;
			if(command == "for") commandType = 1;
			else if(command == "if") commandType = 2;
			else if(command == "end") commandType = 3;
			else{
				commandType = 0;
			}
			if(commandType == 0){
				//Diferentiate plus or given value
				size_t plusPos = _order.find('+');
				if(plusPos != string::npos){
					//To extract the plus-value variables and store into queue
					size_t equalPos = _order.find('=');
					varNames.second.push(_order.substr(0,equalPos-0));
					PlusSeperation plusParams;
					plusParams.extractParams(_order.substr(equalPos+1));
					varNames.first = plusParams.getQueue();
				} else{
					//To extract the given-value variables
					size_t equalPos = _order.find('=');
					//Store input variable
					varNames.first.push(_order.substr(equalPos+1));
					//Store output variable
					varNames.second.push(_order.substr(0,equalPos-0));
				}
			}
			pair<string,pair<queue<string>,queue<string> > > resultPair(command,varNames);
			//Store order information into order queue
			orderStrQueue.push(resultPair);
			return resultPair;
		}
		//load the global variable information into for object
		void loadGlobalValues(map<string,VariableInfo> _globalVars){
			//Insert global variables
			for (map<string,VariableInfo>::iterator g_mi=_globalVars.begin(); g_mi!=_globalVars.end(); g_mi++){
				//find if repeat loading variable
				mi = varMap.find((*g_mi).first);
				if(mi == varMap.end()){
					modifiedGlobalVars.insert((*g_mi));
					varMap.insert((*g_mi));
				}
			}
		}
		//Decode and process the order
		void orderDecode(){
			queue<pair<string,pair<queue<string>,queue<string> > > > orders = orderStrQueue;
			//Check if it is plus order
			size_t plusPos = orders.front().first.find('+');
			while(!orders.empty()){
				if(plusPos != string::npos){
					plusValue(orders.front().second.first,orders.front().second.second);
				} else{
					givenValue(orders.front().second.first,orders.front().second.second);
				}
				orders.pop();
			}
		}
		//Given value
		void givenValue(queue<string> _inputVars,queue<string> _outputVars){
			pair<string,VariableInfo> output;
			mi = varMap.find(_inputVars.front());
			output.first = _outputVars.front();
			output.second.type = (*mi).second.type;
			output.second.value = (*mi).second.value;
			//Check if output variable existed before
			map<string,VariableInfo>::iterator repeatMI = varMap.find(_outputVars.front());
			if(repeatMI == varMap.end()){
				varMap.insert(output);
			} else{
				varMap.erase(_outputVars.front());
				varMap.insert(output);
			}
		}
		//Plus value
		void plusValue(queue<string> _inputVars,queue<string> _outputVars){
			string outputVal = "0";
			queue<string> inputs = _inputVars;
			//Add all inputs
			while(!inputs.empty()){
				//Check if it exist in variable list, if not, find its type
				mi = varMap.find(inputs.front());
				//Find the string
				size_t stringPos = inputs.front().find('"');
				//Find the float
				size_t floatPos = inputs.front().find('.');
				if(mi == varMap.end()){
					//Find the type and add together
					if(inputs.front()[0] - '0'<49 && inputs.front()[0] - '0'>0 && floatPos == string::npos){
						outputVal = int2str(str2int(outputVal)+str2int(inputs.front()));
						pair<string,VariableInfo> output;
						output.first = _outputVars.front();
						output.second.type = "int";
						output.second.value = outputVal;
						//Check if output variable existed before
						map<string,VariableInfo>::iterator repeatMI = varMap.find(_outputVars.front());
						if(repeatMI == varMap.end()){
							varMap.insert(output);
						} else{
							varMap.erase(_outputVars.front());
							varMap.insert(output);
						}
					} else if(floatPos != string::npos){
						outputVal = float2str(str2float(outputVal)+str2float(inputs.front()));
						pair<string,VariableInfo> output;
						output.first = _outputVars.front();
						output.second.type = "float";
						output.second.value = outputVal;
						//Check if output variable existed before
						map<string,VariableInfo>::iterator repeatMI = varMap.find(_outputVars.front());
						if(repeatMI == varMap.end()){
							varMap.insert(output);
						} else{
							varMap.erase(_outputVars.front());
							varMap.insert(output);
						}
					} else if(stringPos != string::npos){
						outputVal = outputVal + inputs.front();
						pair<string,VariableInfo> output;
						output.first = _outputVars.front();
						output.second.type = "string";
						output.second.value = outputVal;
						//Check if output variable existed before
						map<string,VariableInfo>::iterator repeatMI = varMap.find(_outputVars.front());
						if(repeatMI == varMap.end()){
							varMap.insert(output);
						} else{
							varMap.erase(_outputVars.front());
							varMap.insert(output);
						}
					}
				} else{
					map<string,VariableInfo>::iterator plusMI = varMap.find(inputs.front());
					if((*plusMI).second.type == "int"){
						outputVal = int2str(str2int(outputVal)+str2int((*plusMI).second.value));
						pair<string,VariableInfo> output;
						output.first = _outputVars.front();
						output.second.type = "int";
						output.second.value = outputVal;
						//Check if output variable existed before
						map<string,VariableInfo>::iterator repeatMI = varMap.find(_outputVars.front());
						if(repeatMI == varMap.end()){
							varMap.insert(output);
						} else{
							varMap.erase(_outputVars.front());
							varMap.insert(output);
						}
					} else if((*plusMI).second.type == "float"){
						outputVal = float2str(str2float(outputVal)+str2float((*plusMI).second.value));
						pair<string,VariableInfo> output;
						output.first = _outputVars.front();
						output.second.type = "float";
						output.second.value = outputVal;
						//Check if output variable existed before
						map<string,VariableInfo>::iterator repeatMI = varMap.find(_outputVars.front());
						if(repeatMI == varMap.end()){
							varMap.insert(output);
						} else{
							varMap.erase(_outputVars.front());
							varMap.insert(output);
						}
					} else if((*plusMI).second.type == "string"){
						outputVal = outputVal + (*plusMI).second.value;
						pair<string,VariableInfo> output;
						output.first = _outputVars.front();
						output.second.type = "string";
						output.second.value = outputVal;
						//Check if output variable existed before
						map<string,VariableInfo>::iterator repeatMI = varMap.find(_outputVars.front());
						if(repeatMI == varMap.end()){
							varMap.insert(output);
						} else{
							varMap.erase(_outputVars.front());
							varMap.insert(output);
						}
					}
				}
				inputs.pop();
			}
		}
};

//Word seperation system - Read the file and call other functions to convert orders into program
class WordSepSys{
	private:
		//Store the orders
		queue<string> strQueue;
		//Store the functions
		FuncSeperation func1;
		//For object to process the loop
		ForWordSepSys loop1;
	public:
		//Store the variables information for searching, name(string), type and value(VariableInfo)
		map<string,VariableInfo> varMap;
		map<string,VariableInfo>::iterator mi;
		//Constructor
		WordSepSys(string addr){
			
			//Open txt file
			ifstream file;
			file.open(addr.c_str());
			//Store the current line
			string curLine;
			//Looping to read each line
			if(file.is_open()){
				while (getline(file, curLine)){
					strQueue.push(curLine);
				}  
			}
			//Enter into main console to convert the string queue into programs line by line
			mainConsole(strQueue);
			//Close the file
			file.close();
									
		}
		//Main console: Looping and convert every order into program
		void mainConsole(queue<string> _strQueue){
			
			//Store the function information
			funcInformStore(_strQueue.front());
			_strQueue.pop();
			//Read every order in queue and convert to program
			orderDecode(_strQueue);
			
		}
		//Restore the function information
		void funcInformStore(string _funcInform){

			string order = _funcInform;
			func1.FuncSepInit(order);
						
		}
		//Decide how to decode the order
		void orderDecode(queue<string> _stringQueue){
			
			//Decode the order line-by-line
			while(!_stringQueue.empty()){
				
				//Extract the command type, 1:for, 2:if, 3:end
				size_t pos = _stringQueue.front().find_first_of(' ');
				string command = _stringQueue.front().substr(0,pos-0);
				int commandType = 0;
				if(command == "for") commandType = 1;
				else if(command == "if") commandType = 2;
				else if(command == "end") commandType = 3;
				else{
					commandType = 0;
				}
				//Do different behaviors according to command type
				switch(commandType){
					
					//Do for command
					case 1:{
						printf("for ");
						
						size_t equalPos = _stringQueue.front().find('=');
						size_t colonPos = _stringQueue.front().find(':');
						//Restore counter name
						string forCounterName = _stringQueue.front().substr(pos+1,equalPos-pos-1);
						//Restore start and end
						string forStartVal = _stringQueue.front().substr(equalPos+1,colonPos-equalPos-1);
						string forEndVal = _stringQueue.front().substr(colonPos+1);
						_stringQueue.pop();
						//When encouter "for" or "end", and we should break the loop
						bool isContinue = 1;
						//Input the loop information
						loop1.storeLoopInfo(forCounterName,forStartVal,forEndVal);
						//Decode and pop the next range of orders and related variables' names into for-object
						while(isContinue){
							size_t spacePos = _stringQueue.front().find_first_of(' ');
							string inForCommand = _stringQueue.front().substr(0,spacePos-0);
							if(inForCommand == "end"){
								isContinue = 0;
								break;
							} else{
								//Pop an order into for-object, for-object should store the needed and 
								//will-be-modified variable names
								pair<string,pair<queue<string>,queue<string> > > forInfoPair =
									loop1.popAndExtractOrderInfo(_stringQueue.front());
								//Temporary vector to store variables
								vector<string> forVarVector;
								vector<string>::iterator vi;
								//Check the input variable
								queue<string> inputVarNames = forInfoPair.second.first;
								//If the variable is existed before, find and insert it into globalInputs
								while(!inputVarNames.empty()){
									mi = varMap.find(inputVarNames.front());
									if(mi != varMap.end()){
										forVarVector.push_back(inputVarNames.front());
									}
									inputVarNames.pop();
								}
								//Check the output variable
								queue<string> outputVarNames = forInfoPair.second.second;
								//If the variable is existed before, find and insert it into globalInputs
								while(!outputVarNames.empty()){
									mi = varMap.find(outputVarNames.front());
									if(mi != varMap.end()){
										forVarVector.push_back(outputVarNames.front());
									}
									outputVarNames.pop();
								}
								//Store the global variables into for-object
								map<string,VariableInfo> globalVars;
								for(vi = forVarVector.begin();vi != forVarVector.end();vi++){
									mi = varMap.find((*vi));
									globalVars.insert((*mi));
								}
								//Send global inputs and ouputs to for-object
								loop1.loadGlobalValues(globalVars);
								_stringQueue.pop();
							}									
						}
						//Let for-object process the loop
						loop1.startLoop();
						
						break;
					}
					//Do if command
					case 2:{
						printf("if\n");
						break;
					}
					//Skip end command
					case 3:{
						printf("end\n");
						break;
					}
					//Do other command
					default:
						//Diferentiate plus or given value
						size_t plusPos = _stringQueue.front().find('+');
						if(plusPos != string::npos){
							//To process the plus-value orders
							plusValue(_stringQueue.front());
						} else{
							//To process the given-value orders
							givenValue(_stringQueue.front());
						}
					
				}
				
				_stringQueue.pop();
				
			}
			seeVariables(*this);
			
		}
		//To process the given-value orders
		void givenValue(string _givenOrder){
			//Find the equal site
			size_t equalPos = _givenOrder.find('=');
			//Find if the variable is a float
			size_t floatPos = _givenOrder.find('.');
			//Find if the variable is a string
			size_t strPos = _givenOrder.find('"');
			//According to data type store the data
			if(floatPos != string::npos){
				//Store the float-type variable
				string name = _givenOrder.substr(0,equalPos-0);
				VariableInfo info;
				info.type = "float";
				info.value = _givenOrder.substr(equalPos+1);
				varMap.insert(pair<string,VariableInfo>(name,info));
			} else if(strPos != string::npos){
				//Store the string-type variable
				string name = _givenOrder.substr(0,equalPos-0);
				VariableInfo info;
				info.type = "string";
				info.value = _givenOrder.substr(equalPos+1);
				varMap.insert(pair<string,VariableInfo>(name,info));
			} else{
				//Store the int-type variable
				string name = _givenOrder.substr(0,equalPos-0);
				VariableInfo info;
				info.type = "int";
				info.value = _givenOrder.substr(equalPos+1);
				varMap.insert(pair<string,VariableInfo>(name,info));
			}
		}
		//Handle the plus order
		void plusValue(string _plusOrder){
			//Find the equal site
			size_t equalPos = _plusOrder.find('=');
			//Extract variables into PlusSeperation object
			string resultName = _plusOrder.substr(0,equalPos-0);
			PlusSeperation params;
			params.extractParams(_plusOrder.substr(equalPos+1));					
			//Check if they are 0:variables, 1:numbers, 2:floats, or 3:strings,
			queue<int> paramTypes = checkVarTypes(params);
			//Copy the params queue
			queue<string> paramsStr = params.getQueue();
			//Store the result value
			string resultValStr = "";
			//Declare a result value variable
			int resultInt = 0;
			float resultFloat = 0.0;
			//Record the result type
			int resultType = 0;
			string resultTypeStr = "string";
			//Sum the variables
			while(!paramTypes.empty()){
				if(paramTypes.front() == 0){
					//Find the variable type
					mi = varMap.find(paramsStr.front());
					string varType = (*mi).second.type; 
					//Convert to the type
					if(varType == "int"){
						resultInt = resultInt + str2int((*mi).second.value);
						resultType = 1;
					} else if(varType == "float"){
						resultFloat = resultFloat + str2float((*mi).second.value);
						resultType = 2;
					} else if(varType == "string"){
						resultValStr = resultValStr + (*mi).second.value;
						resultType = 3;
					}
				} else{
					switch(paramTypes.front()){
						case 1:{
							resultInt = resultInt + str2int(paramsStr.front());
							resultType = 1;
							break;
						}
						case 2:{
							resultFloat = resultFloat + str2float(paramsStr.front());
							resultType = 2;
							break;
						}
						case 3:{
							resultValStr = resultValStr + paramsStr.front();
							resultType = 3;
							break;
						}
						default:
							cout<<"Unsupported type!"<<endl;
					}
				}
				paramsStr.pop();
				paramTypes.pop();
			}
			//convert to string
			switch(resultType){
				case 1:{
					resultValStr = int2str(resultInt);
					resultTypeStr = "int";
					break;
				}
				case 2:{
					resultValStr = float2str(resultFloat);
					resultTypeStr = "float";
					break;
				}
			}
			//Store into variable table
			VariableInfo resultInfo;
			resultInfo.type = resultTypeStr;
			resultInfo.value = resultValStr;
			map<string,VariableInfo>::iterator varMI = varMap.find(resultName);
			if(varMI == varMap.end()){
				varMap.insert(pair<string,VariableInfo>(resultName,resultInfo));
			} else{
				varMap.erase(resultName);
				varMap.insert(pair<string,VariableInfo>(resultName,resultInfo));
			}
		}
		//Check plus parameters' types
		queue<int> checkVarTypes(PlusSeperation _params){
			
			//Copy the queue
			queue<string> temp = _params.getQueue();
			//Store types into queue
			queue<int> types;
			//Check each variable type
			while(!temp.empty()){
				
				//Take the first character
				char word = temp.front()[0];
				//Find if the variable is a float
				size_t floatPos = temp.front().find('.');
				//Find if the variable is a string
				size_t strPos = temp.front().find('"');
				//Determine the type of the variable
				if(word - '0' >= 49 && word - '0' < 75){
					types.push(0);
				} else if(word - '0' >= 0 && word - '0' < 10 && floatPos == string::npos){
					types.push(1);
				} else if(floatPos != string::npos){
					types.push(2);
				} else if(strPos != string::npos){
					types.push(3);
				} else{}
				
				temp.pop();
			}
			
			return types;
		}
		FuncSeperation getFunc(){
			return func1;
		}
		queue<string> getOrderQueue(){
			return strQueue;
		}
		
};

//Convert string to integer
int str2int(string _str){
	istringstream is(_str);
	int val;
	is>>val;
	return val;
}

//Covert string to float
float str2float(string _str){
	stringstream fs(_str);
	float val;
	fs>>val;
	return val;
}

//Convert integer to string
string int2str(int i) {
	string s;
  	stringstream ss(s);
  	ss << i;
  	return ss.str();
}

//Convert integer to string
string float2str(float f) {
	string s;
  	stringstream ss(s);
  	ss << f;
  	return ss.str();
}

//See the variables' information, usage: seeVariables(*this); -> When called from WordSepSys member function
void seeVariables(WordSepSys sys){
	for(sys.mi = sys.varMap.begin();sys.mi != sys.varMap.end();sys.mi++){
		cout<<(*(sys.mi)).first<<" "<<(*(sys.mi)).second.type<<" "<<(*(sys.mi)).second.value<<endl;
	}
	cout<<endl;
}

//See content in queue, usage: seeQueue<T>( __queue__ );
template <typename T>
void seeQueue(queue<T> q){
	while(!q.empty()){
		cout<<q.front()<<" ";
		q.pop();
	}
	cout<<endl;
}

int main(){
	//string functionAddr = "C:/Users/user/Desktop/test.txt";
	string functionAddr = "C:/Users/Administrator/Desktop/test.txt"; 
	WordSepSys sepSys(functionAddr); //Build WordSepSys and convert into program
	
	cout<<"Function name: "<<sepSys.getFunc().getFuncName()<<endl;
	cout<<"Function input: ";
	seeQueue(sepSys.getFunc().getParams().getQueue());
	cout<<"Function output: "<<sepSys.getFunc().getOutputVarName()<<endl;
	
	return 0;
}
