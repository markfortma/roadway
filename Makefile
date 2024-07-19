roadway: roadway.c
	$(CC) $(CFLAGS) $(CINCS) $(CLIBS) -o $@ $<

.PHONY: clean
clean:
	$(RM) roadway *~

