PORT = /dev/ttyACM0
BAUD = 115200
FQBN = esp32:esp32:esp32s3:FlashSize=16M,PSRam=opi,CDCOnBoot=cdc
BUILD_DIR = ./build

all: compile

compile:
	@echo "==> Compiling project for ESP32S3..."
	arduino-cli compile --fqbn $(FQBN) --build-path $(BUILD_DIR)

upload:
	@echo "Uploading in PORT $(PORT)..."
	arduino-cli upload -p $(PORT) --fqbn $(FQBN) --input-dir $(BUILD_DIR)

monitor:
	@echo "Open Serial Terminal ( Ctrl + C to exit )..."
	arduino-cli monitor -p $(PORT) -c baudrate=$(BAUD)

run:
	compile upload monitor

git-push:
	@echo "Checkin Git's status..."
	git add .
	@git status
	@echo "Entering commit message: " msg; \
	if [ -z "$$msg"]; then msg="Update code ESP32S3"; fi; \
	git commit -m "$$msg"
	@echo "Pushing code up Github..."
	git push origin main
	@echo "Completed sync Git!"

clean:
	rm -rf $(BUILD_DIR)
	@echo "Removed recent ./build  :v"  
