OBJDIR = obj
SRCDIR = src
BINDIR = bin

CLIENT = $(BINDIR)/client.jar
SERVER = $(BINDIR)/server.jar

CLINETMAIN = gbn.GBNClient
SERVERMAIN = gbn.GBNServer

SRC = $(wildcard src/gbn/*.java)
SRC += $(wildcard src/timerPackage/*.java)
SRC += $(wildcard src/tool/*.java)

TMP = $(patsubst %.java,%.class,$(SRC))
CLASS = $(patsubst $(SRCDIR)/%,$(OBJDIR)/%,$(TMP))

main: $(SRC) $(BIN_DIR) $(OBJ_DIR)
	javac -d $(OBJDIR) $(SRC)
	jar cfe $(CLIENT) $(CLINETMAIN) $(OBJDIR)/
	jar cfe $(SERVER) $(SERVERMAIN) $(OBJDIR)/

$(BIN_DIR):
	mkdir -p $(BIN_DIR)
$(OBJ_DIR):
	mkdir -p $(OBJ_DIR)

run:
	java -jar $(CLIENT)
	java -jar $(SERVER)

clean:
	$(RM) $(CLASS) $(CLIENT) $(SERVER)

.PHONY: clean