

compile:
	g++ -o rudp -std=c++11 -pthread main.cpp Listener.cpp RUDP.cpp sender.cpp UDPSock.cpp 

debug:
	g++ -o rudp -std=c++11 -DDEBUG -pthread main.cpp Listener.cpp RUDP.cpp sender.cpp UDPSock.cpp 

#debug_listener:
#	g++ -o rudp -std=c++11 -DDEBUG -DLISTEN -pthread main.cpp Listener.cpp RUDP.cpp sender.cpp UDPSock.cpp TimeoutTimer.cpp

listener:
	./rudp 6666 127.0.0.1 7777

sender:
	./rudp 7777 127.0.0.1 6666


