#pragma once
#include <d2d1_1.h>
#include <queue>

class Mouse
{
	friend class Window;
public:
	class Event
	{
	public:
		enum class Type
		{
			LPress,
			RPress,
			MPress,
			X1Press,
			X2Press,
			LRelease,
			RRelease,
			MRelease,
			X1Release,
			X2Release,
			WheelUp,
			WheelDown,
			Move,
			Invalid
		};
	private:
		Type m_Type;
		bool bLeftPressed = false;
		bool bRightPressed = false;
		bool bMiddlePressed = false;
		bool bX1Pressed = false;
		bool bX2Pressed = false;
		float x = 0.0f;
		float y = 0.0f;
	public:
		Event() : m_Type(Type::Invalid), bLeftPressed(false), bRightPressed(false), bMiddlePressed(false), x(0), y(0) {}
		Event(Type type, const Mouse& parent) : m_Type(type), bLeftPressed(parent.LeftPressed()), bRightPressed(parent.RightPressed()), bMiddlePressed(parent.MiddlePressed()), x(parent.x), y(parent.y) {}
		bool IsValid() const
		{
			return m_Type != Type::Invalid;
		}
		Type GetType() const
		{
			return m_Type;
		}
		D2D1_POINT_2F GetPos() const
		{
			return D2D1::Point2F(x, y);
		}
		float GetPosX() const
		{
			return x;
		}
		float GetPosY() const
		{
			return y;
		}
		bool LeftPressed() const
		{
			return bLeftPressed;
		}
		bool RightPressed() const
		{
			return bRightPressed;
		}
		bool MiddlePressed() const
		{
			return bMiddlePressed;
		}
		bool X1ButtonPressed() const
		{
			return bX1Pressed;
		}
		bool X2ButtonPressed() const
		{
			return bX2Pressed;
		}
	};
public:
	Mouse() = default;
	Mouse(const Mouse&) = delete;
	Mouse& operator=(const Mouse&) = delete;
	std::pair<float, float> GetPos() const;
	float GetX() const;
	float GetY() const;
	bool RightPressed() const;
	bool LeftPressed() const;
	bool MiddlePressed() const;
	bool X1Pressed() const;
	bool X2Pressed() const;
	bool InWindow() const;
	Mouse::Event Read();
	bool IsEmpty() const
	{
		return buffer.empty();
	}
	void flush();
private:
	void OnMouseMove(float x, float y);
	void OnMouseLeave();
	void OnMouseEnter();
	void OnLeftPressed(float x, float y);
	void OnLeftReleased(float x, float y);
	void OnRightPressed(float x, float y);
	void OnRightReleased(float x, float y);
	void OnMiddlePressed(float x, float y);
	void OnMiddleReleased(float x, float y);
	void OnX1Pressed(float x, float y);
	void OnX1Released(float x, float y);
	void OnX2Pressed(float x, float y);
	void OnX2Released(float x, float y);
	void OnWheelUp(float x, float y);
	void OnWheelDown(float x, float y);
	void TrimBuffer();
private:
	static constexpr unsigned int bufferSize = 4u;
	float x = 0.0f;
	float y = 0.0f;
	bool bLeftPressed = false;
	bool bRightPressed = false;
	bool bMiddlePressed = false;
	bool bX1Pressed = false;
	bool bX2Pressed = false;
	bool bInWindow = false;
	std::queue<Event> buffer;
};