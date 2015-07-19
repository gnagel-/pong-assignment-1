#include <SFML/Graphics.hpp>

class Paddle 
{
public:
	sf::RectangleShape paddle;
	sf::Vector2f paddleSize;
	sf::Color paddleColor;
	sf::Vector2f paddlePosition;

	Paddle(sf::Vector2f ps, sf::Color pc, sf::Vector2f pp);

	sf::RectangleShape createPaddle(sf::Vector2f ps, sf::Color pc, sf::Vector2f pp);

	sf::RectangleShape getShape();

	
};