.SUFFIXES : .cpp .o

NAME			:= webserv
CXX				:= c++
CFLAGS			:= -std=c++98 -Werror -Wall -Wextra
DEBUG			:= -g
MEMORY			:= -g3 -fsanitize=address

FG_BLACK		:= \033[90m
FG_RED			:= \033[91m
FG_GREEN		:= \033[92m
FG_YELLOW		:= \033[93m
FG_BLUE			:= \033[94m
FG_MAGENTA		:= \033[95m
FG_CYAN			:= \033[96m
FG_WHITE		:= \033[97m
FG_DEFAULT		:= \033[39m

DEFAULT			:= \033[0m
BOLD			:= \033[1m
ITALIC			:= \033[3m
UNDERLINE		:= \033[4m
REVERSE			:= \033[7m
STRIKETHROUGH	:= \033[9m

BG_BLACK		:= \033[100m
BG_RED			:= \033[101m
BG_GREEN		:= \033[102m
BG_YELLOW		:= \033[103m
BG_BLUE			:= \033[104m
BG_MAGENTA		:= \033[105m
BG_CYAN			:= \033[106m
BG_WHITE		:= \033[107m
BG_DEFAULT		:= \033[49m

RESET 			:= \033[0m

SRC_DIR			:= ./src
OBJ_DIR			:= ./obj

INCLUDE_DIR 	:= $(shell find $(SRC_DIR) -type d)
INCLUDE			:= $(addprefix -I, $(INCLUDE_DIR))

SRC				:= $(shell find $(SRC_DIR) -name "*.cpp")
OBJ				:= $(patsubst $(SRC_DIR)/%.cpp,$(OBJ_DIR)/%.o,$(SRC))

all: $(NAME)

$(NAME): $(OBJ)
	@$(CXX) $(CFLAGS) $(OBJ) -o $(NAME)
	@echo "$(FG_WHITE)Creating $@$(RESET)"

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	@mkdir -p $(dir $@)
	@$(CXX) $(CFLAGS) -c $< -o $@ $(INCLUDE)
	@echo "$(FG_CYAN)Compiled:$(RESET) $< -> $@"

clean:
	@rm -f $(OBJ)
	@echo "$(FG_BLUE)Cleaned up object files$(RESET)"

fclean: clean
	@rm -f $(NAME) log/*.log
	@echo "$(FG_BLUE)Cleaned up executable and log files$(RESET)"

re:
	@$(MAKE) fclean
	@$(MAKE) all

mem:
	@$(MAKE) fclean
	@$(MAKE) all CFLAGS="$(CFLAGS) $(MEMORY)"
	@echo "$(FG_YELLOW)Executable compiled with memory sanitizer$(RESET)"

lldb:
	@$(MAKE) fclean
	@$(MAKE) all CFLAGS="$(CFLAGS) $(DEBUG)"
	@echo "$(FG_YELLOW)Executable compiled to enable LLDB debugging$(RESET)"

.PHONY: all clean fclean re mem lldb
