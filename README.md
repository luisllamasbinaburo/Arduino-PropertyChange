# Librería Arduino PropertyChange
La librería PropertyChange implementa una variable que conoce su estado anterior, y ejecuta una acción de callback cuando el valor actual cambia. Adicionalmente se pueden definir triggers, condiciones que evalúan al asignar un valor y que disparan sus propias acciones.

Más información https://www.luisllamas.es/libreria-arduino-properychange/

## Instrucciones de uso

La librería ProperyChange<T> es un wrapper entorno a una otra variable de tipo T. Almacena el último valor y el valor actual de la misma. Para actualizar el valor de la variable podemos usar la función `Update(T newValue)`, o simplemente emplear el operador `=` como en una variable convencional.
Adicionalmente, podemos definir una función de Callback, que dispara cuando se detecta un cambio en la variable. Las funciones de callback reciben la propia instancia, por lo que tienen acceso a todos los campos de la misma, incluidos el valor o anterior.

La librería PropertyChange también permite añadir triggers, formados por una condición y una función de callback similar la anterior. Al actualizar el valor de la variable, se evalúan los triggers (en función de su tipo) y, en caso de cumplirse, disparan la función asignada.
El comportamiento del trigger está condicionado por su tipo, existiendo:
- AnyTime, disparan siempre que se cumple la condición.
- OnChange, disparan en todo cambio de valor, en el que se cumpla la condición.
- Once, disparan una única vez y quedan desactivados hasta que son rearmados mediante las funciones `Enable..()`.

Los trigger pueden ser desactivados o activados mediante los grupos de funciones `Enable..()` y `Disable..()`. Combinado a que cada Callback dispone de la instancia del objeto que lo invoca,da lugar a muchas posibles combinaciones potentes (ver ejemplo `TriggersEnable`).

Por último al comportamiento ante la primera asignación, podemos emplear el campo IgnoreFirst para que la primera asignación sea considerada como una inicialización, por lo cuál es ignorada. 

### Constructor
La clase ProperyChange se instancia a través de su constructor.
```c++
	PropertyChange(PropertyChangeCallback onPropertyChange = nullptr);
	PropertyChange(T initialValue, PropertyChangeCallback onPropertyChange = nullptr);
```

## Definiciones
```c++
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
```

### Uso de ProperyChange
```c++
	// Actualizar la propiedad
	void operator=(T newValue);
	bool Update(T newValue);

	// Añadir/eliminar un trigger
	uint8_t AddTrigger(PropertyChangeCondition condition, PropertyChangeCallback callback, TriggerType type = TriggerType::AnyTime);
	uint8_t RemoveTrigger(uint8_t index);
	
	// Desactivar triggers
	void DisableTrigger(uint8_t index);
	void DisableAllExcept(uint8_t index);
	void DisableAllTriggers();

	// Activar triggers
	void EnableTrigger(uint8_t index);
	void EnableAllExcept(uint8_t index);
	void EnableAllTriggers();

	// Controlar respuesta a la primera asignacion de valor
	bool IgnoreFirst = false;
	bool IsFirstValue = true;
	
	// Evalua a true si el ultimo update era un cambio de valor
	bool HasChanged = false;

	// Ultimo valor registrado y valor actual
	T LastValue;
	T CurrentValue;
```

## Ejemplos
La librería ProperyChange incluye los siguientes ejemplos para ilustrar su uso.

* Basic: Ejemplo que muestra el uso básico de PropertyChange.
```c++
#include "PropertyChangeLib.h"

void Debug(PropertyChange<int> a)
{
	Serial.print("Changed to: ");
	Serial.println(a.CurrentValue);
}

PropertyChange<int> value(Debug);

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
```

* IgnoreFirst: Ejemplo que muestra el uso de IgnoreFirst.
```c++
#include "PropertyChangeLib.h"

PropertyChange<int> value([](PropertyChange<int> a) {Serial.print("Changed to: "); Serial.println(a.CurrentValue); });

void setup()
{
	while (!Serial);

	Serial.begin(9600);

	value.IgnoreFirst = true;
	value = 10;	// Ignored (first value)
	value = 20;	// Print Changed to: 20
	value = 20;
	value = 20;
	value = 30;	// Print Changed to: 30
}

void loop()
{
}
```

* Triggers: Ejemplo que muestra el uso de Triggers y la diferencia entre el tipo Once, OnChange y AnyTime.
```c++
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
```


* TriggersEnable: Ejemplo que muestra el uso de activar/desactivar triggers. En este ejemplo, usamos una PropertyChange para distinguir entre una secuencia de 00, 01 y 11.
```c++
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
```
