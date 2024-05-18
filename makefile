make:
	rm ./game_files/saved_progress/added_clicks ./game_files/saved_progress/score ./game_files/saved_progress/cost_of_upgrade
	echo "1" >> ./game_files/saved_progress/added_clicks
	echo "0" >> ./game_files/saved_progress/score
	echo "5" >> ./game_files/saved_progress/cost_of_upgrade
	g++ -c main.cpp
	g++ -o koza_clicker main.o -lsfml-system -lsfml-graphics -lsfml-window
	./koza_clicker
