#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <boost/tokenizer.hpp>
#include <cmath>
#include <climits>
#include <cfloat>
using namespace std;
using namespace boost;
//trainSet 0, validationSet 1, testSet 2;

struct label{
  int cls;
  double sim;
};

ofstream myOutput;
vector<vector<double>> data[3];
int K;
vector<vector<label>> nn;

double similarity(vector<double> a, vector<double> b){
  double sim=0;
  for(int i=1;i<a.size();i++){
    sim+= a[i]*b[i];
  }
  return sim;
}


double assignCls(int record,int n){
  double arr[10]={0},c,max=0;
  for(int i=0;i<n;i++){
    arr[nn[record][i].cls]++;
  }
  for(int i=0;i<10;i++){
    if(max<arr[i]){
      max = arr[i];
      c=i;
    }
  }
  return c;
}

double measureAcc(double* validCls,int dataSet){
  int count=0;
  for(int i=0;i<data[dataSet].size();i++){
    if(validCls[i]==data[dataSet][i][0]){
      count++;
    }
  }
  return (double)count/data[dataSet].size();
}

void knn(int dataSet, int end_K) {
  double val,max=0,acc;
  double *validCls;
  int n=end_K;
    validCls = new double[data[dataSet].size()];
    label temp[20];
    //VALIDATION && TEST
      for(int i=0;i<data[dataSet].size();i++){
        for(int j=0;j<data[0].size();j++){
          val = similarity(data[dataSet][i],data[0][j]);
          for(int k=0;k<n;k++){
            if(temp[k].sim<=val){
              if(k<n-1){
                for(int l=n-1;l>=k+1;l--){
                  temp[l].cls = temp[l-1].cls;
                  temp[l].sim =temp[l-1].sim;
                }
              }
              temp[k].cls = data[0][j][0];
              temp[k].sim = val;
              break;
            }
          }
        }
       // cout<<"i="<<i<<endl;
        vector<label> tempV;
        for(int k=0;k<n;k++){
          tempV.push_back(temp[k]);
        }
        nn.push_back(tempV);
        tempV.clear();
        for(int k=0;k<n;k++){
          temp[k].cls = INT_MIN;
          temp[k].sim=-DBL_MAX;
        }
      }

    //Assigning Class
    for(int tt=1;tt<=end_K;tt++){
      for(int i=0;i<data[dataSet].size();i++){
          validCls[i] = assignCls(i,tt);
      }
      acc = measureAcc(validCls,dataSet);
      if(dataSet==2 && tt==end_K){
        cout<<"ACCURACY: "<<acc<<"\n";
        for(int j=0;j<data[dataSet].size();j++)
          myOutput<<validCls[j]<<endl;
      }
      if(max<acc){
        max=acc;
        K = tt;
      }
    }
    nn.clear();
    delete validCls;
}


int main(int argc,char* argv[]){
  string op_file, op_weight;
  ifstream myInput;
  op_file = argv[4];
  if(argc>5)
    op_weight = argv[5];

  //temp variables
  vector<double> tempV;
  string pix;
  int tempInt =1;
  char_separator<char> sep(",");
  myOutput.open(op_file);
  double tempPix,tempSum=0;

  for(int i=0;i<3;i++){
    myInput.open(argv[i+1]);
    while (!myInput.eof()) {
      getline(myInput, pix, '\n');
      if(pix=="")break;
      tokenizer<char_separator<char>> tokens(pix, sep);
      tempSum=0;
      for (const auto& t : tokens) {
          tempPix = stof(t);
          tempV.push_back(tempPix);
          tempSum+=tempPix*tempPix;
      }
      tempSum-=tempV[0]*tempV[0];
      tempSum=sqrt(tempSum);
      for (int j=1;j<tempV.size();j++) {
          tempV[j]/=tempSum;
      }
      data[i].push_back(tempV);
      tempV.clear();
  	}
    myInput.close();
  }

  knn(1,20);
  data[0].insert(data[0].end(),data[1].begin(),data[1].end());
  knn(2,K);
  myOutput.close();
}
