#include "Mouse.h"

std::pair<float, float> Mouse::GetPos() const
{
	return { x, y };
}

float Mouse::GetX() const
{
	return x;
}

float Mouse::GetY() const
{
	return y;
}

bool Mouse::LeftPressed() const
{
	return bLeftPressed;
}

bool Mouse::RightPressed() const
{
	return bRightPressed;
}

bool Mouse::MiddlePressed() const
{
	return bMiddlePressed;
}

bool Mouse::InWindow() const
{
	return bInWindow;
}

Mouse::Event Mouse::Read()
{
	if (buffer.size())
	{
		Mouse::Event e = buffer.front();
		buffer.pop();
		return e;
	}
	return Mouse::Event();
}

void Mouse::flush()
{
	buffer = std::queue<Event>();
}

void Mouse::OnMouseLeave()
{
	bInWindow = false;
}

void Mouse::OnMouseEnter()
{
	bInWindow = true;
}

void Mouse::OnMouseMove(float nx, float ny)
{
	x = nx;
	y = ny;
	buffer.push(Mouse::Event(Mouse::Event::Type::Move, *this));
	TrimBuffer();
}

void Mouse::OnLeftPressed(float x, float y)
{
	bLeftPressed = true;
	buffer.push(Mouse::Event(Mouse::Event::Type::LPress, *this));
	TrimBuffer();
}

void Mouse::OnLeftReleased(float x, float y)
{
	bLeftPressed = false;
	buffer.push(Mouse::Event(Mouse::Event::Type::LRelease, *this));
	TrimBuffer();
}

void Mouse::OnRightPressed(float x, float y)
{
	bRightPressed = true;
	buffer.push(Mouse::Event(Mouse::Event::Type::RPress, *this));
	TrimBuffer();
}

void Mouse::OnRightReleased(float x, float y)
{
	bRightPressed = false;
	buffer.push(Mouse::Event(Mouse::Event::Type::RRelease, *this));
	TrimBuffer();
}

void Mouse::OnMiddlePressed(float x, float y)
{
	bMiddlePressed = true;
	buffer.push(Mouse::Event(Mouse::Event::Type::MPress, *this));
	TrimBuffer();
}

void Mouse::OnMiddleReleased(float x, float y)
{
	bMiddlePressed = false;
	buffer.push(Mouse::Event(Mouse::Event::Type::MRelease, *this));
	TrimBuffer();
}

void Mouse::OnWheelUp(float x, float y)
{
	buffer.push(Mouse::Event(Mouse::Event::Type::WheelUp, *this));
	TrimBuffer();
}

void Mouse::OnWheelDown(float x, float y)
{
	buffer.push(Mouse::Event(Mouse::Event::Type::WheelDown, *this));
	TrimBuffer();
}

void Mouse::TrimBuffer()
{
	while (buffer.size() > bufferSize)
	{
		buffer.pop();
	}
}