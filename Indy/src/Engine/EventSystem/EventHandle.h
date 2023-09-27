#pragma once

struct EventContainerBase;

struct EventHandle
{
	uint32_t event_id;
	EventContainerBase* container;
};

struct EventContainerBase 
{
	virtual bool removeCallback(const EventHandle& handle) { return false; };
};