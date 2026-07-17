# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: tcakir-y <tcakir-y@student.42.fr>          +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2025/07/22 19:01:25 by avaliull          #+#    #+#              #
#    Updated: 2026/07/17 13:02:18 by tcakir-y         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

.DEFAULT_GOAL := all

NAME =	webserv

CXXFILES	=	$(CXXFILES_PARSER)\
				$(CXXFILES_CGI)\
				$(CXXFILES_TIMER)\
				$(HTTP_PARSER)\
				$(RESPONSE_BUILDER)\
				$(CXXFILES_SOCKET)\
				$(MAIN)

CXXFILES_PARSER	=	configParser.cpp\
					configParserTEST.cpp\
					configParserTokenize.cpp\
					serverBlockParsers.cpp\
					configParserHelpers.cpp\
					configParserError.cpp

CXXFILES_CGI =		cgi_exec.cpp

CXXFILES_TIMER =	timer.cpp

HTTP_PARSER =		clientHandler.cpp\
					Http.cpp\
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

CXXFILES_SOCKET =	Server.cpp\
					Signal.cpp\
					Client.cpp\
					ServerUtils.cpp\
					Listener.cpp\
					ListenerDebug.cpp\
					RcvUtils.cpp\
					RcvBuffer.cpp
		
MAIN =				main.cpp

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
CFLAGS	= -Wall -Wextra -Werror -std=c++20
DEBUG_FLAGS	= -g
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
debug:
	+$(MAKE) fclean
	+$(MAKE) CFLAGS="$(CFLAGS) $(DEBUG_FLAGS)" $(NAME)
gdb:
	+$(MAKE) debug
	+gdb -tui ./$(NAME)
test:
	+$(MAKE) $(NAME)
	+$(MAKE) run
run:
	./$(NAME) $(INPUT)
leak:
	+$(MAKE) debug
	+$(MAKE) val
val:
	valgrind --track-fds=yes --track-origins=yes \
	--leak-check=full --show-leak-kinds=all ./$(NAME) $(INPUT)

.PHONY:	clangd all clean fclean re test run leak debug gdb
