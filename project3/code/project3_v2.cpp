#include<iostream>
#include<fstream>
#include<string>
#include<vector>
#include<algorithm>

using namespace std;

struct sub_val
{
	string substring;
	int value;
	double avg_value;
	int priority;
	vector<int> next;
	int pos;

	sub_val(){
		priority = 1;
		pos = -1;
	}
};

bool cmp1(sub_val sv1, sub_val sv2){
	if(sv1.avg_value == sv2.avg_value){
		return sv1.substring.size() > sv2.substring.size();
	}
	else{
		return sv1.avg_value>sv2.avg_value;
	}
}

bool cmp2(sub_val sv1, sub_val sv2){
	return sv1.priority < sv2.priority;
}

void cal_next(string str, vector<int> &next, int len){
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

int KMP(string str, sub_val substr){
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

int main()
{
	ifstream in("in1.txt");
	int n,k;
	string s;
	in>>n>>k>>s;
	// cout<<n<<" "<<k<<" "<<s<<endl;
	vector<sub_val> mySV;
	sub_val temp;
	for(int i=0;i<k;i++){
		in>>temp.substring>>temp.value;
		temp.avg_value = temp.value * 1.0 / temp.substring.size(); //calculate average value
		cal_next(temp.substring, temp.next, temp.substring.size()); // calculate next matrix
		mySV.push_back(temp);
	}

	sort(mySV.begin(), mySV.end(), cmp1);
	// for(int i=0;i<k;i++){
	// 	cout<<mySV[i].substring<<" "<<mySV[i].value<<" "<<mySV[i].avg_value<<endl;
	// }

	for(int i=0;i<mySV.size();i++){
		for(int j=i-1;j>=0;j--){
			if(KMP(mySV[j].substring, mySV[i]) != -1){
				mySV[i].priority ++;
				break;
			}
		}
	}

	sort(mySV.begin(), mySV.end(), cmp2);
//	 for(int i=0;i<k;i++){
//	 	cout<<mySV[i].substring<<" "<<mySV[i].value<<" "<<mySV[i].avg_value<<" "<<mySV[i].priority<<endl;
//	 }


	vector<sub_val> result;
	int sum=0;
	for(int i=0;i<mySV.size();i++){
		int temp = KMP(s, mySV[i]);
		if(temp != -1){
			mySV[i].pos = temp;
			sum += mySV[i].value;
			s.erase(temp, mySV[i].substring.size());
			result.push_back(mySV[i]);
			i = -1;
		}
	}
	for(int i=0;i<result.size();i++){
		cout<<result[i].pos<<" "<<result[i].substring<<endl;
	}
	cout<<sum<<endl;


	return 0;
}
