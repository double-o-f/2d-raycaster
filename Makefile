game: out/ src/game_main.c RC_math.o RC_player.o RC_map.o RC_rend.o RC_window.o RC_input.o RC_state.o RC_ui.o
	gcc src/game_main.c -o game -Iinclude/ out/RC_math.o out/RC_player.o out/RC_map.o out/RC_rend.o out/RC_window.o out/RC_input.o out/RC_state.o out/RC_ui.o -lm -lSDL2 -g

mapgen: out/ src/mapgen_main.c RC_math.o RC_player.o RC_map.o RC_state.o
	gcc src/mapgen_main.c -o mapgen -Iinclude/ out/RC_math.o out/RC_player.o out/RC_map.o out/RC_state.o -lm -g

out/:
	mkdir out

RC_math.o: src/RC_math.c
	gcc -c src/RC_math.c -o out/RC_math.o -Iinclude/ -lm -g

RC_player.o: src/RC_player.c
	gcc -c src/RC_player.c -o out/RC_player.o -Iinclude/ -lm -g

RC_map.o: src/RC_map.c
	gcc -c src/RC_map.c -o out/RC_map.o -Iinclude/ -g

RC_rend.o: src/RC_rend.c
	gcc -c src/RC_rend.c -o out/RC_rend.o -Iinclude/ -lm -g

RC_window.o: src/RC_window.c
	gcc -c src/RC_window.c -o out/RC_window.o -Iinclude/ -lSDL2 -g

RC_input.o: src/RC_input.c
	gcc -c src/RC_input.c -o out/RC_input.o -Iinclude/ -lSDL2 -g

RC_state.o: src/RC_state.c
	gcc -c src/RC_state.c -o out/RC_state.o -Iinclude/ -g

RC_ui.o: src/RC_ui.c
	gcc -c src/RC_ui.c -o out/RC_ui.o -Iinclude/ -g
