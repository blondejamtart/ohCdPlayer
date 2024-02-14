cd-testapp : CdSource.o
	g++ -o cd-testapp CdSource.o DemoApp.cpp -lcdio

CdSource.o :
	g++ -c CdSource.cpp -lcdio

clean :
	rm CdSource.o cd-testapp