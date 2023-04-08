PIO := platformio
BOARD := nanoatmega328

all:	build

build:	clean
	$(PIO) run --environment $(BOARD)

upload:	build
	$(PIO) run --target upload --environment $(BOARD)
	$(PIO) device monitor

get_serial:
	$(PIO) device list --serial

monitor:
	$(PIO) device monitor

clean:
	$(PIO) run -t clean
