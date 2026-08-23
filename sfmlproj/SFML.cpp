#include <iostream>
#include <vector>
#include <random>
#include <chrono>
#include <SFML/Graphics.hpp>
#include <string>
#include <sstream>
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics/VertexArray.hpp>
#include <SFML/Graphics/Shader.hpp>
#include <SFML/System/Clock.hpp>
#include <SFML/System/Time.hpp>
enum class directions { up, down, left, right };
directions dir = directions::right;



sf::Color HSVToRGB(float h, float s, float v) {
	float hPrime = h / 60.0f;
	unsigned int hIndex = unsigned int(hPrime) % 6;
	float chroma = s * v;
	float min = (v - chroma);
	float x = chroma * (1.0f - abs(fmod(hPrime, 2.0f) - 1.0f));
	float outRGB[6][3] = {

		{chroma, x, 0.0f},
		{x, chroma, 0.0f},
		{0.0f, chroma, x},
		{0.0f, x, chroma},
		{x, 0.0f, chroma},
		{chroma, 0.0f, x}

	};
	float rF = (outRGB[hIndex][0] + min);
	float gF = (outRGB[hIndex][1] + min);
	float bF = (outRGB[hIndex][2] + min);
	rF *= 255;
	gF *= 255;
	bF *= 255;
	std::uint8_t rI = std::uint8_t(rF);
	std::uint8_t gI = std::uint8_t(gF);
	std::uint8_t bI = std::uint8_t(bF);
	return sf::Color(rI, gI, bI);
}
void PollEvents(sf::RenderWindow& window, directions &dir) {
	while (const std::optional event = window.pollEvent()) {
		if (event->is<sf::Event::Closed>()) {
			window.close();
		}
		else if (const auto* keyPressed = event->getIf<sf::Event::KeyPressed>()) {
			if (keyPressed->scancode == sf::Keyboard::Scancode::Escape) {
				window.close();
			}
		}
		if (const auto* keyPressed = event->getIf<sf::Event::KeyPressed>()) {
			if (keyPressed->scancode == sf::Keyboard::Scancode::W && dir != directions::down) {
				dir = directions::up;
			}
			else if (keyPressed->scancode == sf::Keyboard::Scancode::S && dir != directions::up) {
				dir = directions::down;
			}
			else if (keyPressed->scancode == sf::Keyboard::Scancode::A && dir != directions::right) {
				dir = directions::left;
			}
			else if (keyPressed->scancode == sf::Keyboard::Scancode::D && dir != directions::left) {
				dir = directions::right;
			}
		}
	}
}

int main() {


	const unsigned int cellSize = 20;
	const unsigned int gridWidth = 20;
	const unsigned int gridHeight = 30;
	sf::Vector2u windowSize = { gridWidth * cellSize, gridHeight * cellSize };
	sf::VideoMode videomode = sf::VideoMode(windowSize);
	std::string title = "SFMLL";
	sf::RenderWindow window = sf::RenderWindow(sf::VideoMode({ gridWidth * cellSize, gridHeight * cellSize }), title);
	window.setFramerateLimit(60);

	std::default_random_engine randeng;
	randeng.seed(std::chrono::steady_clock::now().time_since_epoch().count());
	std::uniform_int_distribution<int> foopos;
	foopos = std::uniform_int_distribution<int>(0, gridWidth - 1);
	std::uniform_int_distribution<int> foopos2;
	foopos2 = std::uniform_int_distribution<int>(0, gridHeight - 1);

	std::vector<sf::Vector2i> snake = { {5,5}, {4, 5}, {3, 5} };
	sf::Vector2i food = { foopos(randeng), foopos2(randeng) };

	sf::RectangleShape seg({ static_cast<float>(cellSize - 1), static_cast<float>(cellSize - 1) });
	seg.setFillColor(sf::Color(76, 175, 80));

	sf::RectangleShape food2({ static_cast<float>(cellSize - 1), static_cast<float>(cellSize - 1) });
	food2.setFillColor(sf::Color::Red);
	

	sf::Clock cloak;
	float timok = 0.0f;
	float timokDelay = 0.15f;

	unsigned int score = 0;

	while (window.isOpen()) {


		//Updating

		PollEvents(window, dir);

		float time = cloak.restart().asSeconds();
		timok += time;
		if (timok > timokDelay) {
			
			timok = 0.0f;
			for (std::size_t i = snake.size() - 1; i > 0; --i) {
				if (snake[0] == snake[i]) {

					snake = { {5,5}, {4, 5}, {3, 5} };
					dir = directions::right;
					score = 0;
					std::cerr << "YOU LOSE!\n";
					break;

				}
				if (snake[0].x < 0 || snake[0].x >= gridWidth || snake[0].y < 0 || snake[0].y >= gridHeight) {

					snake = { {5,5}, {4, 5}, {3, 5} };
					dir = directions::right;
					score = 0;
					std::cerr << "YOU LOSE!\n";
					break;
					
				}
				snake[i] = snake[i - 1];
			}
			if (dir == directions::up) snake[0].y -= 1;
			if (dir == directions::down) snake[0].y += 1;
			if (dir == directions::left) snake[0].x -= 1;
			if (dir == directions::right) snake[0].x += 1;
			if (snake[0] == food) {

				snake.push_back(snake.back());

				food = { foopos(randeng), foopos2(randeng) };
				
				score++;

				std::cerr << "Your score: " << score << '\n';

			}

		}

		



		//Render

		window.clear(sf::Color::Black);

		// Draw

		for (int i = 0; i < snake.size(); i++) {

			seg.setPosition({ static_cast<float>(snake[i].x * cellSize), static_cast<float>(snake[i].y * cellSize) });
			window.draw(seg);

		}
		food2.setPosition({ static_cast<float>(food.x * cellSize), static_cast<float>(food.y * cellSize) });
		window.draw(food2);
		
		// Display screen

		window.display();
	}
	return 0;
}