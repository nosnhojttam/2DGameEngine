
#include "../includes/animation.h"

animation::animation()
{

}

void animation::setState(State state)
{
	currentState = state;
}

void animation::setDirection(Direction direction)
{
	currentDirection = direction;
}

float animation::getX(long elapsed)
{
	if (currentState == IDLE)
		return 0;
	else
		return (elapsed / 100000) % stateCount;
}

float animation::getY(long elapsed)
{
	return (float)currentDirection;
}