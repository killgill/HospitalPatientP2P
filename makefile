p1: healthcenter.cpp patient1.cpp
	g++ -o healthcenter healthcenter.cpp -lnsl -lresolv -fpermissive
	g++ -o patient1 patient1.cpp -lnsl -lresolv -fpermissive
