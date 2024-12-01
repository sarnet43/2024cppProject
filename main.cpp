﻿#include <SFML/Graphics.hpp>
#include <iostream>
#include <vector>
#include <ctime>
#include <cstdlib>
#include <algorithm>
#include <string>
#include <map>

using namespace std;

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
            string suits[] = { "H", "D", "C", "S" }; //H = Heart, D = Diamond, C = Club, S = Spades
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
    int cardCount = 0;
    void addCard(Card card) {
        hand.push_back(card); // 카드 추가
        cardCount++;          // 받은 카드 개수 증가
        if (cardCount <= 2) { // 처음 2장만 점수에 더함
            score += card.value;
        }
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
        if (calculateScore() < 31) { 
            addCard(deck.drawCard());
        }
    }
};

class BlackJack {
public:
    BlackJack(sf::RenderWindow& gameWindow)
        : window(gameWindow),
        hitButton("images/hitButton.png", 140.f, 1200.f),
        stayButton("images/stayButton.png", 450.f, 1200.f),
        plusButton("images/plusButton.png", 50.f, 1000.f),
        minusButton("images/subButton.png", 250.f, 1000.f),
        divideButton("images/divButton.png", 450.f, 1000.f),
        multiplyButton("images/mulButton.png", 650.f, 1000.f),
        selectedOperation("+"), playerWin(), playerLost(), deadHeat() {
        if (!font.loadFromFile("font/AggroM.ttf")) {
            cerr << "Error loading font" << endl;
        }
        playerWin.loadFromFile("images/youwin.png");
        playerLost.loadFromFile("images/youlost.png");
        deadHeat.loadFromFile("images/deadheat.png");

        // 덱 섞기
        deck.shuffle();

        // 초기 카드 분배
        player.addCard(deck.drawCard()); //플레이어 2장
        player.addCard(deck.drawCard()); 
        dealer.addCard(deck.drawCard()); // 딜러 2장씩 배분
        dealer.addCard(deck.drawCard());
    }

    void play() {

        while (window.isOpen()) {
            sf::Event event;
            while (window.pollEvent(event)) {
                if (event.type == sf::Event::Closed) window.close();

                if (hitButton.isClicked(window, event)) handleHit();
                else if (stayButton.isClicked(window, event)) handleStay();
                else if (plusButton.isClicked(window, event)) selectedOperation = "plus";
                else if (minusButton.isClicked(window, event)) selectedOperation = "minus";
                else if (divideButton.isClicked(window, event)) selectedOperation = "division";
                else if (multiplyButton.isClicked(window, event)) selectedOperation = "times";
            }//while event
            window.clear(sf::Color(165, 182, 141));
            //버튼 표시
            hitButton.draw(window);
            stayButton.draw(window);
            plusButton.draw(window);
            minusButton.draw(window);
            divideButton.draw(window);
            multiplyButton.draw(window);

            player.showHand(window, textures, 650.f);
            dealer.showHand(window, textures, 150.f);

            // 점수 텍스트
            sf::Text playerScoreText("Player Score: " + to_string(player.calculateScore()), font, 30);
            playerScoreText.setFillColor(sf::Color::Black);
            playerScoreText.setPosition(50, 500);
            window.draw(playerScoreText);

            sf::Text dealerScoreText("Dealer Score: " + to_string(dealer.calculateScore()), font, 30);
            dealerScoreText.setFillColor(sf::Color::Black);
            dealerScoreText.setPosition(50, 50);
            window.draw(dealerScoreText);

            window.display();
        }//while isOpen
    }

private:
    sf::RenderWindow& window; 
    sf::Font font;
    sf::Texture playerWin;
    sf::Texture playerLost;
    sf::Texture deadHeat;
    ImageButton hitButton, stayButton, plusButton, minusButton, divideButton, multiplyButton; //사칙연산 버튼
    Deck deck;
    Player player; // 플레이어 객체
    Dealer dealer; // 딜러 객체
    string selectedOperation; //선택한 사칙연산
    map<string, sf::Texture> textures;

    void handleHit() {
        Card newCard = deck.drawCard();

        if (selectedOperation == "plus") player.score += newCard.value;
        else if (selectedOperation == "minus") player.score -= newCard.value;
        else if (selectedOperation == "times") player.score *= newCard.value;
        else if (selectedOperation == "division" && newCard.value != 0) player.score /= newCard.value;

        // 플레이어 카드에 추가
        player.addCard(newCard);
        dealer.playTurn(deck);  //딜러의 턴 시작
    }

    void handleStay() {
        int playerFinalScore =  player.calculateScore();
        int dealerFinalScore =  dealer.calculateScore();
        sf::Sprite result;

        dealer.playTurn(deck);

        if (playerFinalScore > 31) {
            // 플레이어 점수가 31을 초과하면 무조건 패배
            result.setTexture(playerLost);
            result.setPosition(window.getSize().x / 2 - 250, 400);
        }
        else if (dealerFinalScore > 31) {
            // 딜러 점수가 31을 초과하면 플레이어 승리
            result.setTexture(playerWin);
            result.setPosition(window.getSize().x / 2 - 250, 400);
        }
        else if (playerFinalScore > dealerFinalScore) {
            // 점수가 31 이하이고, 플레이어가 더 높은 점수라면 승리
            result.setTexture(playerWin);
            result.setPosition(window.getSize().x / 2 - 250, 400);
        }
        else if (playerFinalScore < dealerFinalScore) {
            // 점수가 31 이하이고, 딜러가 더 높은 점수라면 패배
            result.setTexture(playerLost);
            result.setPosition(window.getSize().x / 2 - 250, 400);
        }
        else {
            // 점수가 동점인 경우 무승부
            result.setTexture(deadHeat);
            result.setPosition(window.getSize().x / 2 - 250, 400);
        }

        //결과 이미지를 띄우고 2초 뒤 종료
        sf::Clock clock;
        while (clock.getElapsedTime().asSeconds() < 2.f) {
            window.draw(result);
            window.display();
        }

        window.close(); // 게임 종료
    }
};

int main() {
    bool showGuide = false; 
    // 창 생성
    sf::RenderWindow window(sf::VideoMode(800, 1400), "Blackjack Game");
    BlackJack game(window);

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
    howtoplay.setPosition(window.getSize().x / 2 - 325, 200);

    // 메인 루프
    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close(); // 창 닫기
            
            if (startButton.isClicked(window, event)) game.play();
            // guideButton 클릭 시 설명 이미지 열기
            if (!showGuide && guideButton.isClicked(window, event)) showGuide = true;
            
            if (showGuide) { //esc키를 누르면 설명 이미지 닫기
                if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Escape) showGuide = false;
            }
        }

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
