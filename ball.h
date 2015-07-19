#include <SFML/Graphics.hpp>

class Ball
{
public:
	sf::CircleShape ball;
	sf::Vector2f origin;
	sf::Vector2f ballPosition;
	float radius;
	sf::Sound ballSound;

	Ball::Ball(float radius);

	void createBall(float radius);

	sf::CircleShape getShape();


};