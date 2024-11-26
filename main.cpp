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
        srand(time(0)); // 현재 시간을 기준으로 랜덤 시드 설정
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
        hand.push_back(card); //hand에 카드 추가
        score += card.value; //점수에 카드 점수 더하기
    }

    int calculateScore() {
        return score; //현재 점수 반환
    }

    void showHand(sf::RenderWindow& window, map<string, sf::Texture>& textures, float yPos) {
        float xOffset = 50.f; //카드 가로 간격
        for (Card& card : hand) {
            // 카드의 텍스처를 아직 로드하지 않았다면 로드
            if (textures.find(card.imagePath) == textures.end()) {
                sf::Texture texture;
                if (!texture.loadFromFile(card.imagePath)) {
                    cerr << "Error loading image: " << card.imagePath << endl;
                    continue;
                }
                textures[card.imagePath] = texture;
            }

            // 카드 스프라이트 생성 및 위치 설정
            sf::Sprite sprite(textures[card.imagePath]);
            sprite.setPosition(xOffset, yPos);
            window.draw(sprite);
            xOffset += 80.f; // 다음 카드의 가로 위치로 이동
        }
    }

};

class Dealer : public Player {
public:
    void playTurn(Deck& deck) {
        while (calculateScore() < 27) { //점수가 27 미만이면 카드를 뽑음
            addCard(deck.drawCard());
        }
    }
};

class BlackJack {
public:
    BlackJack() {
        deck.shuffle(); // 게임 시작 시 덱을 섞는다.
    }

    void play(sf::RenderWindow& window, sf::Font& font) {
        //플레이어 점수 텍스트
        sf::Text playerScoreText("", font, 20); 
        playerScoreText.setFillColor(sf::Color::White);
        playerScoreText.setPosition(50, 400);

        //딜러 점수 텍스트
        sf::Text dealerScoreText("", font, 20);
        dealerScoreText.setFillColor(sf::Color::White);
        dealerScoreText.setPosition(50, 100);

        while (window.isOpen()) {
            sf::Event event;
            while (window.pollEvent(event)) {
                if (event.type == sf::Event::Closed) {
                    window.close();
                } //if
            }
        }
    }

private:
    Deck deck;

};
class ImageButton {
public:
    ImageButton(const std::string& imagePath, float x, float y) {
        // 텍스처 로드
        if (!buttonTexture.loadFromFile(imagePath)) {
            std::cerr << "이미지를 로드할 수 없습니다: " << imagePath << std::endl;
        }

        // 스프라이트 설정
        buttonSprite.setTexture(buttonTexture);
        buttonSprite.setPosition(x, y);
    }

    // 버튼 그리기
    void draw(sf::RenderWindow& window) {
        window.draw(buttonSprite);
    }

    // 버튼 클릭 여부 확인
    bool isClicked(sf::RenderWindow& window, sf::Event& event) {
        if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left) {
            sf::Vector2i mousePos = sf::Mouse::getPosition(window);
            if (buttonSprite.getGlobalBounds().contains(static_cast<sf::Vector2f>(mousePos))) {
                return true;
            }
        }
        return false;
    }

private:
    sf::Texture buttonTexture;
    sf::Sprite buttonSprite;
};
int main() {
    bool showGuide = false;
    // 창 생성
    sf::RenderWindow window(sf::VideoMode(800, 1400), "게임");

    // 텍스처 객체 생성
    sf::Texture startLogo;
    if (!startLogo.loadFromFile("images/startLogo.png")) {
        cerr << "이미지를 로드할 수 없습니다." << endl;
        return -1; // 이미지 로드 실패 시 프로그램 종료
     }
        // 스프라이트 생성 및 중앙 배치
    sf::Sprite mainLogo(startLogo);
    mainLogo.setPosition((window.getSize().x - mainLogo.getLocalBounds().width) / 2, 300);
    ImageButton startButton("images/startButton.png", window.getSize().x / 2 - 170, 1000);
    ImageButton guideButton("images/guideButton.png", window.getSize().x / 2 - 170, 1150);

    sf::Texture guideImage;
    guideImage.loadFromFile("images/guideImage.png");
    sf::Sprite howtoplay(guideImage);
    howtoplay.setPosition(window.getSize().x / 2 - 325, 300);

    // 메인 루프
    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close(); // 창 닫기

            // guideButton 클릭 시 설명 이미지 열기
            if (!showGuide && guideButton.isClicked(window, event)) {
                showGuide = true;

            }   
            if (showGuide) {
                if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Escape) showGuide = false;
            }
        }
        

        // 창을 흰색으로 초기화
        window.clear(sf::Color(165, 182, 141));

        // 스프라이트를 화면에 그리기
        window.draw(mainLogo);
        startButton.draw(window);
        guideButton.draw(window);
        if (showGuide) window.draw(howtoplay);
        // 화면 갱신
        window.display();
    }
	return 0;
}
