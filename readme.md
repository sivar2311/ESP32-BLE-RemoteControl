[![Build](https://github.com/sivar2311/ESP32-BLE-RemoteControl/actions/workflows/build.yml/badge.svg)](https://github.com/sivar2311/ESP32-BLE-RemoteControl/actions/workflows/build.yml)

# ESP32-BLE-Remote-Control

This library allows you to send and receive simple command messages via Bluetooth Low Energy.

# Features:

## Connectionless

Messages are sent as Bluetooth advertisement packets - no active connection or pairing is required.

## Security
- 128bit AES encryption: All messages are transmitted encrypted using 128bit AES encryption.
- NONCE: Each message contains a random NONCE value that increases the entropy of the encrypted message.
- Rolling code: Each message is provided with a rolling code to prevent replay attacks.

## One receiver - multiple senders
A receiver can (theoretically) manage up to 65536 different remote controls and process 65536 different commands.

# Installation

Currently this library is neither registered with Arduino nor PlatformIO and therefore has to be installed / included manually. 

### Arduino IDE

Download the repository as a ZIP file.
Then select `Sketch` / `Include Library` / `Add .ZIP library...` in the ArduinoIDE menu and select the downloaded ZIP file.

### PlatformIO

Add the following to your `platformio.ini`:
```ini
lib_deps = 
  https://github.com/sivar2311/ESP32-BLE-RemoteControl.git
``` 

## Dependencies

This library depends on [NimBLE-Arduino](https://github.com/h2zero/NimBLE-Arduino).

# Use

## Remote control information

Each remote control is managed with the structure `ble_remote_control_info_t`.
This contains the ID number of the remote control, the AES key and the rolling code.
It provides two constructors to simplify the instantiation of a remote control.

### Instancing a remote control information

Default:

```C++

ble_remote_control_info_t remote (
/* id      */ 0x0001,                                                                                       
/* aes-key */ {0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x00A, 0x00B, 0x0C, 0x0D, 0x0E, 0x0F}  
);
```

With pre-assigned rolling-code:

```C++
ble_remote_control_info_t remote (
/* id           */ 0x0001,                                                                                       
/* aes-key      */ {0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x00A, 0x00B, 0x0C, 0x0D, 0x0E, 0x0F}, 
/* rolling-code */ 0x1234
);
```

*Note:*
This can be useful if the remote control is to be operated in a very power-saving way and is switched off in the meantime. 
The rolling code can, for example, be stored in the EEPROM, NVS or in a file before switching off and reloaded from it after switching on.

## Sender

The sending of commands is done by the class 'BLERemoteControlSender'.

### Instancing the sender

```C++
BLERemoteControlSender sender;
```

### Sending commands

The function `send_command` is used to send commands.
The function is passed the command code to be sent of type `uint16_t` in the first parameter.
In the second parameter, a ***pointer*** to the remote control of type `ble_remote_control_info_t` is passed.

Example: Sending the command code "0x1234" with the previously created remote control

```C++
sender.send_command(0x1234, &remote);
```

Example: [sender.ino](/examples/transmitter/transmitter.ino)

## Receiver

Receiving the commands is done by the class 'BLERemoteControlReceiver'.

### Instancing the receiver

```C++
BLERemoteControlReceiver receiver;
```

### Adding remote controls

All (accepted) remote controls must first be added to the receiver.
The function `add_remote_control` is used for this purpose. 
The function receives as parameter a ***pointer*** to the remote control of type `ble_remote_control_info_t`.

```C++
receiver.add_remote_control(&remote);
```

Example: [SingleReceiver.ino](/examples/Receiver/SingleReceiver/SingleReceiver.ino).

A receiver can receive several remote controls. 
To do this, all remote controls to be received must be added to the receiver.

```C++
receiver.add_remote_control(&remote_1);
receiver.add_remote_control(&remote_2);
receiver.add_remote_control(&remote_3);
receiver.add_remote_control(&remote_4);
```

Example: [MultiReceiver.ino](/examples/Receiver/MultiReceiver/MultiReceiver.ino)


### Handling of received commands

To process commands, a callback function is called when remote control commands are received from the receiver.
The callback function contains as a parameter a ***pointer*** of type `ble_remote_command_info_t` which contains the command and a ***pointer*** to the remote control.

```C++
void handle_command(ble_remote_command_info_t* command_info) {
    Serial.print("Received command: ");
    Serial.print(command_info->command);
    Serial.print(" from remote-control with id: ");
    Serial.println(command_info->remote_control.id, HEX);
}
```

### Assign the callback function for command handling

```C++
receiver.on_command(handle_command);
```

### Start the receiver

```C++
receiver.begin();
```

### Removing remote controls

A remote control can be removed (e.g. if it has been lost).
The function `remove_remote_control` is used for this purpose.
The function receives as parameter a ***pointer*** to the remote control of type `ble_remote_control_info_t`.

```C++
receiver.remove_remote_control(&remote_3);
```

### Synchronising a remote control

The receiver class allows the synchronisation of remote controls. This is necessary if the rolling code of the remote control is smaller than that of the receiver. This can happen, for example, by restarting the remote control without saving and reloading the rolling code (in the remote control of the sender).

Starting the synchronisation:
```C++
receiver.sync_remotecontrol();
```
The next received command from a remote control is used to update the rolling code in the receiver's remote control information. The command itself is not handled. Subsequently, the synchronisation mode is terminated and the following commands are handled normally.


### Synchronisation callback function
Once synchronisation has taken place, the synchronisation callback function is called.
The callback function is passed a ***pointer*** of type `ble_remote_control_info_t` as a parameter, which specifies the synchronised remote control.

```C++
void print_sync_info(ble_remote_control_info_t* remote_control) {
    Serial.print("Remote Control with id ");
    Serial.print(remote_control->id);
    Serial.println(" has been synchronized");
}
```

### Assigning the synchronisation callback function

```C++
receiver.on_sync(print_sync_info);
```