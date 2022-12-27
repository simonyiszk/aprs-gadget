#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

int main(void)
{
	printf("csump\n");

	const int samples_per_bit = 7; // (source sampling rate / 1200Bps)
	FILE *source = fopen("digital.txt", "r");
	int len = 0;   //number of samples from source signal
	int trash = 0; //whatever

	while (!feof(source))
	{
		fscanf(source, "%d", &trash);
		len++;
	}
	printf("%d samples found\n", len);
	rewind(source);

	int bitcount = 0;
	int timer = 0;
	bool lastsample = false;
	bool lastbit = false;
	bool *samples = (bool *)malloc(len * sizeof(bool));

	//TODO: NRZI miatt átnázni!!!

	for (int i = 0; i < len; i++)
	{
		fscanf(source, "%d", &trash);
		samples[i] = (trash == 1);
		if (i == 0)
		{ // starting state
			lastsample = samples[i];
		}
		else if (lastsample != samples[i])
		{ // edge detection
			lastsample = samples[i];
			timer = samples_per_bit / 2;
			lastbit = 0;
			bitcount++; //NRZI: toggle = 0 bit
		}

		timer++;

		if (timer % samples_per_bit == 0)
		{ // sampling timeout at every half T_bit period
			if (lastbit == 0)
			{
				lastbit = 1;
			}
			else
			{
				bitcount++;
			}
			//timer = 0;
		}
	}

	printf("%d bits found\n", bitcount);
	rewind(source);

	bool *bits = (bool *)malloc(bitcount * sizeof(bool));
	bitcount = 0;

	for (int i = 0; i < len; i++)
	{
		fscanf(source, "%d", &trash);
		samples[i] = (trash == 1);
		if (i == 0)
		{ // starting state
			lastbit = samples[i];
		}
		else if (lastsample != samples[i])
		{ // edge detection
			lastsample = samples[i];
			timer = samples_per_bit / 2;
			lastbit = 0;
			bits[bitcount] = 0;
			bitcount++; //NRZI: toggle = 0 bit
		}

		timer++;

		if (timer % samples_per_bit == 0)
		{ // sampling timeout at every half T_bit period
			if (lastbit == 0)
			{
				lastbit = 1;
			}
			else
			{
				bits[bitcount] = 1;
				bitcount++;
			}
			//timer = 0;
		}
	}

	unsigned char c = 0;
	int offset = 8;
	unsigned char state = 0; //0-not locked, 1-locked, 2-addr, 3-control, 4-pid, 5-text

	printf("LSB  MSB\n");
	for (int i = 0; i < bitcount; i++)
	{
		//printf("%d", bits[i]);
		c = (c >> 1) | (bits[i] << 7); //convert to LSB FIRST!!!
		if (!((i - offset) % 8))
		{ //trying to build data byte
			if (state == 0)
			{
				if (c != 0x7E)
				{ //sync pattern not found
					offset--;
					if (offset == 0)
					{
						offset = 8;
					}
				}
				else
				{ //sync ok.
					state = 1;
				}
			}
			else if (state == 1)
			{
				if (c != 0x7E)
				{ //data
					state = 2;
				}
			}
			else if (state == 2)
			{
				if (c == 0x03)
				{ //control field
					state = 3;
				}
			}
			else if (state == 3)
			{
				if (c == 0xF0)
				{ //Protocol id shit
					state = 4;
				}
			}
			//printf(" \t%02X %3d\t%c\n", c, c, (state==2) ? (c>>1) : c);
			printf("%c", (state == 2) ? (c >> 1) : c);
			c = 0;
		}
	}

	free(bits);
	free(samples);
	fclose(source);
	return 0;
}