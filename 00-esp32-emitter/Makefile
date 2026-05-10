# === ESP32 Arduino CLI Makefile ===

# Set these to match your system & board:
SKETCH_DIR := /home/master/Dropbox/A/scholar/sherbrooke/26-05-S6/app1/out/00-esp32-emitter
PORT := /dev/ttyUSB0
FQBN := esp32:esp32:esp32
BAUD := 115200

CLI := arduino-cli

.PHONY: all compile upload monitor upload-monitor clean

all: compile

compile:
	$(CLI) compile --fqbn $(FQBN) $(SKETCH_DIR)

upload: compile
	$(CLI) upload -p $(PORT) --fqbn $(FQBN) $(SKETCH_DIR)

monitor:
	$(CLI) monitor -p $(PORT) -c baudrate=$(BAUD) -b $(FQBN)

# One-shot: Upload then monitor (with automatic short delay to allow board reset)
upload-monitor: upload
	sleep 1 && $(CLI) monitor -p $(PORT) -c baudrate=$(BAUD) -b $(FQBN)

clean:
	rm -rf $(SKETCH_DIR)/build

setup:
	$(CLI) core install esp32:esp32
	$(CLI) lib install "Adafruit DPS310@1.1.6"
	$(CLI) lib install "Adafruit Unified Sensor@1.1.15"
	$(CLI) lib install "Adafruit BusIO@1.17.4"
