/***************************************************
Copyright (c) 2017 Luis Llamas
(www.luisllamas.es)

Licensed under the Apache License, Version 2.0 (the "License"); you may not use this file except in compliance with the License. You may obtain a copy of the License at http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software distributed under the License is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the License for the specific language governing permissions and limitations under the License
 ****************************************************/


#include "PropertyChangeLib.h"

PropertyChange<int> value(Debug);

void Debug(PropertyChange<int> a)
{
	Serial.print("Changed to: ");
	Serial.println(a.CurrentValue);
}

void setup()
{
	Serial.begin(9600);

	value = 10;	// Print Changed to: 10
	value = 20;	// Print Changed to: 20
	value = 20;
	value = 20;
	value = 30;	// Print Changed to: 30
}


void loop()
{
}
