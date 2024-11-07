#include <iostream>
using namespace std;
class Card {
public:
	string suit; //카드 문양
	string rank; //카드 숫자 또는 영어 값
	int value; // 카드 점수 값
	string type; // "number" 또는 "operation", 일반 카드와 사칙연산 카드 구별

	Card(string s, string r, int v, string t) : suit(s), rank(r), value(v), type(t) {} 
};

void main(void) {

}
