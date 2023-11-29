#include <iostream>
#include <fstream>
#include <math.h>
#include <vector>
#include <bitset>

using namespace std;
int binaryToInt(bitset<32> addr, int m) {
    int count = 0;
    int result = 0;
    for (int i = 31; i > 31 - m; i--) {
        result += addr[count] * pow(2, count);
        count++;
    }
    return result;
}
bitset<32> getPHTAddress(bitset<32>mw,int mwbits,bitset<32>w,int wbits){
    bitset<32> result = bitset<32>();
    mw<<=wbits;
    result = mw|w;
    return result;
}
int limiter(int size){
    //cout<< (1 << size) - 1<<endl;
    return (1 << size) - 1;
}
int main (int argc, char** argv) {
	ifstream config;
	config.open(argv[1]);

	int m, w, h;
	config >> m;
	config >> h;
	config >> w;
    int mMinuswSizse = m-w;
	config.close();

	ofstream out;
	string out_file_name = string(argv[2]) + ".out";
	out.open(out_file_name.c_str());
	
	ifstream trace;
	trace.open(argv[2]);

    vector<bitset<2>> PHT;
    vector<bitset<32>> BHT;//w is no larger than 32.
    BHT.resize(pow(2,h));
    PHT.resize(pow(2, m));

    bitset<2> st = bitset<2>("11"); //strong taken
    bitset<2> wt = bitset<2>("10"); //weak taken
    bitset<2> wnt = bitset<2>("01"); //weak not taken
    bitset<2> snt = bitset<2>("00"); //strong not taken
    for (int i = 0; i < pow(2, m); i++) {
        PHT[i] = wt;//初始化
    }
    for (int i = 0; i < pow(2, h); i++) {
        BHT[i] = 0; // 初始值
    }

    // TODO: Implement a two-level branch predictor
	while (!trace.eof()) {
        unsigned long pc;
        int taken;
        int PHTindex  = 0;
        int BHTindex  = 0;
        bitset<32> BHTresult;
        trace >> std::hex >> pc >> taken;
        int prediction = 0;
        int BStake;
        //get data from PC
        bitset<32> addr = bitset<32>(pc);
        bitset<32> hbitForBHTindex = bitset<32>(addr.to_string().substr(32-h-2,h));
        bitset<32> mwbitForUpperPHTindex = bitset<32>(addr.to_string().substr(32-mMinuswSizse-2,mMinuswSizse));

        //cout<<mwbit<<endl;
        //cout<<getPHTAddress(mwbit,mMinuswSizse,wbit,w)<<endl;
        //cout<<addr<<endl;
        //calculate BHT
        BHTindex = binaryToInt(hbitForBHTindex, h);
        BHTresult = BHT[BHTindex];
        //calculate PHTindex

        PHTindex = binaryToInt(getPHTAddress(mwbitForUpperPHTindex,mMinuswSizse,BHTresult,w),m);
        //cout<<bitset<32>(PHTindex)<<endl;
        //cout<<PHTindex<<endl;
        cout<<"hbitForBHTindex is "<<hbitForBHTindex<<endl;
        cout<<"mwbitForUpperPHTindex bits is "<<mwbitForUpperPHTindex<<endl;
        cout<<"wbitForLowerPHTindex bits is "<<BHT[BHTindex]<<endl;
        cout<<"pc is "<<addr<<endl;
        cout<<"now BHT index "<<BHTindex<<" "<< bitset<32>(BHTindex)<<endl;
        cout<<"now BHT "<<BHT[BHTindex]<<endl;
        cout<<"now PHT index "<<PHTindex<<" "<< bitset<32>(PHTindex)<<endl;
        cout<<"current taken is "<<taken<<endl;
        cout<<"now PHT "<<PHT[PHTindex]<<endl;

        if (PHT[PHTindex] == st ||PHT[PHTindex] == wt) { //预测taken
            BStake = 1;
            prediction = 1;
            if (BStake == taken) { //if real branch , st dont change,wt to st
                PHT[PHTindex] = st;
                BHT[BHTindex]<<=1;
                BHT[BHTindex] = BHT[BHTindex]|bitset<32>(1);
                BHT[BHTindex] = BHT[BHTindex]&bitset<32>(limiter(w));

            }
            else { //if nor branch
                if (PHT[PHTindex] == wt) { //if weak taken ,weak not taken
                    PHT[PHTindex] = wnt;
                }
                else {//if strong taken to weak taken
                    PHT[PHTindex] = wt;
                }
                BHT[BHTindex]<<=1;
                BHT[BHTindex] = BHT[BHTindex]|bitset<32>(0);
                BHT[BHTindex] = BHT[BHTindex]&bitset<32>(limiter(w));

            }
            //cout<<BHT[BHTindex]<<endl;

            //cout<<BHT[BHTindex]<<endl;
        }
        else { //预测not take
            BStake = 0;
            prediction = 0;
            if (BStake == taken) { //if branch ,turn PHT to strong not taken
                PHT[PHTindex] = snt;
                BHT[BHTindex]<<=1;
                BHT[BHTindex] = BHT[BHTindex]|bitset<32>(0);
                BHT[BHTindex] = BHT[BHTindex]&bitset<32>(limiter(w));

            }
            else { //if not correct
                prediction = 0;
                if (PHT[PHTindex] == wnt) { //if weak not taken , turn PHT to weak taken
                    PHT[PHTindex]= wt;
                }
                else {//if strong not taken , turn PHT to weak not taken
                    PHT[PHTindex] = wnt;
                }
                BHT[BHTindex]<<=1;
                BHT[BHTindex] = BHT[BHTindex]|bitset<32>(1);
                BHT[BHTindex] = BHT[BHTindex]&bitset<32>(limiter(w));
            }

        }

        cout <<"prediction is "<< prediction << endl;
        cout<<"current PHT "<<PHT[PHTindex]<<endl;
        cout<<"current BHT "<<BHT[BHTindex]<<endl;
        cout<<""<<endl;
        out << prediction << endl; // predict not taken
	}
	 
	trace.close();	
	out.close();
}

// Path: branchsimulator_skeleton_23.cpp