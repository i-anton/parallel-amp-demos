#pragma once
#include "stdafx.h"
#include "logic.h"

constexpr int WINDOW_SIZE = 640;
constexpr int FIELD_SIZE = 64;
constexpr int PIXEL_SIZE = WINDOW_SIZE / FIELD_SIZE;

class UI {
	bool paused;
	sf::RenderWindow window;
	sf::RectangleShape brush;
	std::unique_ptr<std::vector<PixelData>> state;
	std::unique_ptr<std::vector<PixelData>> state_double;
	void handle_input(const sf::Event::KeyEvent& evt);
	void handle_mouse();
	void update();
	void render();
public:
	UI();
	void start();
};