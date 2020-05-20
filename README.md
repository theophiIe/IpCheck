# IpCheck

## Description
This application make parallel ping requests for ipv4 address.   
   
This application use a file configuration filled with ipv4 address.   
Example of valide config file :    
```
008.008.008.008
001.001.001.001
139.130.004.005
```

Example of a request ping :
```
ping -c 4 001.001.001.001 > 001.001.001.001.txt
```
The result of the ping is written to a text file.

## Compilation
```
gcc -pthread -O2 -o main *.o
```

## Execution
```
./main
```
