TGT=ocr-tesslineboxes
SRC=ocr-tesslineboxes.go
BINDIR=${HOME}/.local/bin

all: build

build:
	go build ${SRC}

install: ${TGT}
	install -d ${BINDIR}
	install $+ ${BINDIR}

.PHONY: build install
