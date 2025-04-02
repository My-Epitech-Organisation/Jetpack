##
## EPITECH PROJECT, 2025
## B-NWP-400-NAN-4-1-jetpack-santiago.pidcova
## File description:
## Makefile
##

# Name of the executables
CLIENT_BIN = jetpack_client
SERVER_BIN = jetpack_server

# repertory
BUILD_DIR = build

# commands
CD = cd
RM = rm -rf
CMAKE = cmake
MAKE = make

all: client server

client: $(BUILD_DIR)
	@cd $(BUILD_DIR) && $(CMAKE) .. && $(MAKE) $(CLIENT_BIN)
	@cp $(BUILD_DIR)/client/$(CLIENT_BIN) ./

server: $(BUILD_DIR)
	@cd $(BUILD_DIR) && $(CMAKE) .. && $(MAKE) $(SERVER_BIN)
	@cp $(BUILD_DIR)/server/$(SERVER_BIN) ./

$(BUILD_DIR):
	@mkdir -p $(BUILD_DIR)

tests_run:
	@echo "Running tests... (placeholder)"
	@exit 0

clean:
	@$(RM) $(BUILD_DIR)/*
	@$(RM) $(CLIENT_BIN)
	@$(RM) $(SERVER_BIN)

fclean: clean
	@$(RM) $(BUILD_DIR)

re: fclean all

.PHONY: all clean fclean re tests_run
