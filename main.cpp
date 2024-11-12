#include <SFML/Graphics.hpp>
#include <iostream>
#include <vector>

using namespace std;
class Card {
public:
    string suit; //카드 문양
    string rank; //카드 숫자 또는 영어 값
    int value; // 카드 점수 값
    string type; // "number" 또는 "operation", 일반 카드와 사칙연산 카드 구별

    Card(string s, string r, int v, string t) : suit(s), rank(r), value(v), type(t) {}
};

class Deck {
public:

private:
    vector<Card> card;
};

int main() {

    sf::RenderWindow window(sf::VideoMode(800, 600), "SFML Test Window");

    // 메인 루프
    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            // 창 닫기 이벤트 처리
            if (event.type == sf::Event::Closed)
                window.close();
        }

        // 화면을 흰색으로 클리어
        window.clear(sf::Color::White);

        // 창을 갱신
        window.display();
    }

	return 0;
}
