module;

#include "Engine/Core/LogMacros.h"
#include <vector>
#include <memory>
#include <functional>
#include <typeindex>
#include <unordered_map>

export module Indy_Core_Events:EventManager;

import :EventHandle;
import :EventListener;

export namespace Indy
{
	class EventManagerCSR
	{
	private:
		EventManagerCSR();
		~EventManagerCSR() = default;

	private:
		static std::vector<std::pair<std::type_index, uint32_t>> s_TypeIndices; // These MUST stay ordered
		static std::vector<std::shared_ptr<IEventListener>> s_EventListeners;

	public:
		static bool RemoveEventListener(const IEventHandle& handle)
		{
			// Search for the start and relative end indices of this event type
			size_t startIndex = -1, endIndex = -1;
			for (size_t i = 0; i < s_TypeIndices.size(); i++)
			{
				if (handle.eventID == s_TypeIndices[i].first) // compare type_index
				{
					startIndex = (size_t)s_TypeIndices[i].second; // Always start at the beginning of this event type's section

					if (i + 1 == s_TypeIndices.size())
						endIndex = (size_t)s_TypeIndices.size(); // Don't extend beyond the vector's capacity
					else
						endIndex = (size_t)s_TypeIndices.at(i + 1).second; // Always end at the beginning of the NEXT event type's section
				}
			}

			// We need to know if the handle is valid. This ensures the type id of the handle
			//	correspondes to the correct section in s_EventListeners
			bool isValidIndex = handle.index >= startIndex && handle.index < endIndex;

			if (startIndex < 0 || !isValidIndex)
				return false;

			s_EventListeners.at(handle.index) = nullptr;
			return true;
		};

		template<class EventType>
		static IEventHandle AddEventListener(std::function<void(EventType*)> callback)
		{
			// Retrieve the EventType's type_index
			std::type_index id = std::type_index(typeid(EventType));

			// Generate the handle to be returned;
			IEventHandle handle{ id };

			// Initialize an invalid insertion index and look for type matches
			handle.index = -1;
			for (size_t i = 0; i < s_TypeIndices.size(); i++)
			{
				// If the insert index has been found, the start positions of every
				//	following type must be adjusted to reflect the addition of a new
				//	event listener
				if (handle.index > -1) {
					s_TypeIndices[i].second++;
					continue;
				}

				if (id == s_TypeIndices[i].first) // compare type_index
				{
					// Before we check for null values, set the handle's index to be the last element in this event type's section.
					if (i == s_TypeIndices.size() - 1)
						handle.index = s_TypeIndices.size(); // at either the end of the vector
					else
						handle.index = (size_t)s_TypeIndices.at(i + 1).second; // or the start of the next type's section

					// And check for null values in this event type's section of event listeners.
					for (int32_t j = s_TypeIndices[i].second; j < handle.index; j++)
					{
						if (s_EventListeners.at(j) == nullptr)
							handle.index = j;
					}
				}
			}

			// If we don't have any listeners of this type, our job is simple.
			if (handle.index < 0)
			{
				// Deduce the start index based on whether any event listeners exist
				if (s_TypeIndices.empty())
					handle.index = 0;
				else
					handle.index = s_EventListeners.size();

				// Safe to assume insertion at the end of each vector won't break anything. Indices will always match.
				// Additionally, s_TypeIndices will always retain logical order.
				s_TypeIndices.emplace_back(std::make_pair(id, handle.index));
				s_EventListeners.emplace_back(std::make_shared<EventListener<EventType>>(callback));
				return handle;
			}

			// At this point, while space hasn't been allocated for this event listener, the typeIndex vector and subsequent start
			// indices have been adjusted to make room. Insertion won't break anything here.
			if (s_EventListeners.empty())
				s_EventListeners.emplace_back(std::make_shared<EventListener<EventType>>(callback));
			else
				s_EventListeners.insert(s_EventListeners.begin() + handle.index, std::make_shared<EventListener<EventType>>(callback));

			return handle;
		};

		template<class C, typename EventType>
		static IEventHandle AddEventListener(C* instance, void(C::* callback)(EventType*))
		{
			// Retrieve the EventType's type_index
			std::type_index id = std::type_index(typeid(EventType));

			// Generate the handle to be returned;
			IEventHandle handle{ id };

			// Initialize an invalid insertion index and look for type matches
			handle.index = -1;
			for (size_t i = 0; i < s_TypeIndices.size(); i++)
			{
				// If the insert index has been found, the start positions of every
				//	following type must be adjusted to reflect the addition of a new
				//	event listener
				if (handle.index > -1) {
					s_TypeIndices[i].second++;
					continue;
				}

				if (id == s_TypeIndices[i].first) // compare type_index
				{
					// Before we check for null values, set the handle's index to be the last element in this event type's section.
					if (i == s_TypeIndices.size() - 1)
						handle.index = (int32_t)s_TypeIndices.size(); // at either the end of the vector
					else
						handle.index = (int32_t)s_TypeIndices.at(i + 1).second; // or the start of the next type's section

					// And check for null values in this event type's section of event listeners.
					for (int32_t j = s_TypeIndices[i].second; j < handle.index; j++)
					{
						if (s_EventListeners.at(j) == nullptr)
							handle.index = j;
					}
				}
			}

			// If we don't have any listeners of this type, our job is simple.
			if (handle.index < 0)
			{
				// Deduce the start index based on whether any event listeners exist
				if (s_TypeIndices.empty())
					handle.index = 0;
				else
					handle.index = (int32_t)s_EventListeners.size();

				// Safe to assume insertion at the end of each vector won't break anything. Indices will always match.
				// Additionally, s_TypeIndices will always retain logical order.
				s_TypeIndices.emplace_back(std::make_pair(id, handle.index));
				s_EventListeners.emplace_back(std::make_shared<MemberEventListener<C, EventType>>(instance, callback));
				return handle;
			}

			// At this point, while space hasn't been allocated for this event listener, the typeIndex vector and subsequent start
			// indices have been adjusted to make room. Insertion won't break anything here.
			s_EventListeners.insert(s_EventListeners.begin() + handle.index, std::make_shared<MemberEventListener<C, EventType>>(instance, callback));

			return handle;
		};;

		template<typename EventType>
		static void Notify(EventType* event)
		{
			// Retrieve the EventType's type_index
			std::type_index id = std::type_index(typeid(EventType));

			int compareIndex = -1;
			uint32_t startIndex = UINT32_MAX, endIndex;
			for (const auto& typeIndex : s_TypeIndices)
			{
				++compareIndex;

				// No need to compare types we don't care about.
				if (id != typeIndex.first)
					continue;

				// Set start index to the index of this block.
				// Assume it lasts until the end of the vector.
				startIndex = typeIndex.second;
				endIndex = (uint32_t)s_EventListeners.size();

				// If it does last until the end of the vector, we have what we need.
				if ((size_t)compareIndex + 1 == s_TypeIndices.size())
					break;
				else
				{
					// Otherwise, end at the start of the next block, if one exists
					endIndex = s_TypeIndices[(size_t)(compareIndex) + 1].second;
					break;
				}
			}

			// If a start index wasn't found, then no event listeners exist of this type.
			if (startIndex == UINT32_MAX)
				return;

			// Otherwise, we can call all relevant callbacks
			// Size of flattened listener vector is given by endIndex - startIndex
			for (size_t j = 0; j < (size_t)(endIndex - startIndex); j++)
			{
				std::shared_ptr<IEventListener> listener;
				if (event->bubbles)
					listener = s_EventListeners.at(endIndex - j - 1);
				else
					listener = s_EventListeners.at(startIndex + j);

				// There may be null listeners that have been previously removed
				if (listener == nullptr)
					continue;

				listener->Exec(event);
			}
		};
	};
}
