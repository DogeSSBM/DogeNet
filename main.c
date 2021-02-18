#include "Includes.h"

typedef struct node {
	float (*conv)(float);	//do math shit
	int x,y;		//Visualization coordinates
}node;

typedef struct{
	u8 *image;	//Store the image
	u32 width;
	u32 height;
	u32 size;
	u32 imgCount;
}Digits;

u32 linearize(Digits * buf, u32 x, u32 y, u32 z)
{
	return z * buf->size + y * buf->width + x;
}

void drawDigit(Digits *buffer, uint digitNum, Coord pos)
{
	for(int x = 0; x < buffer->width; x++){
		for(int y = 0; y  < buffer->height; y++){
			u8 val = buffer->image[linearize(buffer, x, y, digitNum)];
			setRGB(val, val, val);
			drawPixel(x + pos.x,
			          y + pos.y);
		}
	}
}

Digits ReadDigits()
{
	File* file = fopen("./Data/train-images-idx3-ubyte", "r");
	if(file == NULL){
		printf("Could not open dataset (./Data/train-images-idx3-ubyte)\n");
		exit(-1);
	}
	i32 magic = 0;
	i32 imgCount = 0;
	i32 imgLenX = 0;
	i32 imgLenY = 0;
	//Get the magic number bus
	for(uint i = 0; i < 4; i++){
		magic = (magic<<8)|(i32)fgetc(file);
	}
	printf("magic = %6d\n", magic);
	//Get image count
	for(uint i = 0; i < 4; i++){
		imgCount = (imgCount<<8)|(i32)fgetc(file);
	}
	printf("imgCount = %6d\n", imgCount);
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
	//Get buffer size
	i32 imgSize = imgLenX * imgLenY;

	//Allocate the buffer
	Digits buffer = {
		calloc(imgCount, imgSize),
		imgLenX,
		imgLenY,
		imgSize,
		imgCount
	};

	//Copy image sauce to our big-ass buffer
	for(uint digit = 0; digit < imgCount; digit++){
		for(uint pixel = 0; pixel < imgSize; pixel++){
			const u8 val = fgetc(file);
			buffer.image[pixel + (digit * imgSize)] = val;
		}
	}
	return buffer;
}

int main(int argc, char const *argv[])
{
	const Length window = {800, 600};
	init(window);
	const uint numDigits = 112;
	Digits buffer = ReadDigits();
	u32 imgCol = gfx.xlen - buffer.width;
	u32 imgRow = gfx.ylen - buffer.height;
	while(1){
		Ticks frameStart = getTicks();
		clear();

		Coord pos = {0};
		for(uint digit = 0; digit < numDigits; digit++){
			drawDigit(&buffer, digit, pos);
			if((pos.x += buffer.width) > imgCol){
				pos.x = 0;
				if((pos.y += buffer.height) > imgRow){
					pos.y = 0;
				}
			}
		}

		draw();
		events(frameStart + TPF);
	}
	return 0;
}
