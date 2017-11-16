p1: healthcenter.cpp patient1.cpp
	g++ -g healthcenter.cpp -o healthcenter -lnsl -lresolv -fpermissive
	g++ -g patient1.cpp -o patient1 -lnsl -lresolv -fpermissive
	g++ -g patient2.cpp -o patient2 -lnsl -lresolv -fpermissive
	g++ -g patient3.cpp -o patient3 -lnsl -lresolv -fpermissive
	g++ -g patient4.cpp -o patient4 -lnsl -lresolv -fpermissive
	g++ -g doctor1.cpp -o doctor1 -lnsl -lresolv -fpermissive
	g++ -g doctor2.cpp -o doctor2 -lnsl -lresolv -fpermissive
