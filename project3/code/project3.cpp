#include<iostream>
#include<fstream>
#include<string>
#include<vector>
#include<algorithm>
#include<unistd.h>

using namespace std;

struct SubVal
{
	string substring;
	int value;
	vector<int> next;
	int pos;

	SubVal(){
		pos = -1;
	}
};

struct Output
{
	string target;
	int value_sum;
	vector<SubVal> path;
	
	Output(){
		value_sum = 0;
	}
};

bool cmp(SubVal sv1, SubVal sv2){
	return sv1.value > sv2.value;
}

void calNext(string str, vector<int> &next, int len){
	next.push_back(-1);
	int k=-1;
	for(int q=1; q<=len-1; q++){
		while(k>-1 && str[k+1]!=str[q]){
			k=next[k];
		}
		if(str[k+1]==str[q]){
			k=k+1;
		}
		next.push_back(k);
	}
}

int KMP(string str, SubVal substr){
	int slen = str.size();
	int plen = substr.substring.size();
	int k=-1;
	for(int i=0;i<slen;i++){
		while(k>-1 && substr.substring[k+1]!=str[i])
			k = substr.next[k];
		if(substr.substring[k+1] == str[i])
			k = k+1;
		if(k == plen-1){
			return i-plen+1;
		}
	}
	return -1;
}

Output addPath(Output &op1, Output &op2){
	Output op_tmp = op1;
	for(int i=0;i<op2.path.size();i++){
		op_tmp.path.push_back(op2.path[i]);
	}
	return op_tmp;
}

Output dfs(string str, vector<SubVal> &mySV, vector<Output> &max_tmp){
	for(int i=0;i<max_tmp.size();i++){
		if(str == max_tmp[i].target){
			return max_tmp[i];
		}
	}
	
	Output re_tmp;
	re_tmp.target = str;
	
	if(str == ""){
		return re_tmp;
	}
	else{
		Output op_tmp = re_tmp;
		
		for(int i=0;i<mySV.size();i++){
			Output tmp = op_tmp;
			string str_tmp = str;
			int pos_tmp = KMP(str, mySV[i]);
			if(pos_tmp != -1){
				mySV[i].pos = pos_tmp;
				tmp.path.push_back(mySV[i]);
				tmp.value_sum += mySV[i].value;
				// cout<<str_tmp<<"+++"<<mySV[i].substring<<endl;
				
				str_tmp.erase(pos_tmp, mySV[i].substring.size()); 
				
				Output temp = dfs(str_tmp, mySV, max_tmp);
				if(temp.value_sum + tmp.value_sum > re_tmp.value_sum){
					re_tmp = addPath(tmp, temp);
					re_tmp.value_sum = temp.value_sum + tmp.value_sum;
				}
			}
		}
		max_tmp.push_back(re_tmp);//cout<<re_tmp.target<<"----"<<re_tmp.value_sum<<endl;
		return re_tmp;
	}
}

Output dfsInit(string str, vector<SubVal> &mySV){
	Output result;
	vector<Output> max_tmp;
	result.target = str;

	Output result_tmp = result;
	
	for(int i=0;i<mySV.size();i++){
		Output op_tmp = result_tmp;
		string str_tmp = str;
		int pos_tmp = KMP(str, mySV[i]);
		if( pos_tmp != -1){
			mySV[i].pos = pos_tmp;
			op_tmp.path.push_back(mySV[i]);
			op_tmp.value_sum += mySV[i].value;
			// cout<<str_tmp<<"+++"<<mySV[i].substring<<endl;
			
			str_tmp.erase(pos_tmp, mySV[i].substring.size());
			
			Output temp = dfs(str_tmp, mySV, max_tmp);
			
			if(temp.value_sum + op_tmp.value_sum > result.value_sum){
				result = addPath(op_tmp, temp);
				result.value_sum = temp.value_sum + op_tmp.value_sum;
			}
		}
	}
	return result;
}

int main( int argc, char* argv[] )
{
	string in_file = argv[1];
	string out_file = argv[2];

    ifstream in(in_file.c_str());
	int str_len, subset_len;
	string str;
	in>>str_len>>subset_len>>str;
	vector<SubVal> mySV;
	SubVal temp;
	for(int i=0;i<subset_len;i++){
		in>>temp.substring>>temp.value;
		calNext(temp.substring, temp.next, temp.substring.size());
		mySV.push_back(temp);
	}

//	sort(mySV.begin(), mySV.end(), cmp);
	Output op_result = dfsInit(str, mySV);

	ofstream out(out_file.c_str());
	for(int i=0;i<op_result.path.size();i++){
		out<<op_result.path[i].pos<<" "<<op_result.path[i].substring<<endl;
	}
	out<<op_result.value_sum<<endl;

	return 0;
}

