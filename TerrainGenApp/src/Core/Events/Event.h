#pragma once

#include <SDL3/SDL_scancode.h>

enum class EventType
{
	KeyDown,
	KeyUp,
	MouseMove,
	MouseButtonDown,
	MouseButtonUp,
	MouseScroll
};

class Event
{
public:
	virtual ~Event() = default;
	virtual EventType GetType() const = 0;

public:
	bool handled = false;
};

class KeyEvent : public Event
{
public:
	KeyEvent(SDL_Scancode code, bool rep) : code(code), repeat(rep) {}

public:
	SDL_Scancode code;
	bool repeat;
};

class KeyDownEvent : public KeyEvent
{
public:
	KeyDownEvent(SDL_Scancode code, bool rep) : KeyEvent(code, rep) {}
	EventType GetType() const override { return EventType::KeyDown; }
};

class KeyUpEvent : public KeyEvent
{
public:
	KeyUpEvent(SDL_Scancode code, bool rep) : KeyEvent(code, rep) {}
	EventType GetType() const override { return EventType::KeyUp; }
};

class MouseMoveEvent : public Event
{
public:
	MouseMoveEvent(int x, int y, int dx, int dy) : x(x), y(y), dx(dx), dy(dy) {}
	EventType GetType() const override { return EventType::MouseMove; }

public:
	int x, y;
	int dx, dy;
};

class MouseButtonEvent : public Event
{
public:
	MouseButtonEvent(uint8_t button, int x, int y) : button(button), x(x), y(y) {}

public:
	uint8_t button;
	int x, y;
};

class MouseButtonDownEvent : public MouseButtonEvent
{
public:
	MouseButtonDownEvent(uint8_t button, int x, int y) : MouseButtonEvent(button, x, y) {}
	EventType GetType() const override { return EventType::MouseButtonDown; }
};

class MouseButtonUpEvent : public MouseButtonEvent
{
public:
	MouseButtonUpEvent(uint8_t button, int x, int y) : MouseButtonEvent(button, x, y) {}
	EventType GetType() const override { return EventType::MouseButtonUp; }
};

class MouseScrollEvent : public Event
{
public:
	MouseScrollEvent(int xScroll, int yScroll) : xScroll(xScroll), yScroll(yScroll) {}
	EventType GetType() const override { return EventType::MouseScroll; }

public:
	int xScroll, yScroll;
};