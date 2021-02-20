#include "Includes.h"

//Images are stored here
typedef struct{
	u8 *image;	//Image buffer
	u32 width;
	u32 height;
	u32 size;	//Size of the image in bytes (width * height)
	u32 imgCount; 	//Images in the buffer
}Digits;

//Layer of neurons
typedef struct Layer{
	u32 layerSize;	//Amount of neurons in the layer
	float *neurons;	//Array containing neuron vals
	u32 inputSize;	//Input neuron count
	float *inputBias;//Array of input bias
	float *input;	//Array of input neurons
	float *inputWeight;//Array of input weights
}Layer;

//Network of layers
typedef struct Network{
	u32 networkSize;
	Layer *layers;
}Network;

//Neural net routines
float sigmoid(float x)
{
	return 1.0 / (powf(2.0, -x) + 1);
}

Layer newLayer(u32 layerSize, u32 inputSize, float *inputBuffer)
{
	return (Layer){
		layerSize,
		calloc(layerSize, sizeof(float)),
		inputSize,
		calloc(layerSize, sizeof(float)),
		inputBuffer,
		calloc(inputSize * layerSize, sizeof(float))
	};
}

void deleteLayer(Layer *egg)
{
	free(egg->neurons);
	free(egg->input);
	free(egg->inputWeight);
	free(egg->inputBias);
	memset(egg, '\0', sizeof(Layer));
}

Network *newNetwork(u32 netSize)
{
	Network *net = calloc(1, sizeof(Network));
	*net = (Network){
		netSize,
		calloc(netSize, sizeof(Layer))
	};
	return net;
}

void deleteNetwork(Network *net)
{
	for(int i = 0; i < net->networkSize; i++) {
		deleteLayer(net->layers+i);
	}
	free(net->layers);
	free(net);
}

void stimulateNetwork(Network *net)
{
	for(int l = 0; l < net->networkSize; l++){	//Go through each layer
		for(int n = 0; n < net->layers[l].layerSize; n++){	//Each neuron
			int ni = n * net->layers[l].inputSize;	//Neuron index in weight array
			float wsum = 0;
			for(int i = 0; i < net->layers[l].inputSize; i++){	//And each input
				wsum += net->layers[l].inputWeight[ni + i] //Weighted sum of inputs
					* net->layers[l].input[i];
			}
			wsum += net->layers[l].inputBias[n];	//Add bias to weighted sum
			net->layers[l].neurons[n] = sigmoid(wsum);	//We're done!
		}
	}
}

//Digit routines
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

//Main
int main(int argc, char const *argv[])
{
	const Length window = {800, 600};
	init(window);
	//Read in sample numbers
	const uint numDigits = 112;
	Digits buffer = ReadDigits();
	//Setup neural network
	float *inputBuffer = calloc(buffer.size, sizeof(float));
	Network *net = newNetwork(3);
	net->layers[0] = newLayer(16, buffer.size, inputBuffer);
	net->layers[1] = newLayer(16, net->layers[0].layerSize, net->layers[0].neurons);
	net->layers[2] = newLayer(10, net->layers[1].layerSize, net->layers[1].neurons);
	stimulateNetwork(net);
	//For drawing stuff
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
	deleteNetwork(net);
	return 0;
}
