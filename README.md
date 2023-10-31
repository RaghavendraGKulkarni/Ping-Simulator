<h1 align="center" id="title">PINGER SIMULATOR USING SOCKETS</h1>

<p id="description">Simulate a Client – Server Ping Protocol System which allows the client to send ping messages to the server and the server to respond back the ping message converted to uppercase.</p>

<h2>🧐 Features</h2>

Here're some of the project's best features:

*   The server is always be in an ON state.
*   The client sends ping messages in the format: ping "ping\_Number" "time\_stamp"
*   When the server receives a ping message, it generates a random number in the range [1, 12]. If the number is less than 4, then the server drops the ping message. Else, it responds back to the client by capitalizing the letters in the ping message. Hence, the system injects an artifical average packet loss of 33%.
*   The client waits for 1 second to receive response from the server. If response is received within 1 second the client prints the received message and the Round Trip Time. Else, it prints “Request Timeout”.
*   The client finally computes and prints the maximum, minimum and average RTTs along with the percentage of packets lost.