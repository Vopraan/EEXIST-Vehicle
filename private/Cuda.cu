#include "../public/Cuda.h"

void StartSysUpdate(struct System* SystemPtr, struct tube*** BufferPtr)
{
	//printf("WHAT\n");

	UpdateSystems /*<<< SIZE, SIZE >>>*/ (SystemPtr, BufferPtr);
	//cudaDeviceSynchronize();
}


// PERFROM DATA TRNASFERS FOR UPDATE
//__global__
void UpdateSystems(struct System* SystemPtr, struct tube*** BufferPtr)
{
	for(int y = 0; y < SIZE; y++)
	{
		for(int x = 0; x < SIZE; x++)
		{
			// STORE TAKE AND PUT CHEMC COORDS
			int TakeSrcX = ((int)SystemPtr->Tubes[x][y].SrcX + SystemPtr->Biases[x][y].SrcX) % SIZE;
			int TakeSrcY = ((int)SystemPtr->Tubes[x][y].SrcY + SystemPtr->Biases[x][y].SrcY) % SIZE;
			int TakeDesX = ((int)SystemPtr->Tubes[x][y].DesX + SystemPtr->Biases[x][y].DesX) % SIZE;
			int TakeDesY = ((int)SystemPtr->Tubes[x][y].DesY + SystemPtr->Biases[x][y].DesY) % SIZE;

			// LOOP THROUGH KARMA TUBES FOR CENTERAL TUBE TRANSFER
			for(int Ky = -KARMA; Ky <= KARMA; Ky++)
			{
				for (int Kx = -KARMA; Kx <= KARMA; Kx++)
				{
					float KarmDist = sqrtf(powf(Kx, 2) + pow(Ky, 2));
					if(KarmDist > KARMA) { continue; }

					// ADD AKRMA OFFSET TO CENTERAL TUBE
					int KSrcX = TakeSrcX + Kx;
					int KScrY = TakeSrcY + Ky;
					int KDesX = TakeDesX + Kx;
					int KDesY = TakeDesY + Ky;

					// FILTER OUT TRNASFERS FROM FIRST TWO ROWS THE FIRST ROW IS RESERVED AS INPUT FOR THE SYSTEM
					if(KScrY == 0 || KDesY == 0) { continue; }

					// CALC HOW MUCH CHEM TO TAKE
					float Take = TAKE * (1 - (KarmDist / KARMA));

					// MAKE SURE TANSFER IS IN RANGE OF TUBES
					KSrcX = abs(KSrcX % SIZE);
					KScrY = abs(KScrY % SIZE);
					KDesX = abs(KDesX % SIZE);
					KDesY = abs(KDesY % SIZE);

					if(SystemPtr->Tubes[KSrcX][KScrY].SrcX > Take && (SystemPtr->Tubes[KDesX][KDesY].SrcX + Take) < SIZE)
					{
						SystemPtr->Tubes[KSrcX][KScrY].SrcX = SystemPtr->Tubes[KSrcX][KScrY].SrcX - Take;
						SystemPtr->Tubes[KSrcX][KScrY].SrcY = SystemPtr->Tubes[KSrcX][KScrY].SrcY - Take;
						SystemPtr->Tubes[KSrcX][KScrY].DesX = SystemPtr->Tubes[KSrcX][KScrY].DesX - Take;
						SystemPtr->Tubes[KSrcX][KScrY].DesY = SystemPtr->Tubes[KSrcX][KScrY].DesY - Take;

						SystemPtr->Tubes[KDesX][KDesY].SrcX = SystemPtr->Tubes[KDesX][KDesY].SrcX + Take;
						SystemPtr->Tubes[KDesX][KDesY].SrcY = SystemPtr->Tubes[KDesX][KDesY].SrcY + Take;
						SystemPtr->Tubes[KDesX][KDesY].DesX = SystemPtr->Tubes[KDesX][KDesY].DesX + Take;
						SystemPtr->Tubes[KDesX][KDesY].DesY = SystemPtr->Tubes[KDesX][KDesY].DesY + Take;
					}
				}
			}
		}
	}


	/*
	int x = blockIdx.x;
	int y = threadIdx.x;

	// STORE TAKE AND PUT CHEMC COORDS
	int TakeSrcX = ((int)SystemPtr->Tubes[x][y].SrcX + SystemPtr->Biases[x][y].SrcX) % SIZE;
	int TakeSrcY = ((int)SystemPtr->Tubes[x][y].SrcY + SystemPtr->Biases[x][y].SrcY) % SIZE;
	int TakeDesX = ((int)SystemPtr->Tubes[x][y].DesX + SystemPtr->Biases[x][y].DesX) % SIZE;
	int TakeDesY = ((int)SystemPtr->Tubes[x][y].DesY + SystemPtr->Biases[x][y].DesY) % SIZE;

	// LOOP THROUGH KARMA TUBES FOR CENTERAL TUBE TRANSFER
	for(int Ky = -KARMA; Ky <= KARMA; Ky++)
	{
		for (int Kx = -KARMA; Kx <= KARMA; Kx++)
		{
			float KarmDist = sqrtf(powf(Kx, 2) + pow(Ky, 2));
			if(KarmDist > KARMA) { continue; }

			// ADD AKRMA OFFSET TO CENTERAL TUBE
			int KSrcX = TakeSrcX + Kx;
			int KScrY = TakeSrcY + Ky;
			int KDesX = TakeDesX + Kx;
			int KDesY = TakeDesY + Ky;

			// FILTER OUT TRNASFERS FROM FIRST TWO ROWS THE FIRST ROW IS RESERVED AS INPUT FOR THE SYSTEM
			if(KScrY == 0 || KDesY == 0) { continue; }

			// CALC HOW MUCH CHEM TO TAKE
			float Take = TAKE * (1 - (KarmDist / KARMA));

			// MAKE SURE TANSFER IS IN RANGE OF TUBES
			KSrcX = abs(KSrcX % SIZE);
			KScrY = abs(KScrY % SIZE);
			KDesX = abs(KDesX % SIZE);
			KDesY = abs(KDesY % SIZE);

			int i = (y * SIZE) + x;

			BufferPtr[i][KSrcX][KScrY].SrcX = -Take;
			BufferPtr[i][KSrcX][KScrY].SrcY = -Take;
			BufferPtr[i][KSrcX][KScrY].DesX = -Take;
			BufferPtr[i][KSrcX][KScrY].DesY = -Take;

			BufferPtr[i][KDesX][KDesY].SrcX =  Take;
			BufferPtr[i][KDesX][KDesY].SrcY =  Take;
			BufferPtr[i][KDesX][KDesY].DesX =  Take;
			BufferPtr[i][KDesX][KDesY].DesY =  Take;
		}
	}
	*/
}

void StartSysAdd(struct System* SystemPtr, struct tube*** BufferPtr)
{
	//AddSystems <<< SIZE, SIZE >>> (SystemPtr, BufferPtr);
	//cudaDeviceSynchronize();
}

// ADD TOGETHER GIVEN MATRICIES
__global__
void AddSystems(struct System* SystemPtr, struct tube*** BufferPtr)
{
	int x = blockIdx.x;
	int y = threadIdx.x;

	if(y == 0) {return;}

	//Add MAtricies
	for(int i = 0; i < SIZE * SIZE; i++)
	{
		SystemPtr->Tubes[x][y].SrcX = SystemPtr->Tubes[x][y].SrcX + BufferPtr[i][x][y].SrcX;
		SystemPtr->Tubes[x][y].SrcY = SystemPtr->Tubes[x][y].SrcY + BufferPtr[i][x][y].SrcY;
		SystemPtr->Tubes[x][y].DesX = SystemPtr->Tubes[x][y].DesX + BufferPtr[i][x][y].DesX;
		SystemPtr->Tubes[x][y].DesY = SystemPtr->Tubes[x][y].DesY + BufferPtr[i][x][y].DesY;
	}

	//Calmp Tube VAlues
	if(SystemPtr->Tubes[x][y].SrcX < 0) 		{ SystemPtr->Tubes[x][y].SrcX = 0; }
	if(SystemPtr->Tubes[x][y].SrcX > SIZE - 1) 	{ SystemPtr->Tubes[x][y].SrcX = SIZE - 1; }

	if(SystemPtr->Tubes[x][y].SrcY < 0) 		{ SystemPtr->Tubes[x][y].SrcY = 0; }
	if(SystemPtr->Tubes[x][y].SrcY > SIZE - 1) 	{ SystemPtr->Tubes[x][y].SrcY = SIZE - 1; }

	if(SystemPtr->Tubes[x][y].DesX < 0) 		{ SystemPtr->Tubes[x][y].DesX = 0; }
	if(SystemPtr->Tubes[x][y].DesX > SIZE - 1) 	{ SystemPtr->Tubes[x][y].DesX = SIZE - 1; }

	if(SystemPtr->Tubes[x][y].DesY < 0) 		{ SystemPtr->Tubes[x][y].DesY = 0; }
	if(SystemPtr->Tubes[x][y].DesY > SIZE - 1) 	{ SystemPtr->Tubes[x][y].DesY = SIZE - 1; }
}