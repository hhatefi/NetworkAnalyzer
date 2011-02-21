

all:
	cd src; make -f main.mk; cd ..

clean: 
	cd src; make -f main.mk clean; cd ..
