NAME = httpTest

PURPLE = \033[35m
CYAN = \033[36m
BLUE = \033[34m
PINK = \033[95m
GREEN = \033[1;32m
RESET = \033[0m

COMP = c++
FLAGS = -Wall -Wextra -Werror -std=c++20 
SRC_DIR = ./
OBJ_DIR = obj/

HEADER_FILES = Http.hpp httpBuffer.hpp
SRC_FILES = test.cpp Http.cpp httpParsers.cpp httpGetters.cpp httpSetters.cpp httpUtils.cpp recv.cpp httpBuffer.cpp
OBJ = $(addprefix $(OBJ_DIR), $(SRC_FILES:.cpp=.o))

all: $(NAME)

$(NAME): $(OBJ)
	@$(COMP) $(FLAGS) $(OBJ) -o $(NAME)
	@echo "$(GREEN)Compilation successful! Program name: $(NAME) $(RESET)"

$(OBJ_DIR)%.o: $(SRC_DIR)%.cpp $(HEADER_FILES)
	@mkdir -p $(OBJ_DIR)
	@echo "$(PURPLE)Compiling $<... $(RESET)"
	@$(COMP) $(FLAGS) -c $< -o $@

clean:
	@$(RM) -rf $(OBJ_DIR)
	@echo "$(BLUE)Object files removed $(RESET)"

fclean: clean
	@$(RM) -f $(NAME)
	@echo "$(BLUE)Executable file removed $(RESET)"

re: fclean all
	@echo "$(PINK)Object and executable files successfully rebuilt $(RESET)"

.PHONY: all re clean fclean