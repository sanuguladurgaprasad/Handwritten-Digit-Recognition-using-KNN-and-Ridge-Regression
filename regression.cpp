#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <boost/tokenizer.hpp>
#include <cmath>
#include <climits>
#include <cfloat>
#include <stdlib.h>
#include <unordered_map>
using namespace std;
using namespace boost;
//trainSet 0, validationSet 1, testSet 2;

ofstream weightOutput,myOutput;
vector<vector<double>> data[3];
double **weights;
double lambda[] = {0.01, 0.05, 0.1, 0.5, 1.0, 2.0, 5.0};
string op_file, op_weight;

int regression(int dataSet,int index){
  int dim = data[0][0].size()-1;
  int *R = new int[dim];
  int lambda_len=7;
  if(index>-1){
    lambda[0]=lambda[index];
    lambda[1]=2*lambda[index];
    lambda_len=2;
  }
  double accu[7];
  double *valid = new double[data[dataSet].size()];
  vector<double> newLabels[10];
  weights=new double*[10];
  double oldObj, newObj;
  for(int i=0;i<10;i++){
    weights[i]=new double[dim];
  }

  for(int i=0;i<data[0].size();i++){
    for(int j=0;j<10;j++){
      if(data[0][i][0]==j){
        newLabels[j].push_back(1);
      }
      else{
        newLabels[j].push_back(-1);
      }
    }
  }
  for(int l=0;l<lambda_len;l++){
    int st=clock();
    //cout<<"lambda="<<lambda[l]<<endl;
    for(int p=0;p<10;p++){
      for(int q=0;q<dim;q++){
        weights[p][q]=0;
      }
    }
    for(int c=0;c<10;c++){
    //  cout<<"c="<<c<<endl;
      //objective function
      double a1=0;
      for(int i1=0;i1<data[0].size();i1++){
        double sum1=0;
        for(int j1=0;j1<dim;j1++){
          sum1+=data[0][i1][j1+1]*weights[c][j1];
        }
        a1+= (sum1-newLabels[c][i1])*(sum1-newLabels[c][i1]);
      }

      double sum1=0;
      for(int j1=0;j1<dim;j1++){
        sum1+=weights[c][j1]*weights[c][j1];
      }
      oldObj = a1+lambda[l]*sum1;
      int iter=0;
      while(1){
        double *a = new double[data[0].size()];
	for(int z=0;z<dim;z++)R[z]=-1;
	int u=0;
	srand(time(NULL));
	while(u<dim){
	   int rr = rand()%dim;
	   if(R[rr]==-1){
		R[rr]=rr;
		u++;
	//	cout<<rr<<endl;
	   }
	   else continue;
	//}
        //for(int d=0;d<dim;d++){
          //cout<<rr<<endl;
          for(int i=0;i<data[0].size();i++){
            double sum=0;
            for(int j=0;j<dim;j++){
              if(j!=rr){
                sum+=data[0][i][j+1]*weights[c][j];
              }
            }
            a[i]=newLabels[c][i]-sum;
          }

          double mul=0;
          for(int i=0;i<data[0].size();i++){
            mul+=data[0][i][rr+1]*a[i];
          }

          double mul1=0;
          for(int i=0;i<data[0].size();i++){
            mul1+=data[0][i][rr+1]*data[0][i][rr+1];
          }
          mul1+=lambda[l];
          weights[c][rr]=mul/mul1;
        }
        //calculting Obj
        a1=0;
        for(int i1=0;i1<data[0].size();i1++){
          double sum1=0;
          for(int j1=0;j1<dim;j1++){
            sum1+=data[0][i1][j1+1]*weights[c][j1];
          }
          a1+= (sum1-newLabels[c][i1])*(sum1-newLabels[c][i1]);
        }

        double sum1=0;
        for(int j1=0;j1<dim;j1++){
          sum1+=weights[c][j1]*weights[c][j1];
        }
        newObj = a1+lambda[l]*sum1;
        //cout<<"oldObj= "<<oldObj<<"newObj= "<<newObj<<" err="<<(oldObj-newObj)/oldObj<<"========>Iter==>"<<iter++<<endl;
        if((oldObj-newObj)/oldObj<0.0001){//10*exp(-5)
          break;
        }
        oldObj = newObj;
      }
    }
    //validation/test set

    for(int i=0;i<data[dataSet].size();i++){
      double max=-DBL_MAX,cls;
      for(int c=0;c<10;c++){
        double xw=0;
        for(int d=0;d<dim;d++){
          xw+=data[dataSet][i][d+1]*weights[c][d];
        }
        //weightOutput<<xw<<",";
        if(max<xw){
          max=xw;
          cls=c;
        }
      }
      valid[i]=cls;
    }
    double count=0;
    accu[l]=0;
    for(int i=0;i<data[dataSet].size();i++){
      if(valid[i]==data[dataSet][i][0]){
        count++;
      }
    }
    accu[l]=count/data[dataSet].size();
    //cout<<"accu="<<accu[l]<<endl;
    if(dataSet==2){
      myOutput.open(op_file);
      weightOutput.open(op_weight);
      for(int i=0;i<data[dataSet].size();i++)
        myOutput<<valid[i]<<endl;
      for(int i=0;i<10;i++){
        for(int j=0;j<dim;j++){
          weightOutput<<weights[i][j]<<",";
        }weightOutput<<endl;
      }
      weightOutput<<"\n";
      myOutput.close();
      weightOutput.close();
    }
    int end=clock();
    //cout<<"time: "<<(end-st)/double(CLOCKS_PER_SEC)<<endl;
  }
  double max=0;
  int L;
  for(int i=0;i<lambda_len;i++){
    if(max<accu[i]){
      max = accu[i];
      L=i;
    }
  }
  if(dataSet==2 && L==1){
    myOutput.open(op_file);
    weightOutput.open(op_weight);
    for(int i=0;i<data[dataSet].size();i++)
      myOutput<<valid[i]<<endl;
    for(int i=0;i<10;i++){
      for(int j=0;j<dim;j++){
        weightOutput<<weights[i][j]<<",";
      }weightOutput<<endl;
    }
    weightOutput<<"\n";
    myOutput.close();
    weightOutput.close();
  }
  for(int i=0;i<10;i++){
    delete weights[i];
  }
  if(dataSet==2)cout<<"ACCURACY: "<<accu[L]<<endl;
  delete weights;
  delete valid;
  return L;
}

int main(int argc,char* argv[]){
  ifstream myInput;
  op_file = argv[4];
  if(argc>5)
    op_weight = argv[5];

  //temp variables
  vector<double> tempV;
  string pix;
  int tempInt =1;
  char_separator<char> sep(",");
  weightOutput.open(op_weight);
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
  int start=clock();
  int lambda_index =regression(1,-1);
  data[0].insert(data[0].end(),data[1].begin(),data[1].end());
  regression(2,lambda_index);
  int stop=clock();
  //cout<<"time: "<<(stop-start)/double(CLOCKS_PER_SEC)<<endl;
//  cout<<"lamda= "<<lambda[lambda_index];

}
