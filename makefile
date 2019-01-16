CC = gcc


OBJS_PATH = main.o GUI/GuiApi/GuiCommonSdlHelpers.o GUI/GuiApi/WidgetPrimitive.o GUI/GuiApi/Window.o GUI/GuiApi/Panel.o GUI/GuiApi/Button.o GUI/GuiApi/AffecterBtn.o GUI/GuiApi/GuiConfigSamples.o \
GUI/ChessGui/Cell.o GUI/ChessGui/ChessGameGuiController.o GUI/ChessGui/ChessGuiManager.o \
BoardGame/BoardGame.o ChessGame/ChessGame.o ChessGame/ChessGameBoard.o \
ChessGame/ChessGameViewOutput.o ChessGame/ChessAI.o DataStructs/ArrayList.o \
IO/FileIO.o IO/Parser.o IO/BasicErrorsHandling.o MemGateway/MemGateway.o


OBJS = main.o GuiCommonSdlHelpers.o WidgetPrimitive.o Window.o Panel.o Button.o \
AffecterBtn.o GuiConfigSamples.o Cell.o ChessGameGuiController.o ChessGuiManager.o \
BoardGame.o ChessGame.o ChessGameBoard.o ChessGameViewOutput.o ChessAI.o \
ArrayList.o FileIO.o Parser.o BasicErrorsHandling.o MemGateway.o

EXEC = chessprog
COMP_FLAG = -std=c99 -Wall -Wextra \
-Werror -pedantic-errors
SDL_COMP_FLAG = -I/usr/local/lib/sdl_2.0.5/include/SDL2 -D_REENTRANT
SDL_LIB = -L/usr/local/lib/sdl_2.0.5/lib -Wl,-rpath,/usr/local/lib/sdl_2.0.5/lib -Wl,--enable-new-dtags -lSDL2 -lSDL2main

all: chessprog

$(EXEC): $(OBJS_PATH)
	$(CC) $(OBJS) $(SDL_LIB) -o $@

main.o: main.c
	$(CC) $(COMP_FLAG) $(SDL_COMP_FLAG) -c $*.c

IO/BasicErrorsHandling.o: IO/BasicErrorsHandling.c IO/BasicErrorsHandling.h
	$(CC) $(COMP_FLAG) $(SDL_COMP_FLAG) -c $*.c

MemGateway/MemGateway.o: MemGateway/MemGateway.c MemGateway/MemGateway.h IO/BasicErrorsHandling.h
	$(CC) $(COMP_FLAG) $(SDL_COMP_FLAG) -c $*.c

DataStructs/ArrayList.o: DataStructs/ArrayList.c DataStructs/ArrayList.h
	$(CC) $(COMP_FLAG) $(SDL_COMP_FLAG) -c $*.c

BoardGame/BoardGame.o: BoardGame/BoardGame.c BoardGame/BoardGame.h
	$(CC) $(COMP_FLAG) $(SDL_COMP_FLAG) -c $*.c

ChessGame/ChessGameBoard.o: ChessGame/ChessGameBoard.c ChessGame/ChessGameBoard.h BoardGame/BoardGame.h DataStructs/ArrayList.h
	$(CC) $(COMP_FLAG) $(SDL_COMP_FLAG) -c $*.c

IO/Parser.o: IO/Parser.c IO/Parser.h 
	$(CC) $(COMP_FLAG) $(SDL_COMP_FLAG) -c $*.c

ChessGame/ChessGameViewOutput.o: ChessGame/ChessGameViewOutput.c ChessGame/ChessGameViewOutput.h ChessGame/ChessGameBoard.h BoardGame/BoardGame.h DataStructs/ArrayList.h
	$(CC) $(COMP_FLAG) $(SDL_COMP_FLAG) -c $*.c

IO/FileIO.o: IO/FileIO.c IO/FileIO.h IO/Parser.h ChessGame/ChessGameViewOutput.h ChessGame/ChessGameBoard.h BoardGame/BoardGame.h DataStructs/ArrayList.h
	$(CC) $(COMP_FLAG) $(SDL_COMP_FLAG) -c $*.c

ChessGame/ChessGame.o: ChessGame/ChessGame.c ChessGame/ChessGame.h IO/FileIO.h IO/Parser.h ChessGame/ChessGameViewOutput.h ChessGame/ChessGameBoard.h BoardGame/BoardGame.h DataStructs/ArrayList.h
	$(CC) $(COMP_FLAG) $(SDL_COMP_FLAG) -c $*.c
	
ChessGame/ChessAI.o: ChessGame/ChessAI.c ChessGame/ChessAI.h ChessGame/ChessGame.h IO/FileIO.h IO/Parser.h ChessGame/ChessGameViewOutput.h ChessGame/ChessGameBoard.h BoardGame/BoardGame.h DataStructs/ArrayList.h
	$(CC) $(COMP_FLAG) $(SDL_COMP_FLAG) -c $*.c
	

GUI/GuiApi/GuiCommonSdlHelpers.o: GUI/GuiApi/GuiCommonSdlHelpers.c GUI/GuiApi/GuiCommonSdlHelpers.h
	$(CC) $(COMP_FLAG) $(SDL_COMP_FLAG) -c $*.c

GUI/GuiApi/WidgetPrimitive.o: GUI/GuiApi/WidgetPrimitive.c GUI/GuiApi/WidgetPrimitive.h \
GUI/GuiApi/GuiCommonSdlHelpers.h
	$(CC) $(COMP_FLAG) $(SDL_COMP_FLAG) -c $*.c

GUI/GuiApi/Window.o: GUI/GuiApi/Window.c GUI/GuiApi/Window.h GUI/GuiApi/WidgetPrimitive.h
	$(CC) $(COMP_FLAG) $(SDL_COMP_FLAG) -c $*.c
	
GUI/GuiApi/Panel.o: GUI/GuiApi/Panel.c GUI/GuiApi/Panel.h GUI/GuiApi/WidgetPrimitive.h
	$(CC) $(COMP_FLAG) $(SDL_COMP_FLAG) -c $*.c

GUI/GuiApi/Button.o: GUI/GuiApi/Button.c GUI/GuiApi/Button.h GUI/GuiApi/WidgetPrimitive.h GUI/GuiApi/GuiCommonSdlHelpers.h
	$(CC) $(COMP_FLAG) $(SDL_COMP_FLAG) -c $*.c

GUI/GuiApi/AffecterBtn.o: GUI/GuiApi/AffecterBtn.c GUI/GuiApi/AffecterBtn.h GUI/GuiApi/Button.h
	$(CC) $(COMP_FLAG) $(SDL_COMP_FLAG) -c $*.c

GUI/GuiApi/GuiConfigSamples.o: GUI/GuiApi/GuiConfigSamples.c GUI/GuiApi/GuiConfigSamples.h GUI/GuiApi/AffecterBtn.h GUI/GuiApi/Button.h
	$(CC) $(COMP_FLAG) $(SDL_COMP_FLAG) -c $*.c

GUI/ChessGui/Cell.o: GUI/ChessGui/Cell.c GUI/ChessGui/Cell.h GUI/GuiApi/Button.h
	$(CC) $(COMP_FLAG) $(SDL_COMP_FLAG) -c $*.c

GUI/ChessGui/ChessGameGuiController.o: GUI/ChessGui/ChessGameGuiController.c \
GUI/ChessGui/ChessGameGuiController.h GUI/ChessGui/Cell.h  GUI/GuiApi/WidgetPrimitive.h \
GUI/GuiApi/Window.h GUI/GuiApi/Panel.h GUI/GuiApi/Button.h \
GUI/GuiApi/AffecterBtn.h GUI/GuiApi/GuiConfigSamples.h
	$(CC) $(COMP_FLAG) $(SDL_COMP_FLAG) -c $*.c
	
GUI/ChessGui/ChessGuiManager.o: GUI/ChessGui/ChessGuiManager.c GUI/ChessGui/ChessGuiManager.h GUI/ChessGui/ChessGameGuiController.h
	$(CC) $(COMP_FLAG) $(SDL_COMP_FLAG) -c $*.c

clean:
	rm -f *.o $(EXEC)
