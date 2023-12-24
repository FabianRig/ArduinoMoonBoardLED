tension board port notes:

APK
-BasicLedWall.java
    -> display()

- will need to send version and maybe serial number

- climbPlacementMinimalData contains needed hold information
    - based on v2/v3 there are different functions to display holds (should default to 2)
    - bluetoothServiceKt.prepBytedV2 converts to array of integers
    - returned array is sent to sentBytes function
    - sendBytes calls writeCharacteristics 
        characteristic uuid: 6E400002-B5A3-F393-E0A9-E50E24DCCA9E
        service uuid: 6E400001-B5A3-F393-E0A9-E50E24DCCA9E