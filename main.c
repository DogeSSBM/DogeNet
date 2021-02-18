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
	//Get image count
	for(uint i = 0; i < 4; i++){
		imgNum = (imgNum<<8)|(i32)fgetc(file);
	}
	//Get column count
	for(uint i = 0; i < 4; i++){
		imgLenY = (imgLenY<<8)|(i32)fgetc(file);
	}
	//Get row count
	for(uint i = 0; i < 4; i++){
		imgLenX = (imgLenX<<8)|(i32)fgetc(file);
	}

	Coord digitPos = {0};
	Coord pixelPos = {0};
	//Allocate the buffer
	Digit *arr = malloc(numDigits * sizeof(Digit));
	memset(arr, 0, numDigits * sizeof(Digit));
	//Copy image sauce to our big-ass buffer
	for(uint digit = 0; digit < numDigits; digit++){
		for(uint pixel = 0; pixel < DIGIT_TOTAL; pixel++){
			const u8 val = fgetc(file);
			arr[digit].arr[pixel] = val;
			//Do the draw
			setRGB(val, val, val);
			drawPixelCoord(
				coordOffset(coordMul(digitPos, DIGIT_LEN),pixelPos)
			);
			if(pixelPos.x++ > DIGIT_LEN){
				pixelPos.x = 0;
				if(pixelPos.y++ > DIGIT_LEN){
					pixelPos.y = 0;
				}
			}
		}
		if(digitPos.x++ > gfx.xlen/DIGIT_LEN){
			digitPos.x = 0;
			if(digitPos.y++ > gfx.ylen/DIGIT_LEN){
				digitPos.y = 0;
			}
		}
		draw();
	}
	return arr;
}

int main(int argc, char const *argv[])
{
	const Length window = {800, 600};
	init(window);

	while(1){
		Ticks frameStart = getTicks();
		clear();



		draw();
		events(frameStart + TPF);
	}
	return 0;
}
