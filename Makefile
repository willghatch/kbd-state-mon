kbd-state-mon: kbd-state-mon.c
	gcc -Wall -o kbd-state-mon kbd-state-mon.c -lX11
	strip kbd-state-mon

install:
	install -Dm 755 kbd-state-mon $(PREFIX)/bin/kbd-state-mon
