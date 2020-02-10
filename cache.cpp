#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <bitset>
#include <cstdlib>
#include <cmath>
#include <cstring>
#include <limits>
#include <vector>

using namespace std;

int cache_size;
int block_size;
int asso; //associativity 
int policy;
int block_number;
int set_number;
int tag_bit;
int offset_bit;
int index_bit = 0;
int next = 0; //for full FIFO's next location
int current = 0; //for full LRU's current cache data
int current_four[1000000]; //for 4way LRU's current cache data
int LRU_num = 1;

void calculate();
int convertbinary(int bit,string s);
int direct_map(string cache[][1],int isValid[],string data,string tmp);
int full(int cache[][1],int info[],string data);
int four_way(int cache[][4], int info[][4], string data, string tmp);

int main(int argc, char *argv[]){
	string line;
	fstream fin ,fout;
	fin.open(argv[1],ios::in);
	fout.open(argv[2], ios::out);

	getline(fin,line);
	cache_size = atoi(line.c_str());
	getline(fin,line);
	block_size = atoi(line.c_str());
	getline(fin,line);
	asso = atoi(line.c_str());
	getline(fin,line);
	policy = atoi(line.c_str());
    calculate();


	switch(asso){
		case 0:{
			string cache_direct[block_number][1];
			int isValid_direct[block_number];
			for(int i=0; i<block_number; i++){
				cache_direct[i][0] = "xxxxxxxxxx";
			}
			while(getline(fin,line) != NULL){
				stringstream ss;
				unsigned int n;
				string data,tmp;
				ss << hex << line;
				ss >> n;
				bitset<32> b(n);
				data = b.to_string();
				tmp = data.substr(tag_bit,index_bit);
				fout << direct_map(cache_direct,isValid_direct,data,tmp)<< endl;
			}
		break;
		}
		case 1:{
			int cache_fourway[block_number][4];
			int info_fourway[block_number][4];
			for(int i=0; i<block_number; i++){
				cache_fourway[i][0] = -1;
				cache_fourway[i][1] = -1;
				cache_fourway[i][2] = -1;
				cache_fourway[i][3] = -1;
				info_fourway[i][0] = 0;
				info_fourway[i][1] = 0;
				info_fourway[i][2] = 0;
				info_fourway[i][3] = 0;
				current_four[i] = 0;
			}
			while(getline(fin,line) != NULL){
				stringstream ss;
				unsigned int n;
				string data,tmp;
				ss << hex << line;
				ss >> n;
				bitset<32> b(n);
				data = b.to_string();
				tmp = data.substr(tag_bit,index_bit);
				fout << four_way(cache_fourway,info_fourway,data,tmp) << endl;
			}
		break;
		}
		case 2:{
		    int cache_full[block_number][1];
			int info_full[block_number]; //FIFO LRU record
			for(int i=0; i<block_number; i++){
				cache_full[i][0] = -1;
				info_full[i] = 0;
			}
			while(getline(fin,line) != NULL){
				stringstream ss;
				unsigned int n;
				string data,tmp;
				ss << hex << line;
				ss >> n;
				bitset<32> b(n);
				data = b.to_string();
				tmp = data.substr(tag_bit,index_bit);
				fout << full(cache_full,info_full,data)<<endl;
			}
		break;
		}
	}
}

void calculate(){
	int temp;
	block_number = cache_size*1024/block_size;
	if(asso == 1){block_number /= 4;}
	temp = block_size;
	while(temp > 1){
		temp /= 2;
		offset_bit++;
	}

	temp = block_number;
	while(temp > 1){
		temp /= 2;
		index_bit++;
	}
	
	switch(asso){
		case 0:
			index_bit = index_bit;
		break;
		case 1:
			index_bit = index_bit;
		break;
		case 2:
			index_bit = 0;
		break;
	}
	tag_bit = 32 - index_bit - offset_bit;

	cout << tag_bit <<" "<<index_bit <<endl;
}

int convertbinary(int bit,string s){
	int dec = 0;
	for(int i=0; i<bit; i++){
		if(s.at(i) == '1'){
		dec = dec + pow(2,bit-i-1);
		}
	}
	return dec;
}

int direct_map(string cache[][1],int isValid[],string data,string tmp){
		int out;
		int currentblock;
        currentblock = convertbinary(index_bit,tmp);

		if(cache[currentblock][0].compare(data.substr(0,tag_bit)) != 0 && isValid[currentblock] == 1){
			out = convertbinary(tag_bit,cache[currentblock][0]);
			cache[currentblock][0] = data.substr(0,tag_bit);
		}
		else{
			isValid[currentblock] = 1;
			out = -1;
			cache[currentblock][0] = data.substr(0,tag_bit);

		}
return out;
}

int full(int cache[][1],int info[],string data){
		int out;
		int isExist = 0;

		switch(policy){
		case 0://FIFO
			for(int i=0; i<block_number && isExist == 0; i++){
				if(cache[i][0] == convertbinary(tag_bit,data.substr(0,tag_bit))){
					isExist = 1;
				}
				else{
					isExist = 0;
				}
			}

			if(isExist == 1){
				out = -1;
			}
			else if(cache[next][0] == -1){
				out = -1;
				cache[next][0] = convertbinary(tag_bit,data.substr(0,tag_bit));
				next++;
				if(next >= block_number){
					next = 0;
				}
			}
			else{
				out = cache[next][0];
				cache[next][0] = convertbinary(tag_bit,data.substr(0,tag_bit));
				next++;
				if(next >= block_number){
					next = 0;
				}
			}
		break;
		case 1: case 2://LRU
			isExist = 0;
			int chooseNumber = 0;
			int c = 0; // index of same tag
			for(int i=0; i<block_number && isExist == 0; i++){
				if(cache[i][0] == convertbinary(tag_bit,data.substr(0,tag_bit))){
					isExist = 1;
					c = i;
				}
				else{
					isExist = 0;
				}
			}
			
			if(isExist == 1){
				out = -1;
				info[c] = LRU_num;
				LRU_num++;
			}
			else if(current<block_number){
				out = -1;
				cache[current][0] = convertbinary(tag_bit,data.substr(0,tag_bit));
				info[current] = LRU_num;
				LRU_num++;
				current++;
				
			}
			else{ //full of cache
				for(int i=0; i<block_number; i++){
					if(info[i] < info[chooseNumber] && info[i] != 0){
						chooseNumber = i;
					}
				}
				out = cache[chooseNumber][0];
				cache[chooseNumber][0] = convertbinary(tag_bit,data.substr(0,tag_bit));
				info[chooseNumber] = LRU_num;
				LRU_num++;
			}
			
			
		break;
		}

		return out;

}


int four_way(int cache[][4], int info[][4], string data, string tmp){
	int out;
	int isExist = 0;
	int currentblock;
    currentblock = convertbinary(index_bit,tmp);
	switch(policy){
		case 0:{
			isExist = 0;
			int chooseNumber = 0;
			int c = 0;
			for(int i=0; i<4 && isExist == 0; i++){
				if(cache[currentblock][i] == convertbinary(tag_bit,data.substr(0,tag_bit))){
					isExist = 1;
					c = i;
				}
				else{
					isExist = 0;
				}
			}
			if(isExist == 1){
				out = -1;
			}
			else if(cache[currentblock][info[currentblock][0]] == -1){
				out = -1;
				cache[currentblock][info[currentblock][0]] = convertbinary(tag_bit,data.substr(0,tag_bit));
				info[currentblock][0] = info[currentblock][0] + 1;
				if(info[currentblock][0] >= 4){
					info[currentblock][0] = 0;
				}
			}
			else{
				out = cache[currentblock][info[currentblock][0]];
				cache[currentblock][info[currentblock][0]] = convertbinary(tag_bit,data.substr(0,tag_bit));
				info[currentblock][0] = info[currentblock][0] + 1;
				if(info[currentblock][0] >= 4){
					info[currentblock][0] = 0;
				}
			}
			break;
		}
		case 1: case 2:{
			isExist = 0;
			int chooseNumber = 0;
			int c = 0; // index of same tag
			for(int i=0; i<4 && isExist == 0; i++){
				if(cache[currentblock][i] == convertbinary(tag_bit,data.substr(0,tag_bit))){
					isExist = 1;
					c = i;
				}
				else{
					isExist = 0;
				}
			}
			
			if(isExist == 1){
				out = -1;
				info[currentblock][c] = LRU_num;
				LRU_num++;
			}
			else if(current_four[currentblock]<4){
				out = -1;
				cache[currentblock][current_four[currentblock]] = convertbinary(tag_bit,data.substr(0,tag_bit));
				info[currentblock][current_four[currentblock]] = LRU_num;
				
				LRU_num++;
				current_four[currentblock]++;
				
			}
			else{ //full of cache
				for(int i=0; i<4; i++){
					if(info[currentblock][i] < info[currentblock][chooseNumber]){
						chooseNumber = i;
					}
				}
				//cout << info[chooseNumber] <<" "<< info[1] <<endl;
				out = cache[currentblock][chooseNumber];
				cache[currentblock][chooseNumber] = convertbinary(tag_bit,data.substr(0,tag_bit));
				info[currentblock][chooseNumber] = LRU_num;
				LRU_num++;
			}
			break;
			}
	}
	return out;
}
