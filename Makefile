CFLAGS = -lm
all: bin/super_mario_mpi
	@echo ""
	@echo "Excecutable: bin/super_mario_mpi"
	

bin/super_mario_mpi:  src/main.cpp src/SMPlayer.cpp src/SMController.cpp src/SMObject.cpp src/SMWorld.cpp src/SMCharacter.cpp | bin/.
	mpic++ $(CFLAGS) $^ -o $@

.PRECIOUS: %/.
%/.:
	mkdir -p $(dir $@)

.PHONY: clean
clean:
	rm -rf bin