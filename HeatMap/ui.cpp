#include "ui.h"

constexpr auto end = FIELD_SIZE - 1;
constexpr auto shadow_row_size = FIELD_SIZE + 2;

void UI::handle_input(const sf::Event::KeyEvent& evt) {
	const auto key = evt.code;
	if (key == sf::Keyboard::Space)
		paused = !paused;
}

void UI::handle_input(const sf::Event::MouseButtonEvent& evt) {
	const auto coords = window.mapCoordsToPixel(sf::Vector2f(
		static_cast<float>(evt.x), static_cast<float>(evt.y)
	));
	if (coords.x >= WINDOW_SIZE || coords.y >= WINDOW_SIZE)
		return;
	const auto local_coords = sf::Vector2i(coords.x / PIXEL_SIZE, coords.y / PIXEL_SIZE);
	const auto field_idx = (local_coords.x) + (local_coords.y) * FIELD_SIZE;
	if (evt.button == sf::Mouse::Button::Left)
		(*heat)[field_idx] = 36;
	else if (evt.button == sf::Mouse::Button::Right)
		(*heat)[field_idx] = 0;
}

void UI::update() {
	const auto input_p = &((*input)[0]);
	const auto output_p = &((*output)[0]);
	const auto size = (*output).size();
	logic::textured(*input, *output, *heat, FIELD_SIZE, 5, 1);
	for (size_t i = 0; i < size; i++)
		input_p[i] = output_p[i];
}

void UI::render() {
	window.clear();
	const auto& pixels = (*output);
	int x_idx = 0;
	int y_idx = 0;
	for (size_t i = 0; i < pixels.size(); i++)
	{
		if (pixels[i] != 0) {
			brush.setPosition(
				static_cast<float>(x_idx * PIXEL_SIZE),
				static_cast<float>(y_idx * PIXEL_SIZE)
			);
			brush.setFillColor(sf::Color(0, pixels[i], 0));
			window.draw(brush);
		}
		x_idx++;
		if (x_idx >= FIELD_SIZE) {
			x_idx = 0;
			y_idx++;
		}
	}
	window.display();
}

UI::UI() :
	paused(true),
	window(
		sf::VideoMode(WINDOW_SIZE, WINDOW_SIZE),
		"HeatDemo",
		sf::Style::Titlebar | sf::Style::Close),
	brush(sf::Vector2f(PIXEL_SIZE, PIXEL_SIZE)),
	input(new std::vector<PixelData>(FIELD_SIZE* FIELD_SIZE)),
	output(new std::vector<PixelData>(FIELD_SIZE* FIELD_SIZE)),
	heat(new std::vector<PixelData>(FIELD_SIZE* FIELD_SIZE))
{
	brush.setFillColor(sf::Color::Green);
	brush.setOutlineThickness(1.0f);
	brush.setOutlineColor(sf::Color::Black);
}

void UI::start() {
	while (window.isOpen()) {
		sf::Event event;
		while (window.pollEvent(event)) {
			if (event.type == sf::Event::Closed)
				window.close();
			else if (event.type == sf::Event::KeyPressed)
				handle_input(event.key);
			else if (event.type == sf::Event::MouseButtonReleased)
				handle_input(event.mouseButton);
		}
		if (!paused)
			update();
		render();
	}
}
