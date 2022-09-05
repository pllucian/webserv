# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: pllucian <pllucian@21-school.ru>           +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2022/02/23 13:26:20 by pllucian          #+#    #+#              #
#    Updated: 2022/09/01 12:23:20 by pllucian         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

SRCS =	WebServ.cpp \
		Http.cpp \
		Request.cpp \
		Response.cpp \
		Config.cpp \
		Location.cpp \
		Server.cpp \
		Client.cpp \
		main.cpp

INCL =	WebServ.hpp \
		Http.hpp \
		Request.hpp \
		Response.hpp \
		Config.hpp \
		Location.hpp \
		Server.hpp \
		Client.hpp

OBJS = $(addprefix objs/, $(SRCS:.cpp=.o))

DEPS = $(wildcard objs/*.d)

NAME = webserv

CXX = c++

CFLAGS = -Wall -Wextra -Werror -std=c++98 -g

objs/%.o:	srcs/%.cpp
			@ [ -d objs ] || mkdir -p objs
			$(CXX) $(CFLAGS) -MMD -c $< -o $@ -I./includes

$(NAME):	$(OBJS)
			$(CXX) -o $@ $^
			@ [ -d www/testsite/upload ] || mkdir -p www/testsite/upload
			@ [ -d www/website0/upload ] || mkdir -p www/website0/upload
			@ [ -d www/website1/upload ] || mkdir -p www/website1/upload

all:		$(NAME)

clean:
			$(RM)r $(OBJS) $(DEPS) objs

fclean:		clean
			$(RM) $(NAME)
			@ $(RM)r www/testsite/upload
			@ $(RM)r www/website0/upload
			@ $(RM)r www/website1/upload

re:			fclean all

.PHONY:		all clean fclean re

include		$(DEPS)
