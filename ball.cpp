#include "ball.h"

// Create the ball
Ball::Ball(float radius){
Ball:createBall(radius);
}

void Ball::createBall(float radius){
	sf::CircleShape ball;
	ball.setRadius(radius);
	ball.setFillColor(sf::Color::White);
	ball.setOrigin(radius / 2, radius / 2);

	this->ball = ball;
	this->ballPosition = ball.getPosition();
	this->radius = radius;
}

sf::CircleShape Ball::getShape(){
	return this->ball;
}
