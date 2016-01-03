#include <iostream>
#include <algorithm>
#include <vector>
#include <fstream>
#include <thread>
using namespace std;

struct matrix{
	vector <double> coeff;
};

void swapLine(int i, int j, vector <matrix*> &adress)
{
	matrix *help;
	help = adress[i];
	adress[i] = adress[j];
	adress[j] = help;
}

vector <matrix*> readFromFile(int &numberOfCores, int &matrixSize, double &eps)
{
	vector <matrix*> adress;
	ifstream fileInMatrix;
	double element;
	matrix *head = new matrix;
	adress.push_back(head);
	fileInMatrix.open("inputMatrix.txt");
	ifstream fileInProc("inputProc.txt");
	fileInMatrix >> matrixSize >> eps;
	for (int i = 0; i < matrixSize; i++)
	{
		if (i) 
		{
			head = new matrix;
			adress.push_back(head);
		}
		for (int j = 0; j <= matrixSize; j++)
		{
			fileInMatrix >> element;
			adress[i]->coeff.push_back(element);
		}
	}
	fileInProc >> numberOfCores;
	fileInMatrix.close();
	fileInProc.close();
	return adress;
}

void minusLineA_B(vector <matrix*> &adress, int A, int B, double eps)
{
	if (B < adress.size()&&A<adress.size())
	{
		double coefMult = adress[A]->coeff[B];
		double coefDel = adress[B]->coeff[B];

		for (int i = 0; i < adress[A]->coeff.size(); i++)
		{
			adress[A]->coeff[i] -= adress[B]->coeff[i] * coefMult / coefDel;
		}
	}
}

bool findMax(vector <matrix*> &adress, int line,double eps)
{
	double maxEl = adress[line]->coeff[line];
	int maxLine = line;
	for (int i = line; i < adress.size(); i++)
	{
		if (adress[i]->coeff[line]>maxEl)
		{
			maxEl = adress[i]->coeff[line];
			maxLine = i;
		}
	}
	if (maxEl < eps) return false;
	swapLine(line, maxLine, adress);
}

vector <double> multithreading(vector <matrix*> adress, double eps, int numberOfCores)
{
	vector <double> ans;
	for (int i = 0; i < adress.size(); i++)
	{
		if (!findMax(adress, i, eps))
		{
	//		return ans;
		}
		int j = i+1;
		while (j < adress.size())
		{
			if (numberOfCores == 1)
			{
				minusLineA_B(adress, j, i, eps);
			}
			if (numberOfCores == 2)
			{
				thread thr1(minusLineA_B, ref(adress), j + 1, i, eps);
				minusLineA_B(adress, j, i, eps);
				thr1.join();
			}
			if (numberOfCores == 3)
			{
				thread thr1(minusLineA_B, ref(adress), j + 1, i, eps);
				thread thr2(minusLineA_B, ref(adress), j + 2, i, eps);
				minusLineA_B(adress, j, i, eps);
				thr1.join();				
				thr2.join();
			}
			if (numberOfCores == 4)
			{
				thread thr1(minusLineA_B, ref(adress), j + 1, i, eps);
				thread thr2(minusLineA_B, ref(adress), j + 2, i, eps);
				thread thr3(minusLineA_B, ref(adress), j + 3, i, eps);
				minusLineA_B(adress, j, i, eps);
				thr1.join();				
				thr2.join();
				thr3.join();
			}
			j += numberOfCores;
		}
	}
	for (int i = adress.size()-1; i >= 0; i--)
	{
		ans.push_back(adress[i]->coeff[adress.size()]);
		for (int j=adress.size()-1;j>i;j--)
			ans[ans.size() - 1] -= ans[adress.size()-1-j] * adress[i]->coeff[j];
		ans[ans.size() - 1] /= adress[i]->coeff[i];
	}
	return ans;
}

void writeAns(ofstream &out, vector <double> ans, double eps)
{
	int count = 0;
	double k = eps;
	while (k < 1)
	{
		count++;
		k *= 10;
	}
	for (int i = ans.size() - 1; i >= 0; i--)
	{
		if (ans[i] < eps)ans[i] = 0;
		out.precision(count);
		out << "X" << ans.size() - i << "=" << ans[i] << endl;
	}
}

int main()
{
	int matrixSize = 0, numberOfCores = 0;
	double eps = 0;
	ofstream out("output.txt");
	vector <matrix*> adress = readFromFile(numberOfCores, matrixSize, eps);
	vector <double> ans = multithreading(adress, eps, numberOfCores);
	if (ans.size() == 0) out << "ERROR\n";
	else writeAns(out, ans,eps);
	out.close();
	return 0;
}