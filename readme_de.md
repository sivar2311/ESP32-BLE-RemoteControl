[![Build](https://github.com/sivar2311/ESP32-BLE-RemoteControl/actions/workflows/build.yml/badge.svg)](https://github.com/sivar2311/ESP32-BLE-RemoteControl/actions/workflows/build.yml)

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
Ein Empfänger kann (theoretisch) bis zu 65536 verschiedene Fernbedienungen verwalten und 65536 verschiedene Befehle verarbeiten.

# Installation

*ToDo...*

# Benutzung

# Fernbedienung

Jede Fernbedienung wird mit der Struktur `ble_remote_control_info_t` verwaltet.
Diese enthält die ID-Nummer der Fernbedienung, den AES-Schlüssel und den Rolling-Code.
Sie stellt zwei Konstruktoren bereit um die Instanziierung einer Fernbedienung zu vereinfachen.

## Instanziierung einer Fernbedienung

Standard:

```C++

ble_remote_control_info_t remote (
/* id      */  0x0001,                                                                                       
/* aes-key */  {0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x00A, 0x00B, 0x0C, 0x0D, 0x0E, 0x0F}  
);
```

Mit vorbelegtem rolling-code:

```C++
ble_remote_control_info_t remote (
/* id           */  0x0001,                                                                                       
/* aes-key      */  {0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x00A, 0x00B, 0x0C, 0x0D, 0x0E, 0x0F}, 
/* rolling-code */  0x1234
);
```

*Notiz:*
Dies kann nützlich sein, wenn die Fernbedienung sehr stromsparend betrieben werden soll und zwischenzeitlich ausgeschaltet wird. 
Der Rolling-Code kann z. B.  vor dem Ausschalten im EEPROM, NVS oder in einer Datei gespeichert, und nach dem Einschalten daraus wieder geladen werden.

# Sender

Das Senden der Befehle wird von der Klasse `BLERemoteControlSender` vorgenommen.

## Instanziierung des Senders

```C++
BLERemoteControlSender sender;
```

## Senden von Befehlen

Zum Senden von Befehlen wird die Funktion `send_command` verwendet.
Der Funktion wird im ersten Parameter der zu sendende Befehls-Code vom typ `uint16_t` übergeben.
Im zweiten Parameter wird ein ***Zeiger*** auf die Fernbedienung vom Typ `ble_remote_control_info_t` übergeben.

Beispiel: Senden des Befehls-Code "0x1234" mit der zuvor angelegten Fernbedienung

```C++
  sender.send_command(0x1234, &remote);
```

Beispiel: [Sender.ino](/examples/Sender/Sender.ino)

# Empfänger

Der Empfang der Befehle wird von der Klasse `BLERemoteControlReceiver` vorgenommen.

## Instanziierung des Empfängers

```C++
BLERemoteControlReceiver receiver;
```

## Hinzufügen von Fernbedienungen

Dem Empfänger müssen zunächst alle (akzeptierten) Fernbedienungen hinzugefügt werden.
Dazu wird die Funktion `add_remote_control` verwendet. 
Die Funktion erhält als Parameter ein ***Zeiger*** auf die Fernbedienung vom Typ `ble_remote_control_info_t`.

```C++
receiver.add_remote_control(&remote);
```

Beispiel: [SingleReceiver.ino](/examples/Receiver/SingleReceiver/SingleReceiver.ino)

Ein Empfänger kann mehrere Fernbedienungen empfangen. 
Dazu müssen dem Empfänger alle zu empfangenden Fernbedienungen hinzugefügt werden.

```C++
receiver.add_remote_control(&remote_1);
receiver.add_remote_control(&remote_2);
receiver.add_remote_control(&remote_3);
receiver.add_remote_control(&remote_4);
```

Beispiel: [MultiReceiver.ino](/examples/Receiver/MultiReceiver/MultiReceiver.ino)


## Behandeln von empfangenen Befehlen

Zur Verarbeitung von Befehlen wird beim Empfang von Fernbedienungs-Befehlen vom Empfänger eine Rückruffunktion aufgerufen.
Die Rückruffunktion enthält als Parameter einen ***Zeiger*** vom Typ `ble_remote_command_info_t` welche den Befehl und einen ***Zeiger*** auf die Fernbedienung enthält.

```C++
void handle_command(ble_remote_command_info_t* command_info) {
    Serial.print("Received command: ");
    Serial.print(command_info->command);
    Serial.print(" from remote-control with id: ");
    Serial.println(command_info->remote_control.id, HEX);
}
```

## Zuweisen der Rückruf-Funktion für die Befehlsbehandlung

```C++
receiver.on_command(handle_command);
```

## Starten des Empfängers

```C++
receiver.begin();
```

## Entfernen von Fernbedienungen

Eine Fernbedienung kann entfernt werden (z. B. wenn diese verloren wurde).
Dazu wird die Funktion `remove_remote_control` verwendet.
Die Funktion erhält als Parameter ein ***Zeiger*** auf die Fernbedienung vom Typ `ble_remote_control_info_t`.

```C++
receiver.remove_remote_control(&remote_3);
```

## Synchronisieren einer Fernbedienung

Die Empfängerklasse erlaubt das Synchronisieren von Fernbedienungen. Dies ist Notwendig wenn der Rolling-Code der Fernbedienung kleiner als der des Empfängers ist. Das kann z.B. durch einen Neustart der Fernbedienung passieren, ohne dass der Rolling-Code (in der Fernbedienung des Senders) gespeichert und wieder geladen wurde.

Starten der Synchronisierung:
```C++
receiver.sync_remotecontrol();
```
Der nächste empfangene Befehl einer Fernbedienung wird genutzt um den Rolling-Code in der Fernbedienungsinformation des Empfängers zu aktualisieren. Der Befehl selbst wird dabei nicht behandelt. Anschließend wird der Synchronisationsmodus beendet und folgende Befehle werden normal behandelt.


### Synchronisations Rückruf-Funktion
Sobald eine Synchronisation stattgefunden hat wird die Synchronisations Rückruf-Funktion aufgerufen.
Der Rückruf-Funktion wird als Parameter ein ***Zeiger*** vom Typ `ble_remote_control_info_t` übergeben, welche die synchronisierte Fernbedienung spezifiziert.

```C++
void print_sync_info(ble_remote_control_info_t* remote_control) {
    Serial.print("Remote Control with id ");
    Serial.print(remote_control->id);
    Serial.println(" has been synchronized");
}
```

### Zuweisen der Synchronisations Rückruf-Funktion

```C++
receiver.on_sync(print_sync_info);
```

