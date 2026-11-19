# Multi-user-Chat-server
Multi-user Chat using TCP server and client 

Every line a user sends is delivered to all connected users, 
including the sender, as 
Client N: message. 

The server uses one thread per client, 
the client uses a second thread to receive while the main thread sends.
