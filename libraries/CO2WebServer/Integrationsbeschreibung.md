# Ardublocks Interface für WebPlotter

#### Inputs die in Ardublocks gesetzt werden können

``webinterface title`` *string* => WebInterfaceTitle        | Titel der Website

---

1. Graph
``name, unit`` *string* => name1, unit1						| Name und Einheit des Messwerts
``good`` *int* => good1										| Wert bis zu dem der Messwert als gut gillt
``bad`` *int* => bad1										| Wert ab dem der Messwert als schlecht gilt
``min`` *int* => min1										| Min auf dem Graph
``max`` *int* => max1										| Max auf dem Graph
``stepsize`` *int* => stepsize1								| Stepsize auf dem Graph
``cycleDuration`` *int* => cycleDuration1					| Intervall in dem neue Daten erhoben werden sollen
``cycleStepsize`` *int* => cycleStepsize1					| Stepsize des Intervalls in Sekunden auf dem Graph
``sensorInput`` *float* => sensorInput (``sensorReading1``)	| Der Sensorwert, der gemessen werden soll

---

2. Graph
``name, unit`` *string* => name2, unit2						| Name und Einheit des Messwerts
``good`` *int* => good2										| Wert bis zu dem der Messwert als gut gillt
``bad`` *int* => bad2										| Wert ab dem der Messwert als schlecht gilt
``min`` *int* => min2										| Min auf dem Graph
``max`` *int* => max2										| Max auf dem Graph
``stepsize`` *int* => stepsize2								| Stepsize auf dem Graph
``cycleDuration`` *int* => cycleDuration2					| Intervall in dem neue Daten erhoben werden sollen
``cycleStepsize`` *int* => cycleStepsize2					| Stepsize des Intervalls in Sekunden auf dem Graph
``sensorInput`` *float* => sensorInput (``sensorReading2``)	| Der Sensorwert, der gemessen werden soll

---

3. Graph
``name, unit`` *string* => name3, unit3						| Name und Einheit des Messwerts
``good`` *int* => good3										| Wert bis zu dem der Messwert als gut gillt
``bad`` *int* => bad3										| Wert ab dem der Messwert als schlecht gilt
``min`` *int* => min3										| Min auf dem Graph
``max`` *int* => max3										| Max auf dem Graph
``stepsize`` *int* => stepsize3								| Stepsize auf dem Graph
``cycleDuration`` *int* => cycleDuration3					| Intervall in dem neue Daten erhoben werden sollen
``cycleStepsize`` *int* => cycleStepsize3					| Stepsize des Intervalls in Sekunden auf dem Graph
``sensorInput`` *float* => sensorInput (``sensorReading3``)	| Der Sensorwert, der gemessen werden soll

---

Kalibrierung
``calibration password`` *string* => calibrationPassword    | Passwort, um die Kalibrierung durchzuführen
``calibration function`` *void* => calibration              | Die Methode der Kalibrierung

---


Alle Felder, die per Ardublocks gesetzt werden müssen, sind mit ``// <- set by Ardublocks`` gekennzeichnet.
Integriert werden sollen die Bereiche, die von diesen Kommentaren umschlossen sind:
- ``SensorPlot WebInterface Reference`` 
- ``SensorPlot WebInterface Module 1/3`` 
- ``SensorPlot WebInterface Module 2/3`` 
- ``SensorPlot WebInterface Module 3/3``