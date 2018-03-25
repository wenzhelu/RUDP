

compile:
	g++ -o rudp -std=c++11 main.cpp Listener.cpp RUDP.cpp sender.cpp UDPSock.cpp

debug_compile:
	g++ -o rudp -std=c++11 -DDEBUG main.cpp Listener.cpp RUDP.cpp sender.cpp UDPSock.cpp

debug_listener:
	g++ -o rudp -std=c++11 -DDEBUG -DLISTEN main.cpp Listener.cpp RUDP.cpp sender.cpp UDPSock.cpp

run_listener:
	./rudp 7777 127.0.0.1 6666


run_sender:
	./rudp 6666 127.0.0.1 7777


