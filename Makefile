CFLAGS := -std=c17 -O3
CLIBS  := -pthread

roadway: roadway.c
	$(CC) $(CFLAGS) $(CINCS) $(CLIBS) -o $@ $<

.PHONY: clean
clean:
	$(RM) roadway *~

