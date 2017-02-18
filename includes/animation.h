#pragma once
#include <iostream>
using namespace std;


class animation
{
	public:
		animation();
		const enum Direction {DOWN,LEFT,RIGHT,UP};
		const enum State { WALK,IDLE };
		void setState(State state);
		void setDirection(Direction direction);
		float getX(long elapsed);
		float getY(long elapsed);
	private:
		State currentState;
		Direction currentDirection;
		int stateCount = 4;
		

};

