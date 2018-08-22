all:
	g++ -O3 server.cpp -o server `pkg-config opencv --cflags --libs`
clean:
	rm server


