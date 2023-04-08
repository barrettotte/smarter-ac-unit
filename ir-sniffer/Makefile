PIO := platformio
PIO_ENV := nano

all:	build

build:	clean
	$(PIO) run --environment $(PIO_ENV)

upload:	build
	$(PIO) run --target upload --environment $(PIO_ENV)
	$(PIO) device monitor

get_serial:
	$(PIO) device list --serial

monitor:
	$(PIO) device monitor

clean:
	$(PIO) run -t clean
