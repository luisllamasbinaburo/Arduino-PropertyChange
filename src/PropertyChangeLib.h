/***************************************************
Copyright (c) 2017 Luis Llamas
(www.luisllamas.es)

Licensed under the Apache License, Version 2.0 (the "License"); you may not use this file except in compliance with the License. You may obtain a copy of the License at http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software distributed under the License is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the License for the specific language governing permissions and limitations under the License
****************************************************/

#ifndef _PROPERTYCHANGELIB_h
#define _PROPERTYCHANGELIB_h

#if defined(ARDUINO) && ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif


template <typename T>
class PropertyChange
{

public:
	typedef bool(*PropertyChangeCondition)(PropertyChange<T> value);
	typedef void(*PropertyChangeCallback)(PropertyChange<T> value);

	enum TriggerType
	{
		Once,
		OnChange,
		AnyTime
	};

	struct Trigger
	{
		PropertyChangeCondition Condition;
		PropertyChangeCallback Callback;
		TriggerType Type;
		bool IsEnabled = true;
	};

	PropertyChange(PropertyChangeCallback onPropertyChange = nullptr);
	PropertyChange(T initialValue, PropertyChangeCallback onPropertyChange = nullptr);
	PropertyChangeCallback OnPropertyChange;

	void operator=(T newValue);
	bool Update(T newValue);

	uint8_t AddTrigger(PropertyChangeCondition condition, PropertyChangeCallback callback, TriggerType type = TriggerType::AnyTime);
	uint8_t RemoveTrigger(uint8_t index);
	
	void DisableTrigger(uint8_t index);
	void DisableAllExcept(uint8_t index);
	void DisableAllTriggers();

	void EnableTrigger(uint8_t index);
	void EnableAllExcept(uint8_t index);
	void EnableAllTriggers();

	bool IgnoreFirst = false;
	bool IsFirstValue = true;
	bool HasChanged = false;

	T LastValue;
	T CurrentValue;

private:
	uint8_t _triggersCount = 0;
	Trigger * _triggers;
	void launchTriggers();
};


template <typename T>
PropertyChange<T>::PropertyChange(PropertyChangeCallback onPropertyChange = nullptr) : PropertyChange(new T, onPropertyChange)
{
}

template <typename T>
PropertyChange<T>::PropertyChange(T initialValue = new(T), PropertyChangeCallback onPropertyChange = nullptr)
{
	CurrentValue = initialValue;
	_triggersCount = 0;
	OnPropertyChange = onPropertyChange;
}

template<typename T>
void PropertyChange<T>::operator=(T newValue)
{
	Update(newValue);
}

template <typename T>
bool PropertyChange<T>::Update(T newValue)
{
	HasChanged = false;
	LastValue = CurrentValue;
	CurrentValue = newValue;

	if (IsFirstValue && IgnoreFirst)
	{
		IsFirstValue = false;
		return;
	}

	if (LastValue != CurrentValue)
	{
		HasChanged = true;
		if (OnPropertyChange != nullptr) OnPropertyChange(*this);
	}

	launchTriggers();

	return HasChanged;
}

template<typename T>
uint8_t PropertyChange<T>::AddTrigger(PropertyChangeCondition condition, PropertyChangeCallback callback, TriggerType type = TriggerType::OnChange)
{
	_triggersCount++;

	Trigger *newTriggers = new Trigger[_triggersCount];

	memmove(newTriggers, _triggers, (_triggersCount - 1) * sizeof(Trigger));
	delete[] _triggers;
	_triggers = newTriggers;

	_triggers[_triggersCount - 1].Condition = condition;
	_triggers[_triggersCount - 1].Callback = callback;
	_triggers[_triggersCount - 1].Type = type;

	return _triggersCount - 1;
}

template <typename T>
uint8_t PropertyChange<T>::RemoveTrigger(uint8_t index)
{
	_triggersCount--;

	Trigger *newTriggers = new Trigger[_triggersCount];

	memmove(newTriggers, _triggers, index * sizeof(Trigger));
	memmove(newTriggers + index, _triggers + index + 1, (_triggersCount - index) * sizeof(Trigger));

	delete[] _triggers;
	_triggers = newTriggers;

	return _triggersCount;
}

template <typename T>
void PropertyChange<T>::DisableTrigger(uint8_t index)
{
	if (index >= _triggersCount) return;
	_triggers[index].IsEnabled = false;
}

template<typename T>
void PropertyChange<T>::DisableAllTriggers()
{
	for (uint8_t index = 0; index < _triggersCount; index++)
	{
		_triggers[index].IsEnabled = false;
	}
}

template<typename T>
void PropertyChange<T>::DisableAllExcept(uint8_t index)
{
	if (index >= _triggersCount) return;
	bool prevState = _triggers[index].IsEnabled;
	DisableAllExcept();
	_triggers[index].IsEnabled = prevState;
}

template <typename T>
void PropertyChange<T>::EnableTrigger(uint8_t index)
{
	if (index >= _triggersCount) return;
	_triggers[index].IsEnabled = true;
}

template <typename T>
void PropertyChange<T>::EnableAllTriggers()
{
	for (uint8_t index = 0; index < _triggersCount; index++)
	{
		_triggers[index].IsEnabled = true;
	}
}

template <typename T>
void PropertyChange<T>::EnableAllExcept(uint8_t index)
{
	if (index >= _triggersCount) return;
	bool prevState = _triggers[index].IsEnabled;
	EnableAllTriggers();
	_triggers[index].IsEnabled = prevState;
}

template<typename T>
void PropertyChange<T>::launchTriggers()
{
	for (uint8_t index = 0; index < _triggersCount; index++)
	{
		if (_triggers[index].IsEnabled &&
			(_triggers[index].Type == TriggerType::Once ||
				_triggers[index].Type == TriggerType::AnyTime ||
				(_triggers[index].Type == TriggerType::OnChange && HasChanged)))
		{
			if (_triggers[index].Condition(*this))
			{
				_triggers[index].Callback(*this);
				if (_triggers[index].Type == TriggerType::Once) _triggers[index].IsEnabled = false;
			}
		}
	}
}

#endif

