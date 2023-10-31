<h1>🛠️ Execution Steps:</h1>

<h2>Step 1:</h2>

<p>Open two separate terminals for Server and Client</p>



<h2>Step 2:</h2>

<p>Enter the directory in both Server and Client Terminals</p>



<h2>Step 3:</h2>

<p>Compile the Code for Normal TCP Server in the Server Terminal</p>

```shell
gcc TCPPingerServer.c -o TCPPingerServer
```

<center>OR</center>

<p>Compile the Code for Concurrent TCP Server in Server Terminal</p>

```shell
gcc -pthread TCPPingerConcurrentServer.c -o TCPPingerConcurrentServer
```



<h2>Step 4:</h2>

<p>Compile the Code for Client in Client Terminal</p>

```shell
gcc TCPPingerClient.c -o TCPPingerClient
```



<h2>Step 5:</h2>

<p>Run the Code for Normal Server in the Server Terminal</p>

```shell
./TCPPingerServer
```

<center>OR</center>

<p>Run the Code for Concurrent Server in Server Terminal</p>

```shell
./TCPPingerConcurrentServer
```



<h2>Step 6:</h2>

<p>Run the Code for Client in the Client Terminal</p>

```shell
./TCPPingerClient -c 10
```



<h2>Step 7:</h2>

<p>Stop the Server in the Server Terminal after the Client terminates</p>

```shell
^C
```



<h2>🍰 Custom Running Guidelines:</h2>

To run the code for custom number of ping messages replace 10 with any positive integer of your choice in Step 6.