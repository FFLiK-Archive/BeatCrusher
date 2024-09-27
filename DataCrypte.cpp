#include <iostream>
#include <Windows.h>
using namespace std;
#define CODE 423947821;

int IntDataEncrypter(int data) {
	data ^= CODE;
	data *= 2;
	data -= CODE;
	data ^= CODE;
	return data;
}

int IntDataDecrypter(int data) {
	data ^= CODE;
	data += CODE;
	if (data % 2)
		return 0;
	data /= 2;
	data ^= CODE;
	return data;
}

int main(){
	int d;
	cin>>d;
	
	cout<<IntDataEncrypter(d) << endl;
	cout<<IntDataDecrypter(d) << endl;
	
	while(1){
		Sleep(1000);
	}
	return 0;
}
