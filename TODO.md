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
        service uuid: 6E400001-B5A3-F393-E0A9-E50E24DCCA9E - service when discovered
        service uuid: 4488B571-7806-4DF6-BCFF-A2897E4953FF - when scanning


    - array being sent via uart characteristic looks like <1, (size), (checksum),2, (hold numbers),3>
    - chunked into 20s
    ``` 
    ArrayList arrayList = new ArrayList();
        arrayList.add(1);
        arrayList.add(Integer.valueOf(list.size()));
        arrayList.add(Integer.valueOf(checksum(list)));
        arrayList.add(2);
        for (Integer num : list) {
            arrayList.add(Integer.valueOf(num.intValue()));
        }
        arrayList.add(3);
        return arrayList;
        
     ```

     - which value do I need to read? 