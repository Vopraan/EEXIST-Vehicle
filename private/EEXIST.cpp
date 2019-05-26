#include "../public/EEXIST.h"
#include "../public/Cuda.h"

// CONSTRUCTOR FOR EEXUST SYTEM
EEXIST::EEXIST()
{
	cudaMallocManaged(&Sys, sizeof(struct System));

	// RESERVE SAPCE ON GPU FOR EEXIST ARRAY
	cudaMallocManaged(&Sys->Tubes, sizeof(struct tube*) * SIZE);
	for (int i = 0; i < SIZE; ++i)
	{
		cudaMallocManaged(&Sys->Tubes[i], sizeof(struct tube) * SIZE);
	}

	// RSERVE SAPCE FOR BIASES
	cudaMallocManaged(&Sys->Biases, sizeof(struct bias*) * SIZE);
	for (int i = 0; i < SIZE; ++i)
	{
		cudaMallocManaged(&Sys->Biases[i], sizeof(struct bias) * SIZE);
	}

	// RESERVE SPACE FOR BUFFER
	cudaMallocManaged(&Buffer, sizeof(struct tube**) * SIZE * SIZE);
	for (int i = 0; i < SIZE * SIZE; i++)
	{
		cudaMallocManaged(&Buffer[i], sizeof(struct tube*) * SIZE);

		for (int x = 0; x < SIZE; x++)
		{
			cudaMallocManaged(&Buffer[i][x], sizeof(struct bias) * SIZE);
		}
	}

	for(int i = 0; i < SIZE * SIZE; i++)
	{
		for(int y = 0; y < SIZE; y++)
		{
			for(int x = 0; x < SIZE; x++)
			{
				Buffer[i][x][y].SrcX = 0;
				Buffer[i][x][y].SrcY = 0;
				Buffer[i][x][y].DesX = 0;
				Buffer[i][x][y].DesY = 0;
			}
		}
	}

	Reset();
}

// ATTEMPT TO UPDATE EEXIST SYSTEM
void EEXIST::AttemptUpdate()
{
	// CHECK IF WE NEED TO RESET SYSTEM
	String NewFile(NEW);
	if(DoesFileExist(&NewFile)) { SaveCurrentBias(); Reset(); return; }

	// CHECK IF WE HAVE OUTPUT FROM UNREAL IF SO PROCESS IT
	String OutputFile(OUTPUT);
	if(DoesFileExist(&OutputFile)) { PrepareUpdate(); return; }
}

// PERFORM RESET BY GNERATION NEW GENRATION OR USING EIXTING GNERARION BIASES
void EEXIST::Reset()
{
	// CHECK IF THERE IS STILL A BIAS TO TEST AND IF SO LOAD IT IN
	String BiasTest;
	GetBiasToTest(&BiasTest);

	// IF WE STILL HAVE BIASES TO TEST LEFT LOAD THEM IN AND FINISH RESET
	if(BiasTest.toChar() != NULL) { LoadInBias(&BiasTest); ResetTubes(); remove(NEW); return; }

	// IF THERE ARE NO BIASES TO TEST LEFT WE WANT TO GNERATE NEW ONES
	GenGeneration(); Reset();
}

// SAVES THE CURRENT LOADED IN BIAS AND SCORE TO BREEDING BIASES IF ITS BETTER THAN PRESENT BIASES
void EEXIST::SaveCurrentBias()
{
	int i = 1;

	char SaveTo[256] = "";

	while(i <= GENERATION_SAMPLE)
	{
		char FileDir[256];
		sprintf(FileDir, "%s%s%d", BIASES, "Bias", i);
		FILE* File = fopen(FileDir, "r");
		if(File == NULL) { strcpy(SaveTo, FileDir); break; }

		// GET SCORE FROM FILE
		char CharScore[32];
		fgets(CharScore, 32, File);
		float CheckScore;
		sscanf(CharScore, "%f", &CheckScore);

		fclose(File);

		if(Score > CheckScore) { strcpy(SaveTo, FileDir); break; }
		i++;
	}

	if(SaveTo[0] == '\0') { return; }

	FILE* File = fopen(SaveTo, "w");
	if(File == NULL) { printf("FAILED TO  SAVE BIAS\n"); exit(0); }

	fprintf(File, "%f\n", Score);

	for(int y = 0; y < SIZE; y++)
	{
		for(int x = 0; x < SIZE; x++)
		{
			fprintf(File, "%3d %3d %3d %3d ", Sys->Biases[x][y].SrcX, Sys->Biases[x][y].SrcY, Sys->Biases[x][y].DesX, Sys->Biases[x][y].DesY);
		}
		fprintf(File, "\n");
	}

	fclose(File);
}

// GET NEXT BIAS TO TEST RETURNS NULL IF NO BIASES LEFT
void EEXIST::GetBiasToTest(String* BiasFileDir)
{
	int Check = 10;
	if(BiasFileDir == NULL) { return; }

	while(Check >= 1)
	{
		sprintf(BiasFileDir->toChar(), "%s%d", GEN, Check);

		if(DoesFileExist(BiasFileDir)) { return; }
		
		Check--;
	}

	BiasFileDir->empty();
}

// LOADS IN A GIVEN BIAS FILE INTO THE SYSTEMS BIAS
void EEXIST::LoadInBias(String* BiasFile)
{
	// VERIFY BIAS FILE EXIST
	FILE* File = fopen(BiasFile->toChar(), "r");
	if(File == NULL) {  printf("GIVEN FILE TO LOAD FOR BIAS DOES NOT EXIST : %s\n", BiasFile->toChar()); exit(0); }

	// MOVE DATA FROM FILE TO TUBES
	for(int y = 0; y < SIZE; y++)
	{
		char Line[((16 * SIZE) + 1)];
		fgets(Line, ((16 * SIZE) + 2), File);

		for (int x = 0; x < SIZE; x++)
		{
			Line[((x + 1) * 16) - 1] = '\0';

			sscanf(&Line[x * 16], "%d %d %d %d", &Sys->Biases[x][y].SrcX, &Sys->Biases[x][y].SrcY, &Sys->Biases[x][y].DesX, &Sys->Biases[x][y].DesY);
		}
	}

	fclose(File);
	remove(BiasFile->toChar());
}

// GENERATE NEW GNERATION WITH BEST TWO BIASES FROM PREVIOUS GNERATIONS
void EEXIST::GenGeneration()
{
	String Breed[GENERATION_SAMPLE];

	// GET BIASES TO BREED
	for(int i = 0; i < GENERATION_SAMPLE; i++)
	{
		sprintf(Breed[i].toChar(), "%s%s%d", BIASES, "Bias", i + 1);
		if(!DoesFileExist(&Breed[i])) { GenRandomBias(&Breed[i]); }
	}


	// LOAD IN BIASES TO BREED
	bias Biases	[GENERATION_SAMPLE]	[SIZE][SIZE];
	bias Avg 						[SIZE][SIZE] = { bias(0, 0, 0, 0) };

	for(int i = 0; i < GENERATION_SAMPLE; i++)
	{
		// OPEN BIAS FILE TO CREATE
		FILE* File = fopen(Breed[i].toChar(), "r");
		if(File == NULL) { printf("FAILED TO LOAD BIAS FILE : %d : %s\n",i,  Breed[i].toChar()); exit(0); }

		char EMPTY[32];
		fgets(EMPTY, 32, File);

		// LOOP THROUGH TUBES AND READ IN FILE DATA
		for (int y = 0; y < SIZE; y++)
		{
			String Line(16 * SIZE + 2);
			fgets(Line.toChar(), (16 * SIZE) + 2, File);

			for (int x = 0; x < SIZE; x++)
			{
				Line.toChar()[((x + 1) * 16) - 1] = '\0';
				sscanf(Line.toChar() + (x * 16), "%d %d %d %d", &Biases[i][x][y].SrcX, &Biases[i][x][y].SrcY, &Biases[i][x][y].DesX, &Biases[i][x][y].DesY);

				Avg[x][y].SrcX = Avg[x][y].SrcX + Biases[i][x][y].SrcX;
				Avg[x][y].SrcY = Avg[x][y].SrcY + Biases[i][x][y].SrcY;
				Avg[x][y].DesX = Avg[x][y].DesX + Biases[i][x][y].DesX;
				Avg[x][y].DesY = Avg[x][y].DesY + Biases[i][x][y].DesY;
			}
		}

		fclose(File);
	}

	// LOOP THROUGH AVEARGE VALUES
	for (int y = 0; y < SIZE; ++y)
	{
		for (int x = 0; x < SIZE; ++x)
		{
			// AVERAGE OUT ALL BIASES
			Avg[x][y].SrcX = Avg[x][y].SrcX / GENERATION_SAMPLE;
			Avg[x][y].SrcY = Avg[x][y].SrcY / GENERATION_SAMPLE;
			Avg[x][y].DesX = Avg[x][y].DesX / GENERATION_SAMPLE;
			Avg[x][y].DesY = Avg[x][y].DesY / GENERATION_SAMPLE;
		}
	}

	// LOOP THROUGH ALL BIASES TO GNERATE
	for(int i = 1; i <= GENERATION_SIZE; i++)
	{
		// OPEN FILE TO CREATE
		String FileName;
		sprintf(FileName.toChar(), "%s%d", GEN, i);
		FILE* File = fopen(FileName.toChar(), "w");
		if(File == NULL) { printf("FAILED TO BIAS FILE: %s\n", FileName.toChar()); exit(0); }

		// LOOP THROUGH EACH TUBE AND GNERTE RANDOM BIAS VALUE
		for (int y = 0; y < SIZE; y++)
		{
			for (int x = 0; x < SIZE; x++)
			{
				if(rand() % 100 < MUTATION) { fprintf(File, "%3d %3d %3d %3d ", (rand() % SIZE), (rand() % SIZE), (rand() % SIZE), (rand() % SIZE)); }
				else { fprintf(File, "%3d %3d %3d %3d ", Avg[x][y].SrcX, Avg[x][y].SrcY, Avg[x][y].DesX, Avg[x][y].DesY); } 
			}

			fprintf(File, "\n");
		}

		fclose(File);
	}

	remove(NEW);

	for(int i = 0; i < GENERATION_SAMPLE; i++)
	{
		remove(Breed[i].toChar());
	}
}

// GENERATE A BIAS FILE AT DIRECTORY WITH RANDOM BIAS VALUES
void EEXIST::GenRandomBias(String* nFile)
{
	// OPEN FILE
	FILE* File = fopen(nFile->toChar(), "w");
	if(File == NULL) { printf("FAILED TO MAKE NEW BIAS FILE : %s\n", nFile->toChar()); exit(0); }

	fprintf(File, "0.0\n");

	for (int y = 0; y < SIZE; y++)
	{
		for (int x = 0; x < SIZE; x++)
		{
			fprintf(File, "%3d %3d %3d %3d ", rand() % SIZE, rand() % SIZE, rand() % SIZE, rand() % SIZE);
		}

		fprintf(File, "\n");
	}

	fclose(File);
}

// LOAD BIAS INTO SYSTEM AND START NRE SYTEM UPDATE
void EEXIST::LoadInBias()
{
}

// RESET SYSTEM TUBES TO DEFAULT LEVELS
void EEXIST::ResetTubes()
{
	// INIT ALL TUBES TO DEFALT VALUE
	for(int y = 0; y < SIZE; y++)
	{
		for (int x = 0; x < SIZE; x++)
		{
			Sys->Tubes[x][y].SrcX = SIZE / 2.0f;
			Sys->Tubes[x][y].SrcY = SIZE / 2.0f;
			Sys->Tubes[x][y].DesX = SIZE / 2.0f;
			Sys->Tubes[x][y].DesY = SIZE / 2.0f;
		}
	}
}

// PREAPARE SYSTEM UPDATE
void EEXIST::PrepareUpdate()
{
	// MAKE SURE SAFE FILE EXIST BEFORE UPDATING
	String SafeFile(SAFE_OUTPUT);
	if(!DoesFileExist(&SafeFile)) { return; }

	// OPEN OUTPUT FROM UNREAL
	FILE* OutputFile = fopen(OUTPUT, "r");
	if(OutputFile == NULL) { return; }
	remove(SAFE_OUTPUT);

	// READ AND STORE NUMBER OF INPUT TUBES
	int OutTube = 0;
	char OutTubeString[16];
	fgets(OutTubeString, 16, OutputFile);
	sscanf(OutTubeString, "%d", &OutTube);

	// LOOP THROUGH INPUT TUBES LOAD IN VALUES
	for(int i = 0; i < OutTube; i++)
	{
		float R, G, B, A;

		char ValString[64];
		fgets(ValString, 64, OutputFile);
		sscanf(ValString, "%f %f %f %f", &R, &G, &B, &A);

		Sys->Tubes[i][0].SrcX = R;
		Sys->Tubes[i][0].SrcY = G;
		Sys->Tubes[i][0].DesX = B;
		Sys->Tubes[i][0].DesY = A;
	}

	// LOAD SCORE VALUE
	String OutScoreString(16);
	fgets(OutScoreString.toChar(), 16, OutputFile);
	sscanf(OutScoreString.toChar(), "%f", &Score);

	// CLOSE OUT FILE AND REMOVE IT
	fclose(OutputFile);
	remove(OUTPUT);

	// PERFROM DATA TRANSFERS ON SYSTEM
	StartSysUpdate(Sys, Buffer);

	// PERFORM ADD ON GPU
	StartSysAdd(Sys, Buffer);

	// OUPUT NEW TUBE STATES TI FILE FOR CONTROLLING THE CAR
	FILE* InputFile = fopen(INPUT, "w");
	if(InputFile == NULL) { printf("FAILED CREATE INPUT FILE\n"); exit(0);}

	// OUPUT  ACCLEARTION TO FILE
	float Val1 = 0;
	for (int y = SIZE - 1; y > SIZE - 1 - OUTPUT_REIGON; y--)
	{
		for (int x = 0; x < OUTPUT_REIGON; x++)
		{
			Val1 = Val1 + Sys->Tubes[x][y].SrcX + Sys->Tubes[x][y].SrcY + Sys->Tubes[x][y].DesX + Sys->Tubes[x][y].DesY;
		}
	}
	Val1 = Val1 / (OUTPUT_REIGON * OUTPUT_REIGON * 4);
	Val1 = Val1 / (SIZE -1);
	Val1 = (Val1 * 2) - 1;
	fprintf(InputFile, "%f\n", Val1);

	// OUPUT STEERING TO FILE
	float Val2 = 0;
	for (int y = SIZE - 1; y > SIZE - 1 - OUTPUT_REIGON; y--)
	{
		for (int x = SIZE - 1; x > SIZE - 1 - OUTPUT_REIGON; x--)
		{
			Val2 = Val2 + Sys->Tubes[x][y].SrcX + Sys->Tubes[x][y].SrcY + Sys->Tubes[x][y].DesX + Sys->Tubes[x][y].DesY;
		}
	}
	Val2 = Val2 / (OUTPUT_REIGON * OUTPUT_REIGON * 4);
	Val2 = Val2 / (SIZE -1);
	Val2 = (Val2 * 2) - 1;

	fprintf(InputFile, "%f\n", Val2);
	fclose(InputFile);

	// WRITE OUT SAFE FILE TO LET UNREAL WE ARE DONE MODIFYING THE INPUT FILE
	fclose(fopen(SAFE_INPUT, "w"));

	// REPORT RECORED STEER AND ACCLE VALUES
	printf("ACCEL: %f STEER: %f SCORE: %f\n", Val1, Val2, Score);
}

// CHECK IF A FILE EXIST ON DISK
bool EEXIST::DoesFileExist(String* FileName)
{
	FILE* File = fopen(FileName->toChar(), "r");

	if(File != NULL)
	{
		fclose(File);
		return true;
	}
	else
	{
		return false;
	}
}

System* EEXIST::GetAISystem()
{
	return Sys;
}
