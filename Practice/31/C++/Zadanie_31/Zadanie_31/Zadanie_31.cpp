#include <iostream>
#include <vector>
#include <fstream>
using namespace std;

int pass = 0;

//ostream& operator<< (ostream& out, const vector<int> &vect) {
ostream& operator<< (ostream& out, const vector<int> vect) {
	//out << vect.size() << "\t" << "| ";
	out << "(" << vect.size() << ", " << pass << ")" << ": [";
	//for (int i = 0; i < vect.size(); i++) out << &vect[i] << " ";
	for (int i = 0; i < vect.size(); i++)
		out << "0x" << &vect[i] << ((i < (vect.size() - 1)) ? ", " : "");
	out << "],";
	return out;
}

int main()
{
	vector<int> vect;
	ofstream data_txt("data.txt");

	for (int i = 0; i < 64; i++) {
		vect.push_back(i);
		data_txt << vect << endl;
	}
	pass = 1;
	while (vect.size()) {
		vect.pop_back();
		data_txt << vect << endl;
	}
}
