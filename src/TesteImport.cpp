#include <iostream>
#include <string>
#include <cmath>
#include <vector>
using namespace std;
#include <fstream>

void readMap (string filename) {
	
	ifstream file(filename);
	string currentEl;
	vector<string> fileContent;

	//ENQUANTO CURRENTEL NÃO RECEBER EL CONTINUA A ADICIONAR ITENS NO FILECONTENT
	while(file >> currentEl){
		fileContent.push_back(currentEl);
	}

	//PRINTA FILECONTENT
	for(int i = 0; i < fileContent.size(); i++){
		cout << "file: " << fileContent[i] << endl;
	}

	file.close();


}

/*****MAIN*****/
int main(){

	readMap("../src/tilemap.txt");

}