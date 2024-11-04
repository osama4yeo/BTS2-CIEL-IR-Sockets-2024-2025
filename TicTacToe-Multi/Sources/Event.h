#pragma once

#include <iostream>
#include <functional>
#include <vector>
#include <algorithm>

template<typename... Func>
class Event
{
public:
	using Delegate = std::function<Func...>;

	void AddListener(const Delegate& listener)
	{
		_listeners.push_back(listener);
	}

	void RemoveListener(const Delegate& listener)
	{
		typename std::vector<Delegate>::iterator it = std::find_if(
			_listeners.begin(),
			_listeners.end(),
			[&](const auto& l)
		{
			return l.target_type() == listener.target_type();
		});

		if(it != _listeners.end())
		{
			_listeners.erase(it);
		}
	}

	template<typename... Args>
	void Invoke(Args... args) const
	{
		for(const Delegate& listener : _listeners)
		{
			listener(std::forward<Args>(args)...);
		}
	}

private:

	std::vector<Delegate> _listeners;
};