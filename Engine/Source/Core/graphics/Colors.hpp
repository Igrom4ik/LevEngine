#pragma once
#include <cstdint>

namespace LEN
{
	// Simple enum to name a set of predefined colors.
	// The underlying type is uint8_t to keep the storage small.
	// Use `Color::Count` to know how many colors are defined.
	enum class Color : uint8_t
	{
		RED,
		GREEN,
		BLUE,
		WHITE,
		BLACK,
		YELLOW,
		CYAN,
		MAGENTA,
		ORANGE,
		PURPLE,
		LIGHT_BLUE,
		PINK,
		LIME,
		SILVER,
		GOLD,
		BROWN,

		Count
	};

	// Small POD struct that holds RGB components as floats in the range [0,1].
	// This is convenient for uploading color uniforms to shaders.
	struct ColorRGB
	{
		float r, g, b;
	};

	// A constexpr table that contains RGB values for every Color enum entry.
	// The table order must match the order of the Color enum above.
	// Because this is constexpr it can be used at compile time and has no
	// runtime initialization cost.
	constexpr ColorRGB COLOR_TABLE[] = {
		{1.0f, 0.0f, 0.0f},       // RED
		{0.0f, 1.0f, 0.0f},       // GREEN
		{0.0f, 0.0f, 1.0f},       // BLUE
		{1.0f, 1.0f, 1.0f},       // WHITE
		{0.0f, 0.0f, 0.0f},       // BLACK
		{1.0f, 1.0f, 0.0f},       // YELLOW
		{0.0f, 1.0f, 1.0f},       // CYAN
		{1.0f, 0.0f, 1.0f},       // MAGENTA
		{1.0f, 0.5f, 0.0f},       // ORANGE
		{0.6f, 0.0f, 1.0f},       // PURPLE
		{0.5f, 0.8f, 1.0f},       // LIGHT_BLUE
		{1.0f, 0.7f, 0.8f},       // PINK
		{0.7f, 1.0f, 0.2f},       // LIME
		{0.75f, 0.75f, 0.75f},    // SILVER
		{1.0f, 0.84f, 0.0f},      // GOLD
		{0.6f, 0.3f, 0.1f},       // BROWN
	};

	// Returns the ColorRGB corresponding to the provided Color enum value.
	// The lookup is a simple array index using the enum converted to size_t.
	// The function is constexpr, so it can be evaluated at compile time.
	constexpr ColorRGB GetColorRGB(Color color)
	{
		return COLOR_TABLE[static_cast<size_t>(color)];
	}
} // namespace LEN