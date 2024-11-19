#include <SFML/Graphics.hpp>
#include <iostream>
#include <vector>
#include <ctime>
#include <cstdlib>
#include <algorithm>
#include <string>
#include <map>

using namespace std;
class Card {
public:
    string suit; //카드 문양
    string rank; //카드 숫자 또는 영어 값
    int value; // 카드 점수 값
    string type; // "number" 또는 "operation", 일반 카드와 사칙연산 카드 구별
    string imagePath; // 이미지 경로

    Card(string s, string r, int v, string t = "number") : suit(s), rank(r), value(v), type(t) {
        if (t == "number") {
            imagePath = "images/card/" + r + s + ".png";
        }
        else {
            imagePath = "images/card/" + r + ".png";
        }
    }
};

class Deck {
public:
    Deck(bool isOperationDeck = false) {
        if (isOperationDeck) {
            cards.push_back(Card("Operations", "+", 0, "operation"));
            cards.push_back(Card("Operations", "-", 0, "operation"));
            cards.push_back(Card("Operations", "*", 0, "operation"));
            cards.push_back(Card("Operations", "/", 0, "operation"));
        }
        else {
            string suits[] = { "H", "D", "C", "S" };
            string ranks[] = { "2", "3", "4", "5", "6", "7", "8", "9", "10", "J", "Q", "K", "A" };
            int values[] = { 2, 3, 4, 5, 6, 7, 8, 9, 10, 10, 10, 10, 11 };

            for (int i = 0; i < 4; i++) {
                for (int j = 0; j < 13; j++) {
                    cards.push_back(Card(suits[i], ranks[j], values[j]));
                }
            }
        }
    }
    void shuffle() {
        srand(time(0));
        random_shuffle(cards.begin(), cards.end());
    }
    Card drawCard() {
        if (!cards.empty()) {
            Card card = cards.back();
            cards.pop_back();
            return card;
        }
        return Card("", "", 0);
    }

    bool isEmpty() {
        return cards.empty();
    }

private:
    vector<Card> cards;
};

class Player {
public:
    vector<Card> hand;
    int score = 0;

    void addCard(Card card) {
        hand.push_back(card);
        score += card.value;
    }

    int calculateScore() {
        return score;
    }

    void showHand(sf::RenderWindow& window, map<string, sf::Texture>& textures, float yPos) {
        float xOffset = 50.f;
        for (Card& card : hand) {
            if (textures.find(card.imagePath) == textures.end()) {
                sf::Texture texture;
                if (!texture.loadFromFile(card.imagePath)) {
                    cerr << "Error loading image: " << card.imagePath << endl;
                    continue;
                }
                textures[card.imagePath] = texture;
            }

            sf::Sprite sprite(textures[card.imagePath]);
            sprite.setPosition(xOffset, yPos);
            window.draw(sprite);
            xOffset += 80.f;
        }
    }

};

class Dealer : public Player {
public:
    void playTurn(Deck& deck) {
        while (calculateScore() < 27) {
            addCard(deck.drawCard());
        }
    }
};


int main() {

    sf::RenderWindow window(sf::VideoMode(1500,1200), "SFML Test Window");

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
