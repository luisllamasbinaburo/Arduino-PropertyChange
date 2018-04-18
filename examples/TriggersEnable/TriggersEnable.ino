/***************************************************
Copyright (c) 2017 Luis Llamas
(www.luisllamas.es)

Licensed under the Apache License, Version 2.0 (the "License"); you may not use this file except in compliance with the License. You may obtain a copy of the License at http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software distributed under the License is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the License for the specific language governing permissions and limitations under the License
 ****************************************************/

#include "PropertyChangeLib.h"

// Create property
PropertyChange<int> value(0);

// Auxiliar function for debug. Show current value, and asign new value
void PropertyDebug(PropertyChange<int>&prop, int newvalue)
{
	Serial.print("Now in: ");
	Serial.println(newvalue);
	prop = newvalue;
}

// Auxiliar function for debug. Make sequence a-b-a-b.. N-times
void makeSequence(size_t number, int a, int b)
{
	for (size_t index = 0; index < number; index++)
	{
		PropertyDebug(value, a);
		PropertyDebug(value, b);
	}
}

void setup()
{
	Serial.begin(9600);

	value.IgnoreFirst = true;

	// Add triggers. All triggers are Type Once, and enable all the other triggers when firing
	value.AddTrigger(
		[](PropertyChange<int> a) { return a.CurrentValue == 1 && a.LastValue == 0; },
		[](PropertyChange<int> a) { Serial.println("Detected secuence 0-1"); a.EnableAllExcept(0); },
		PropertyChange<int>::TriggerType::Once
	);

	value.AddTrigger(
		[](PropertyChange<int> a) { return a.CurrentValue == 0 && a.LastValue == 0; },
		[](PropertyChange<int> a) { Serial.println("Detected secuence 0-0"); a.EnableAllExcept(1); },
		PropertyChange<int>::TriggerType::Once
	);

	value.AddTrigger(
		[](PropertyChange<int> a) { return a.CurrentValue == 1 && a.LastValue == 1; },
		[](PropertyChange<int> a) { Serial.println("Detected secuence 1-1"); a.EnableAllExcept(2); },
		PropertyChange<int>::TriggerType::Once
	);

	// Test several sequences
	Serial.println(); Serial.println("**** Testing 0-1 sequence ****");
	makeSequence(3, 0, 1);

	Serial.println(); Serial.println("**** Testing 0-0 sequence ****");
	makeSequence(3, 0, 0);

	Serial.println("**** Testing 0-1 sequence ****");
	makeSequence(3, 0, 1);

	Serial.println(); Serial.println("**** Testing 1-1 sequence ****");
	makeSequence(3, 1, 1);

	Serial.println(); Serial.println("**** Testing 0-1 sequence ****");
	makeSequence(3, 0, 1);

}

void loop()
{
	delay(1000);
}

