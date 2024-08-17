module;

#include <cstdint>

export module Indy.Graphics:Texture;

export
{
	namespace Indy::Graphics
	{
		struct Texture
		{
			virtual uint32_t& GetWidth() = 0;
			virtual uint32_t& GetHeight() = 0;
		};
	}
}