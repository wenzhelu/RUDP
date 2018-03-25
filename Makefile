

compile:
	g++ -o rudp -std=c++11 -pthread main.cpp Listener.cpp RUDP.cpp sender.cpp UDPSock.cpp TimeoutTimer.cpp

debug_compile:
	g++ -o rudp -std=c++11 -DDEBUG -pthread main.cpp Listener.cpp RUDP.cpp sender.cpp UDPSock.cpp TimeoutTimer.cpp

debug_listener:
	g++ -o rudp -std=c++11 -DDEBUG -DLISTEN -pthread main.cpp Listener.cpp RUDP.cpp sender.cpp UDPSock.cpp TimeoutTimer.cpp

run_listener:
	./rudp 7777 127.0.0.1 6666

run_sender:
	./rudp 6666 127.0.0.1 7777


