#include "ui.h"

constexpr auto end = FIELD_SIZE - 1;
constexpr auto shadow_row_size = FIELD_SIZE + 2;

void UI::copy_state_from_shadow()
{
	const auto& shadow = *shadow_state;
	const auto st = &((*state)[0]);
	for (size_t y = 1; y < FIELD_SIZE; y++)
		for (size_t x = 1; x < FIELD_SIZE; x++) {
			st[(x - 1) + (y - 1) * FIELD_SIZE] = shadow[x + y * shadow_row_size];
		}
}

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
	const auto field_idx = (local_coords.x + 1) + (local_coords.y + 1) * shadow_row_size;
	(*shadow_state)[field_idx] = (*shadow_state)[field_idx] == 0 ? 1 : 0;
	copy_state_from_shadow();
}

void UI::update() {
	logic::parallel_branchless_shared<8>(*shadow_state, shadow_row_size, *shadow_state_double);
	// TODO: remove copy 
	copy_state_from_shadow();
}

void UI::render() {
	window.clear();
	const auto& pixels = (*state);
	int x_idx = 0;
	int y_idx = 0;
	for (size_t i = 0; i < pixels.size(); i++)
	{
		if (pixels[i] != 0) {
			brush.setPosition(
				static_cast<float>(x_idx * PIXEL_SIZE),
				static_cast<float>(y_idx * PIXEL_SIZE)
			);
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
		"Game of life demo",
		sf::Style::Titlebar | sf::Style::Close),
	brush(sf::Vector2f(PIXEL_SIZE, PIXEL_SIZE)),
	state(new std::vector<PixelData>(FIELD_SIZE * FIELD_SIZE)),
	shadow_state(new std::vector<PixelData>(shadow_row_size* shadow_row_size)),
	shadow_state_double(new std::vector<PixelData>(shadow_row_size* shadow_row_size))
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
