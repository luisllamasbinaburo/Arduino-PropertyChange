/***************************************************
Copyright (c) 2017 Luis Llamas
(www.luisllamas.es)

Licensed under the Apache License, Version 2.0 (the "License"); you may not use this file except in compliance with the License. You may obtain a copy of the License at http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software distributed under the License is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the License for the specific language governing permissions and limitations under the License
 ****************************************************/
 
#include "PropertyChangeLib.h"

// Auxiliar function for debug. Show current value, and asign new value
void PropertyDebug(PropertyChange<int>&prop, int newvalue)
{
	Serial.println();
	Serial.println("************************************** ");
	Serial.print("Now in: ");
	Serial.println(newvalue);
	prop = newvalue;
}

// Create property
PropertyChange<int> value(0, [](PropertyChange<int> a) {Serial.print("Changed from "); Serial.print(a.LastValue); Serial.print(" to "); Serial.println(a.CurrentValue); });

void setup()
{
	Serial.begin(9600);

	// Add triggers
	// >= 10, Once trigger
	value.AddTrigger(
		[](PropertyChange<int> a) { return a.CurrentValue >= 10; },
		[](PropertyChange<int> a) { Serial.println(">=10 (Once)"); }, PropertyChange<int>::TriggerType::Once
	);

	// >= 20, OnChange trigger
	value.AddTrigger(
		[](PropertyChange<int> a) { return a.CurrentValue >= 20; },
		[](PropertyChange<int> a) { Serial.println(">=20 (OnChange)");}, PropertyChange<int>::TriggerType::OnChange
	);

	// >= 30, Anytime trigger
	value.AddTrigger(
		[](PropertyChange<int> a) { return a.CurrentValue >= 30; },
		[](PropertyChange<int> a) { Serial.println(">=30 (AnyTime)"); }, PropertyChange<int>::TriggerType::AnyTime
	);

	PropertyDebug(value, 10); //Print Change, >= 10 (Once)
	PropertyDebug(value, 10);
	PropertyDebug(value, 10);
	PropertyDebug(value, 20); //Print Change, >= 20 (OnChange)
	PropertyDebug(value, 20);
	PropertyDebug(value, 20);
	PropertyDebug(value, 30); //Print Change, >= 20 (OnChange), >= 30 (AnyTime)
	PropertyDebug(value, 30); //Print >= 30 (AnyTime)
	PropertyDebug(value, 30); //Print >= 30 (AnyTime)
	PropertyDebug(value, 10); //Print Change
	PropertyDebug(value, 40); //Print Change, >= 20 (OnChange), >= 30 (AnyTime)
	PropertyDebug(value, 40); //Print Change, >= 30 (AnyTime)
}


void loop()
{
}
