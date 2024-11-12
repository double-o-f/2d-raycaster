game: out/ src/game_main.c MT_math.o PL_player.o MP_map.o RD_rend.o WI_window.o IN_input.o ST_state.o UI_ui.o
	gcc src/game_main.c -o game -Iinclude/ out/MT_math.o out/PL_player.o out/MP_map.o out/RD_rend.o out/WI_window.o out/IN_input.o out/ST_state.o out/UI_ui.o -lm -lSDL2 -g

mapgen: out/ src/mapgen_main.c MT_math.o PL_player.o MP_map.o ST_state.o
	gcc src/mapgen_main.c -o mapgen -Iinclude/ out/MT_math.o out/PL_player.o out/MP_map.o out/ST_state.o -lm -g

out/:
	mkdir out

MT_math.o: src/MT_math.c
	gcc -c src/MT_math.c -o out/MT_math.o -Iinclude/ -lm -g

PL_player.o: src/PL_player.c
	gcc -c src/PL_player.c -o out/PL_player.o -Iinclude/ -lm -g

MP_map.o: src/MP_map.c
	gcc -c src/MP_map.c -o out/MP_map.o -Iinclude/ -g

RD_rend.o: src/RD_rend.c
	gcc -c src/RD_rend.c -o out/RD_rend.o -Iinclude/ -lm -g

WI_window.o: src/WI_window.c
	gcc -c src/WI_window.c -o out/WI_window.o -Iinclude/ -lSDL2 -g

IN_input.o: src/IN_input.c
	gcc -c src/IN_input.c -o out/IN_input.o -Iinclude/ -lSDL2 -g

ST_state.o: src/ST_state.c
	gcc -c src/ST_state.c -o out/ST_state.o -Iinclude/ -g

UI_ui.o: src/UI_ui.c
	gcc -c src/UI_ui.c -o out/UI_ui.o -Iinclude/ -g
