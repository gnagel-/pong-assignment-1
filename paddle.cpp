#include <SFML/Graphics.hpp>
#include "paddle.h";

Paddle::Paddle(sf::Vector2f ps, sf::Color pc, sf::Vector2f pp){
	createPaddle(ps, pc, pp);
}

//create the paddle
sf::RectangleShape Paddle::createPaddle(sf::Vector2f pSize, sf::Color pColor, sf::Vector2f pPos)
{
	sf::RectangleShape paddle(pSize);
	paddle.setOrigin(pSize / 2.f);
	paddle.setFillColor(pColor);
	paddle.setPosition(pPos);

	this->paddle = paddle;
	this->paddleSize = pSize;
	this->paddleColor = pColor;
	this->paddlePosition = pPos;
	
	return paddle;
}


sf::RectangleShape Paddle::getShape(){
	return this->paddle;
}



