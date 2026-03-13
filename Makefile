# ════════════════════════════════════════════════ #
#                   🎨 STYLING                    #
# ════════════════════════════════════════════════ #
RED     = \033[31m
GREEN   = \033[32m
WHITE   = \033[37m
BROWN   = \033[33m
YELLOW  = \033[93m
RESET   = \033[0m
BLUE    = \033[34m
CYAN    = \033[36m
GRAY    = \033[90m
MAGENTA = \033[35m

# ════════════════════════════════════════════════ #
#                 🔧 VARIABLES                    #
# ════════════════════════════════════════════════ #
NAME = webserv

# Répertoires
SRC_DIR = Srcs
OBJ_DIR = obj
INCLUDE_DIR = Includes

# Commandes
CXX = c++
CXXFLAGS = -Wall -Wextra -Werror -std=c++98 -I$(INCLUDE_DIR)
RM = rm -f

# Fichiers sources / objets
SRC_FILES = $(shell find $(SRC_DIR) -name "*.cpp")
OBJ_FILES = $(patsubst $(SRC_DIR)/%.cpp, $(OBJ_DIR)/%.o, $(SRC_FILES))

TOTAL = $(words $(SRC_FILES))
INDEX = 0

# ════════════════════════════════════════════════ #
#              📊 PROGRESS BAR MACRO               #
# ════════════════════════════════════════════════ #
define PROGRESS_BAR
BAR_WIDTH=30 ; \
PCT=$$(expr 100 \* $(1) / $(TOTAL)) ; \
FILLED=$$(expr $(1) \* $$BAR_WIDTH / $(TOTAL)) ; \
EMPTY=$$(expr $$BAR_WIDTH - $$FILLED) ; \
BAR=$$(printf "%0.s$(CYAN)▮$(RESET)" $$(seq 1 $$FILLED)) ; \
BAR=$$BAR$$(printf "%0.s$(BLUE)▯$(RESET)" $$(seq 1 $$EMPTY)) ; \
printf "\r[$$BAR] $$PCT%% - Compiling $(YELLOW)$(<F)$(RESET) ... "
endef

# ════════════════════════════════════════════════ #
#              🛠️ COMPILATION RULES               #
# ════════════════════════════════════════════════ #
all: $(NAME)

$(NAME): $(OBJ_FILES)
	@$(CXX) $(CXXFLAGS) $(OBJ_FILES) -o $(NAME)
	@printf "\r["
	@for i in $(shell seq 1 30); do printf "$(CYAN)▮$(RESET)"; done
	@printf "] 100%% - $(GREEN)✔️  Executable created: $(CYAN)$(NAME) 🚀$(RESET)\n"

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp | $(OBJ_DIR)
	@mkdir -p $(dir $@)
	@$(eval INDEX := $(shell expr $(INDEX) + 1))
	@$(call PROGRESS_BAR,$(INDEX))
	@$(CXX) $(CXXFLAGS) -c $< -o $@ \
	&& printf "$(GREEN)✔️ $(RESET)" \
	|| printf "$(RED)❌$(RESET)\n"

$(OBJ_DIR):
	@mkdir -p $@

# ════════════════════════════════════════════════ #
#                🧼 CLEANING RULES                #
# ════════════════════════════════════════════════ #
clean:
	@rm -rf $(OBJ_DIR)
	@printf "$(BROWN)🧹 Cleaning object files...$(RESET)\n"

fclean: clean
	@rm -f $(NAME)
	@printf "$(YELLOW)🗑️ Removed executable: $(GRAY)$(NAME)$(RESET)\n"

re:
	@$(MAKE) --no-print-directory fclean
	@$(MAKE) --no-print-directory all

.PHONY: all clean fclean re
