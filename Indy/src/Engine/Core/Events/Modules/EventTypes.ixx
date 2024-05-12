export module Indy.Events:Types;

export
{
	namespace Indy
	{
		struct IEvent
		{
			bool bubbles = true;
			bool propagates = true;
		};
	}
}