#include "ui.h"

constexpr auto shadow_row_size = FIELD_SIZE + 2;
constexpr auto end = FIELD_SIZE + 1;

void UI::handle_input(const sf::Event::KeyEvent& evt) {
	if (evt.code == sf::Keyboard::Space)
		paused = !paused;
	else if (evt.code == sf::Keyboard::Escape)
		window.close();
}

void UI::handle_mouse() {
	const auto isLeftPressed = sf::Mouse::isButtonPressed(sf::Mouse::Button::Left);
	const auto isRightPressed = sf::Mouse::isButtonPressed(sf::Mouse::Button::Right);
	if (!(isLeftPressed || isRightPressed)) return;
	const auto mousePosition = sf::Mouse::getPosition(window);
	const auto coords = window.mapCoordsToPixel(sf::Vector2f(
		static_cast<float>(mousePosition.x),
		static_cast<float>(mousePosition.y)
	));
	if (coords.x >= WINDOW_SIZE || coords.x < 0 || coords.y >= WINDOW_SIZE || coords.y < 0)
		return;
	const auto local_coords = sf::Vector2i(coords.x / PIXEL_SIZE, coords.y / PIXEL_SIZE);
	const auto field_idx = (local_coords.x + 1) + (local_coords.y + 1) * shadow_row_size;
	if (isLeftPressed)
		(*shadow_state)[field_idx] = 1;
	else if (isRightPressed)
		(*shadow_state)[field_idx] = 0;
}

void UI::update() {
	logic::parallel_branchless_shared<8>(*shadow_state, shadow_row_size, *shadow_state_double, 1);
}

void UI::render() {
	window.clear();
	const auto& pixels = *shadow_state;
	for (size_t y = 1; y < end; y++) {
		const auto y_offset = y * shadow_row_size;
		for (size_t x = 1; x < end; x++) {
			if (pixels[y_offset + x] == 0)
				continue;
			brush.setPosition(
				static_cast<float>((x - 1) * PIXEL_SIZE),
				static_cast<float>((y - 1) * PIXEL_SIZE)
			);
			window.draw(brush);
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
	shadow_state(new std::vector<PixelData>(shadow_row_size* shadow_row_size)),
	shadow_state_double(new std::vector<PixelData>(shadow_row_size* shadow_row_size))
{
	brush.setFillColor(sf::Color::Green);
	brush.setOutlineThickness(1.0f);
	brush.setOutlineColor(sf::Color::Black);
}

void UI::start() {
	while (window.isOpen()){
		sf::Event event;
		while (window.pollEvent(event)) {
			if (event.type == sf::Event::Closed)
				window.close();
			else if (event.type == sf::Event::KeyPressed)
				handle_input(event.key);
		}
		handle_mouse();
		if (!paused)
			update();
		render();
	}
}
