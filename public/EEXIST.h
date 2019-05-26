#ifndef EEXIST_H
#define EEXIST_H

#include "../public/String.h"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <math.h>

#define OUTPUT			"/home/caleb/Documents/Projects/EEXIST/IO/OUTPUT"
#define SAFE_OUTPUT	 	"/home/caleb/Documents/Projects/EEXIST/IO/SAFE_OUTPUT"
#define INPUT			"/home/caleb/Documents/Projects/EEXIST/IO/INPUT"
#define SAFE_INPUT		"/home/caleb/Documents/Projects/EEXIST/IO/SAFE_INPUT"
#define MODE			"/home/caleb/Documents/Projects/EEXIST/IO/MODE"

#define NEW				"/home/caleb/Documents/Projects/EEXIST/IO/NEW"
#define STAT			"/home/caleb/Documents/Projects/EEXIST/IO/STAT"

#define GEN				"/home/caleb/Documents/Projects/EEXIST/Generation/GEN"

#define BIASES			"/home/caleb/Documents/Projects/EEXIST/Biases/"

#define SIZE				8		// THE XY SIZE OF THE SYSTEM
#define GENERATION_SIZE		10		// NUMBER OF BIASES TO GNERATE EACH GENERATION
#define MUTATION			10		// PERCENT CHANCE OF LOADING RANDOM TUBE VALUE IN
#define KARMA				2		// REIGON SIZE TO TAKE CHEM AND PUT CHEM TO
#define OUTPUT_REIGON		2
#define UPDATES				1
#define GENERATION_SAMPLE	1		// HOW MANY BIASES SURVIVE EACH GNERATION
#define TAKE				.01

struct Vec2
{
	int X = 0;
	int Y = 0;

	Vec2(int Nx, int Ny) { X = Nx; Y = Ny; }
};

struct tube
{
	float SrcX = 0;
	float SrcY = 0;

	float DesX = 0;
	float DesY = 0;

	int TubeType = 0;
};

struct bias
{
	bias(int SX, int SY, int DX, int DY)
	{
		SrcX = SX; SrcY = SY; DesX = DX; DesY = DY;
	}

	bias()
	{
		SrcX = 0; SrcY = 0; DesX = 0; DesY = 0;
	}

	int SrcX = 0;
	int SrcY = 0;

	int DesX = 0;
	int DesY = 0;
};

struct System
{
	struct tube** Tubes;

	struct bias** Biases;
};

enum EMode
{
	Train,
	Run
};

class EEXIST
{

private:

	struct System* Sys;
	struct tube*** Buffer;

	float Score = 0;

public:

	EEXIST();

	// PERFORM SYSTEM UPDATE
	void AttemptUpdate();

	// PERFORM RESET BY GNERATION NEW GENRATION OR USING EIXTING GNERARION BIASES
	void Reset();

	// SAVES THE CURRENT LOADED IN BIAS AND SCORE TO BREEDING BIASES IF ITS BETTER THAN PRESENT BIASES
	void SaveCurrentBias();

	// GET NEXT BIAS TO TEST RETURNS NULL IF NO BIASES LEFT
	void GetBiasToTest(String* BiasFileDir);

	// LOADS IN A GIVEN BIAS FILE INTO THE SYSTEMS BIAS
	void LoadInBias(String* BiasFile);

	// GENERATE NEW GNERATION WITH BEST TWO BIASES FROM PREVIOUS GNERATIONS
	void GenGeneration();

	// GENERATE A BIAS FILE AT DIRECTORY WITH RANDOM BIAS VALUES
	void GenRandomBias(String* nFile);

	// LOAD BIAS INTO SYSTEM AND START NRE SYTEM UPDATE
	void LoadInBias();

	// RESET SYSTEM TUBES TO DEFAULT LEVELS
	void ResetTubes();

	// RETRIEVE TUBE DATA FROM SYSTEM
	System* GetAISystem();

	// CHECK IF A FILE EXIST ON DISK
	bool DoesFileExist(String* FileName);

	// PREAPARE SYSTEM UPDATE
	void PrepareUpdate();
};

/*
 *
class EEXIST
{

private:

	struct System* Sys;
	struct tube*** Buffer;

	float Score = 0;
	int UpdateCount = 0;

	int ResetCount = 0;
	float Scores[10] = { 0 };

public:

	EEXIST();

	//Function DEclarations
	void AttemptUpdate();
	System* GetAISystem();

	//Reset Functions
	void reset();

	void OutStats();
	void OutputBias();
	void SystemSetup();
	void BiasSetup();

	//Reset Helpers
	char* GetEmptyBiasFile();
	void WriteBias(char*);
	void ShiftFiles(int);
	void Breed();
	void ApplyBias();

	//Update System Functions
	void Update();

	bool DoesFileExist(const char* FileName);

};
*/

//Cuda Function DEcalrations
//void UpdateSystem(struct System*, struct tube***);

#endif // EEXIST_H
