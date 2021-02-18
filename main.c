#include "Includes.h"
#define DIGIT_LEN		28
#define DIGIT_TOTAL	(DIGIT_LEN*DIGIT_LEN)

typedef struct node {
	float (*conv)(float);	//do math shit
	int x,y;			//Visualization coordinates
}node;

typedef struct{
	u8 arr[DIGIT_TOTAL];	//Store the image
}Digit;

u32 linearize(u32 x, u32 y)
{
	return y * DIGIT_LEN + x;		//linearize the sauce
}

void drawDigit(u8 *digit, uint digitNum)
{
	const uint xdigits = gfx.xlen/DIGIT_LEN;
	for(int x = 0; x < DIGIT_LEN; x++){
		for(int y = 0; y  < DIGIT_LEN; y++){
			u8 val = digit[linearize(x, y)];
			setRGB(val, val, val);
			drawPixel(x + (digitNum%xdigits)*DIGIT_LEN,
			          y + (digitNum/xdigits)*DIGIT_LEN);
		}
	}
}

Digit* ReadDigits(const uint numDigits)
{
	File* file = fopen("./Data/train-images-idx3-ubyte", "r");
	if(file == NULL){
		printf("Could not open dataset (./Data/train-images-idx3-ubyte)\n");
		exit(-1);
	}
	i32 magic = 0;
	i32 imgNum = 0;
	i32 imgLenX = 0;
	i32 imgLenY = 0;
	//Get the magic number bus
	for(uint i = 0; i < 4; i++){
		magic = (magic<<8)|(i32)fgetc(file);
	}
	printf("magic = %6d\n", magic);
	//Get image count
	for(uint i = 0; i < 4; i++){
		imgNum = (imgNum<<8)|(i32)fgetc(file);
	}
	printf("imgNum = %6d\n", imgNum);
	//Get column count
	for(uint i = 0; i < 4; i++){
		imgLenY = (imgLenY<<8)|(i32)fgetc(file);
	}
	printf("imgLenY = %6d\n", imgLenY);
	//Get row count
	for(uint i = 0; i < 4; i++){
		imgLenX = (imgLenX<<8)|(i32)fgetc(file);
	}
	printf("imgLenX = %6d\n", imgLenX);

	// Coord digitPos = {0};
	// Coord pixelPos = {0};
	//Allocate the buffer
	Digit *arr = malloc(numDigits * sizeof(Digit));
	memset(arr, 0, numDigits * sizeof(Digit));
	//Copy image sauce to our big-ass buffer
	for(uint digit = 0; digit < numDigits; digit++){
		for(uint pixel = 0; pixel < DIGIT_TOTAL; pixel++){
			const u8 val = fgetc(file);
			arr[digit].arr[pixel] = val;
		}
	}
	return arr;
}

int main(int argc, char const *argv[])
{
	const Length window = {800, 600};
	init(window);
	const uint numDigits = 1000;
	Digit *arr = ReadDigits(numDigits);
	while(1){
		Ticks frameStart = getTicks();
		clear();

		for(uint digit = 0; digit < 100; digit++){
			drawDigit(arr[digit].arr, digit);
		}

		draw();
		events(frameStart + TPF);
	}
	return 0;
}
