# **************************************************************************** #
#                                                                              #
#                                                        ::::::::              #
#    Makefile                                          :+:    :+:              #
#                                                     +:+                      #
#    By: avaliull <avaliull@student.codam.nl>        +#+                       #
#                                                   +#+                        #
#    Created: 2025/07/22 19:01:25 by avaliull     #+#    #+#                   #
#    Updated: 2025/08/06 15:25:10 by avaliull     ########   odam.nl           #
#                                                                              #
# **************************************************************************** #

.DEFAULT_GOAL := all

NAME =	config_parser

CXXFILES	=	$(CXXFILES_PARSER)\
				$(CXXFILES_CGI)\
				$(CXXFILES_TIMER)\
				$(HTTP_PARSER)\
				$(RESPONSE_BUILDER)\
				$(CXXFILES_SOCKET)

CXXFILES_PARSER	=	config_parser_main.cpp\
					configParser.cpp\
					configParserTEST.cpp\
					configParserTokenize.cpp\
					serverBlockParsers.cpp\
					configParserHelpers.cpp\
					configParserError.cpp

CXXFILES_CGI =		cgi_exec.cpp

CXXFILES_TIMER =	timer.cpp

HTTP_PARSER =	clientHandler.cpp\
				Http.cpp\
				httpBuffer.cpp\
				clientHandler.cpp\
				httpParsers.cpp\
				httpValidate.cpp\
				setRequestConfig.cpp

RESPONSE_BUILDER =	buildResponse.cpp\
					buildResponseAutoIndexReturn.cpp\
					buildResponseDelete.cpp\
					buildResponseError.cpp\
					buildResponseGet.cpp\
					buildResponsePost.cpp\
					buildResponseUtils.cpp

CXXFILES_SOCKET =	Listener.cpp

OFILES	= $(addprefix $(BUILDDIR),$(CXXFILES:.cpp=.o))
DEPFILES	= $(addprefix $(BUILDDIR),$(CXXFILES:.cpp=.d))

VPATH	= $(INCLUDE) $(SRCDIRS)
BUILDDIR = build/
SRCDIR = src/
INCDIR = inc/
SRCDIRS = $(SRCDIR)\
			$(addprefix $(SRCDIR),\
				config\
				cgi_exec\
				timer\
				http\
				response\
				socket\
			)
%/:
	mkdir -p $@

INCLUDE = $(INCDIR)

RM	= rm -rf
CC	= c++

# this apparently achieves dependency gen on per-file basis, on compilation
# without relying on hacky solutions like the one on GNU Makefile docs
# -MMD is a combination of a few flags:
#  -M generates dependency files
#  -MD is the same as -M -MF file
#  -MF file specifies file to write dependencies to
#  -MP adds a phony target so make does not cry when a .h file is removed
CPPFLAGS	= $(INCFLAGS) -MMD -MP

INCFLAGS	= $(addprefix -I,$(INCLUDE))
#CFLAGS	= -Wall -Wextra -Werror
CFLAGS	= -Wall -Wextra -Werror -fsanitize=undefined -std=c++20
LDFLAGS	=
INPUT	= config/test.conf
MAKEFLAGS += -j --no-print-directory


-include $(OFILES:.o=.d)

# builds .d files, then builds .o files based on .d.
# skips files that weren't changed (see CPPFLAGS)
$(BUILDDIR)%.o: %.cpp $(INCLUDE) | $(BUILDDIR)
	$(CC) $(CFLAGS) $(CPPFLAGS) -c $< -o $@

$(NAME): $(OFILES)
	$(CC) $(CFLAGS) -o $@ $(OFILES) $(LDFLAGS) $(INCFLAGS)

#Base/project requirements
all: $(NAME)
#libs_clean:
#	$(MAKE) fclean -C $(LIBFT_DIR)
clean:
	$(RM) $(OFILES)
fclean:	clean #libs_clean
	$(RM) $(NAME) $(DEPFILES)
re:
	+$(MAKE) fclean
	+$(MAKE) all

#LSP connection for neovim
clangd:
	$(MAKE) fclean
	intercept-build-14 $(MAKE)

#debugging
debug: CFLAGS += -g
debug: clean $(NAME)
gdb: fclean debug
	gdb -tui ./$(NAME)
test:
	+$(MAKE) $(NAME)
	+$(MAKE) run
run:
	./$(NAME) $(INPUT)
leak:
	+$(MAKE) -s debug
	+valgrind --track-fds=yes --track-origins=yes \
	--leak-check=full --show-leak-kinds=all ./$(NAME) $(INPUT)
val:
	valgrind --track-fds=yes --track-origins=yes \
	--leak-check=full --show-leak-kinds=all ./$(NAME) $(INPUT)

.PHONY:	clangd all clean fclean re test run leak debug gdb
