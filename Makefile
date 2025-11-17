#SMFL Makefile by github.com/doxakis1

# For Linux users
#
# Before you run this makefile make sure to install g++ compiler and smfl
# you can add them both using the following terminal command: 
# "sudo apt-get install g++ build-essential libsfml-dev libyaml-cpp-dev"
#
# For Mac users
# Before you run this makefile make sure to install g++ compiler and sfml
# you can add them both using the following brew command:
# "brew install sfml g++"

# This makefile relies that you have some existing folders in your code
# A source/ directory with all your source cpp files
# A recommended includes/ directory with all your hpp header files (personal preferance)
# A bin/ directory where your executable will be placed

#define the compiler you want to use:
COMPILER		:= g++

#define the name of the executable:
NAME			:= draw_topology

#Use the following compiler flags, note that you may use whichever c++ version you want
# in my case I use c++17
# the O3 flag is for optimisation and is also optional but highly recommended
COMPILER_FLAGS	:= -O3 -std=c++17

#Following is your includes directory with all your headers
INCLUDES_DIRECTORY	:= inc

#Following is your source directory with all your source files
SOURCES_DIRECTORY	:= src

#The following define your includes:
INCLUDES 		:= -I./$(SOURCES_DIRECTORY) -I./$(INCLUDES_DIRECTORY)

#The following defines your linker flags. Once again O3 is optional but highly recommended
LINKER_FLAGS	:= -O3 -lsfml-graphics -lsfml-window -lsfml-system -lsfml-audio -lyaml-cpp

#The following defines your sources
SOURCE_FILES	:= $(wildcard $(SOURCES_DIRECTORY)/*.cpp)

#The following defines your object files that get made during compilation
OBJECT_FILES	:= $(SOURCE_FILES:.cpp=.o)

#all rule
all:$(NAME)

#Instructions for generating .o files:
.cpp.o:
	$(COMPILER) -c $(COMPILER_FLAGS) $(INCLUDES) $< -o $@

#Linking phase:
$(NAME):$(OBJECT_FILES) Makefile
	mkdir -p ./bin
	$(COMPILER) $(OBJECT_FILES) $(LINKER_FLAGS) -o ./bin/$(NAME)

#generic clean
clean:
	rm -rf $(OBJECT_FILES)

#generic fclean
fclean:
	rm -rf $(OBJECT_FILES) ./bin/$(NAME)

#test command to compile and run your executable
test:$(NAME)
	./bin/$(NAME)

