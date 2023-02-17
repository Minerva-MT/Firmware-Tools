SOURCE = Source
DEST   = Build

all: Merger PFSPacker PFSUnpacker BinarySearcher HexDump Serial Padder

clean:
	rm $(DEST)/*

Merger:
	$(CC) $(SOURCE)/Merger.c $(SOURCE)/Common.c -o $(DEST)/Merger

PFSPacker:
	$(CC) $(SOURCE)/PFSPacker.c $(SOURCE)/Common.c -o $(DEST)/PFSPacker

PFSUnpacker:
	$(CC) $(SOURCE)/PFSUnpacker.c $(SOURCE)/Common.c -o $(DEST)/PFSUnpacker

BinarySearcher:
	$(CC) $(SOURCE)/BinarySearcher.c $(SOURCE)/Common.c -o $(DEST)/BinarySearcher -lsqlite3

HexDump:
	$(CC) $(SOURCE)/HexDump.c $(SOURCE)/Common.c -o $(DEST)/HexDump

Serial:
	$(CC) $(SOURCE)/Serial.c $(SOURCE)/Common.c -o $(DEST)/Serial

Padder:
	$(CC) $(SOURCE)/Padder.c $(SOURCE)/Common.c -o $(DEST)/Padder
