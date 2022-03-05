# BLE advertising data

## BLE advertising data structure

| Name:     | BLE advertisement flags | length of the<br>next field | type of the<br>next field     | Manufacturer<br>data |
|----------:|:-----------------------:|:---------------------------:|:-----------------------------:|:--------------------:|
| *Length*: | 3 Bytes                 | 1 Byte                      | 1 Byte                        | 20 Bytes             |
| *Value*: | `0x02` `0x01` `0x06`    | `0x15`                      | `0xFF`<br>(manufacturer data) | [Data](#data)        |
|           | **fixed**               | **fixed**<br>               | **fixed**                     | *variable*           |

**Note:**
*The receiver does not care about the BLE advertisement flags. Other values may also work.*

## Data

| Name:     | company ID                  | remote control ID            | message             |
|----------:|:---------------------------:|:----------------------------:|:-------------------:|
| *Length*: | 2 Bytes                     | 2 Bytes                      | 16 Bytes            |
| *Value*:  | `0xFFFF`<br>(no company ID) | from `0x0000`<br>to `0xFFFF` | [Message](#message) |
|           | **fixed**                   | *variable*                   | *variable*          |
 
## Message
**128-bit AES encrypted**
| Name:     | remote control ID                            | command                              | rolling code                | nonce           | reserved |
|----------:|:--------------------------------------------:|:------------------------------------:|:---------------------------:|:---------------:|:--------:|
| *Length*: | 2 Bytes                                      | 4 Bytes                              | 4 Bytes                     | 2 Bytes         | 4 Bytes  |
| *Value*:  | from `0x0000`<br>to `0xFFFF`                 | from `0x00000000`<br>to `0xFFFFFFFF` | incremented<br>each message | random<br>value | -        |
|           | *variable*<br>(**fixed** per remote control) | *variable*                           | *variabe*                   | *variable*      | -        |

### Notes

***Remote control ID***: The ID is transmitted redundantly in both the unencrypted and encrypted parts of the BLE advertisement.*
*The unencrypted ID is used to identify the remote control and find the corresponding AES key.*
*After decrypting the message, the IDs are compared. If they match, the message is valid.*

***Command***: *The command field can be used for either 4294967295 different commands or up to 32 simultaneous commands. The interpretation is left to the application / user.*

