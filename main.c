#include "Includes.h"
#define DIGIT_LEN		28

typedef struct node {
	float (*conv)(float);	//do math shit
	int x,y;			//Visualization coordinates
}node;

typedef struct{
	u8 image[DIGIT_LEN * DIGIT_LEN];	//Store the image
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

	Digit *arr = malloc(numDigits);
	memset(arr, 0, numDigits);
	for(uint digit = 0; digit < numDigits; digit++){
		for(uint pixel = 0; pixel < (imgLenX * imgLenY); pixel++){
			arr[digit].image[pixel] = fgetc(file);
		}
	}
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
