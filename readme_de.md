[![Build](https://github.com/sivar2311/ESP32-BLE-RemoteControl/actions/workflows/build.yml/badge.svg)](https://github.com/sivar2311/ESP32-BLE-RemoteControl/actions/workflows/build.yml)
[![Release](https://img.shields.io/github/v/release/sivar2311/ESP32-BLE-RemoteControl)](https://github.com/sivar2311/ESP32-BLE-RemoteControl/releases)
# ESP32-BLE-Remote-Control

Diese Bibliothek erlaubt es Ihnen einfache Kommando-Nachrichten über Bluetooth Low Energy zu senden und zu empfangen.

# Besonderheiten:

## Verbindungslos

Die Nachrichten werden als Bluetooth Werbungspakete verschickt - es ist keine aktive Verbindung oder Paarung notwendig.

## Sicherheit
- 128bit AES-Verschlüsselung: Alle Nachrichten werden mittels 128bit AES-Verschlüsselung verschlüsselt übertragen.
- NONCE: Jede Nachricht enthält ein zufälligen NONCE-Wert, der die Entropie der verschlüsselten Nachricht erhöht.
- Rolling-Code: Jede Nachricht ist mit einem Rolling-Code versehen um Wiederholungsangriffe zu unterbinden.

## Ein Empfänger - mehrere Sender
Ein Empfänger kann (theoretisch) bis zu 65.536 verschiedene Fernbedienungen verwalten und 4.294.967.296 verschiedene oder 32 gleichzeitige Befehle verarbeiten (die Interpretation ist der Anwendung / dem Benutzer überlassen).

# Installation

Zur Zeit ist diese Bibliothek weder bei Arduino noch PlatformIO registriert und muss daher manuell installiert / eingebunden werden. 

### Arduino IDE

Laden Sie das Repository als ZIP-Datei herunter.
Wählen Sie dann in der ArduinoIDE im Menü `Sketch` / `Bibliothek einbinden` / `.ZIP-Bibliothek hinzufügen...` und wählen Sie die heruntergeladene ZIP-Date aus.

### PlatformIO

Fügen Sie ihrer `platformio.ini` folgendes hinzu:
```ini
lib_deps = 
  https://github.com/sivar2311/ESP32-BLE-RemoteControl.git
``` 

## Abhängigkeiten

Diese Bibliothek ist Abhängig von [NimBLE-Arduino](https://github.com/h2zero/NimBLE-Arduino).

# Benutzung

Dieses Kapitel muss neu erstellt werden, da sich grundlegende Änderungen ergeben haben.
Bitte schauen Sie sich bis dahin die [Beispiele](https://github.com/sivar2311/ESP32-BLE-RemoteControl/tree/master/examples) an.