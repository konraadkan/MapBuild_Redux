#include "keyboard.h"

bool Keyboard::KeyIsPressed(unsigned char code) const
{
	return m_Keystates[code];
}

Keyboard::Event Keyboard::ReadKey()
{
	if (m_Keybuffer.size() > 0u)
	{
		Keyboard::Event e = m_Keybuffer.front();
		m_Keybuffer.pop();
		return e;
	}
	return Keyboard::Event();
}

bool Keyboard::KeyIsEmpty() const
{
	return m_Keybuffer.empty();
}

wchar_t Keyboard::ReadChar()
{
	if (m_Charbuffer.size() > 0u)
	{
		wchar_t code = m_Charbuffer.front();
		m_Charbuffer.pop();
		return code;
	}
	return 0;
}

bool Keyboard::CharIsEmpty() const
{
	return m_Charbuffer.empty();
}

void Keyboard::FlushKey()
{
	m_Keybuffer = std::queue<Event>();
}

void Keyboard::FlushChar()
{
	m_Charbuffer = std::queue<wchar_t>();
}

void Keyboard::Flush()
{
	FlushKey();
	FlushChar();
}

void Keyboard::EnableAutorepeat()
{
	bAutorepeatEnabled = true;
}

void Keyboard::DisableAutorepeat()
{
	bAutorepeatEnabled = false;
}

bool Keyboard::AutorepeatEnabled() const
{
	return bAutorepeatEnabled;
}

void Keyboard::OnKeyPressed(unsigned char code)
{
	m_Keystates[code] = true;
	m_Keybuffer.push(Keyboard::Event(Keyboard::Event::Type::Press, code));
	TrimBuffer(m_Keybuffer);
}

void Keyboard::OnKeyReleased(unsigned char code)
{
	m_Keystates[code] = false;
	m_Keybuffer.push(Keyboard::Event(Keyboard::Event::Type::Release, code));
	TrimBuffer(m_Keybuffer);
}

void Keyboard::OnChar(wchar_t character)
{
	m_Charbuffer.push(character);
	TrimBuffer(m_Charbuffer);
}

void Keyboard::ClearState()
{
	m_Keystates.reset();
}

template<typename T> void Keyboard::TrimBuffer(std::queue<T>& buffer)
{
	while (buffer.size() > m_BufferSize)
	{
		buffer.pop();
	}
}