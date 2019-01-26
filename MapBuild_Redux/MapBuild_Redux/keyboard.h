#pragma once
#include <queue>
#include <bitset>

class Keyboard
{
	friend class Window;
public:
	class Event
	{
	public:
		enum class Type
		{
			Press,
			Release,
			Invalid
		};
	private:
		Type m_Type;
		unsigned char m_Code;
	public:
		Event() : m_Type(Type::Invalid), m_Code(m_Code) {}
		Event(Type type, unsigned char code) : m_Type(type), m_Code(code) {}
		bool IsPress() const { return m_Type == Type::Press; }
		bool IsRelease() const { return m_Type == Type::Release; }
		bool IsInvalid() const { return m_Type == Type::Invalid; }
		unsigned char GetCode() const { return m_Code; }
	};
public:
	Keyboard() = default;
	Keyboard(const Keyboard&) = delete;
	Keyboard& operator=(const Keyboard&) = delete;
	bool KeyIsPressed(unsigned char code) const;
	Event ReadKey();
	bool KeyIsEmpty() const;
	wchar_t ReadChar();
	bool CharIsEmpty() const;
	void FlushKey();
	void FlushChar();
	void Flush();
	void EnableAutorepeat();
	void DisableAutorepeat();
	bool AutorepeatEnabled() const;
private:
	void OnKeyPressed(unsigned char code);
	void OnKeyReleased(unsigned char code);
	void OnChar(wchar_t character);
	void ClearState();
	template<typename T> void TrimBuffer(std::queue<T>& buffer);
private:
	static constexpr unsigned int m_nKeys = 256u;
	static constexpr unsigned int m_BufferSize = 4u;
	bool bAutorepeatEnabled = false;
	std::bitset<m_nKeys> m_Keystates;
	std::queue<Event> m_Keybuffer;
	std::queue<wchar_t> m_Charbuffer;
};