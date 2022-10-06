#include <iostream>
#include <thread>
#include <vector>


#define OLC_PGE_APPLICATION
#include "olcPixelGameEngine.h"

using namespace std;
// Override base class with your custom functionality
class PixelTetris : public olc::PixelGameEngine
{
public:
	PixelTetris()
	{
		// Name your application
		sAppName = "Pixel Tetris";
	}

		int nScreenWidth = 100;			// Console Screen Size X (columns)
		int nScreenHeight = 300;			// Console Screen Size Y (rows)
		wstring tetromino[7];
		int nFieldWidth = 120;
		int nFieldHeight = 180;
		unsigned char* pField = nullptr;
		// Game Logic Stuff
		bool bGameOver = false;

		int nCurrentPiece = 0;
		int nCurrentRotation = 0;
		int nCurrentX = ScreenWidth() / 2;
		int nCurrentY = 0;
		int nSpeed = 20;
		int nSpeedCount = 0;
		bool bForceDown = false;
		bool bRotateHold = true;
		int nPieceCount = 0;
		int nScore = 0;
		vector<int> vLines;


		//unsigned char* pField = nullptr;

public:
	int Rotate(int px, int py, int r)
	{
		int pi = 0;
		switch (r % 4)
		{
			case 0: // 0 degrees			// 0  1  2  3
				pi = py * 4 + px;			// 4  5  6  7
				break;						// 8  9 10 11
											//12 13 14 15

			case 1: // 90 degrees			//12  8  4  0
				pi = 12 + py - (px * 4);	//13  9  5  1
				break;						//14 10  6  2
											//15 11  7  3

			case 2: // 180 degrees			//15 14 13 12
				pi = 15 - (py * 4) - px;	//11 10  9  8
				break;						// 7  6  5  4
											// 3  2  1  0

			case 3: // 270 degrees			// 3  7 11 15
				pi = 3 - py + (px * 4);		// 2  6 10 14
				break;						// 1  5  9 13
		}									// 0  4  8 12

		return pi;
	}
	bool DoesPieceFit(int nTetromino, int nRotation, int nPosX, int nPosY)
	{
		for (int px = 0; px < 4; px++)
			for (int py = 0; py < 4; py++)
			{
				// Get index into piece
				int pi = Rotate(px, py, nRotation);

				// Get index into field
				int fi = (nPosY + py) * nFieldWidth + (nPosX + px);

				if (nPosX + px >= 0 && nPosX + px < nFieldWidth)
				{
					if (nPosY + py >= 0 && nPosY + py < nFieldHeight)
					{
						// In Bounds so do collision check
						if (tetromino[nTetromino][pi] != L'.' && pField[fi] != 0)
							// Wait for some time when the piece hits bottom so that the player has some time to move the piece around.
	//						this_thread::sleep_for(300ms);
							return false; // fail on first hit
					}
				}
			}
		return true;
	}
	bool OnUserCreate() override
	{
		// Called once at the start, so create things here

		tetromino[0].append(L"..X...X...X...X."); // Tetronimos 4x4
		tetromino[1].append(L"..X..XX...X.....");
		tetromino[2].append(L".....XX..XX.....");
		tetromino[3].append(L"..X..XX..X......");
		tetromino[4].append(L".X...XX...X.....");
		tetromino[5].append(L".X...X...XX.....");
		tetromino[6].append(L"..X...X..XX.....");
		

		for (int y = 0; y < nFieldHeight; y++)
			Draw(0, y, olc::Pixel(255, 255, 0));
		for (int y = 0; y < nFieldHeight; y++)
			Draw(nFieldWidth - 1, y, olc::Pixel(255, 255, 0));
		for (int x = 0; x < nFieldWidth; x++)
			Draw(x, 0, olc::Pixel(255, 255, 0));
		for (int x = 0; x < nFieldWidth; x++)
			Draw(x, nFieldHeight - 1, olc::Pixel(255, 255, 0));

				return true;
	}

	bool OnUserUpdate(float fElapsedTime) override
	{
		// GAME TIMING =========================
		this_thread::sleep_for(50ms);
		nSpeedCount++;
		bForceDown = (nSpeedCount == nSpeed);

		// GAME LOGIC ==========================
		
		nCurrentX += (GetKey(olc::Key::RIGHT).bReleased && DoesPieceFit(nCurrentPiece, nCurrentRotation, nCurrentX + 1, nCurrentY)) ? 1 : 0;
		nCurrentX -= (GetKey(olc::Key::LEFT).bReleased && DoesPieceFit(nCurrentPiece, nCurrentRotation, nCurrentX - 1, nCurrentY)) ? 1 : 0;
		nCurrentY += (GetKey(olc::Key::DOWN).bReleased && DoesPieceFit(nCurrentPiece, nCurrentRotation, nCurrentX, nCurrentY + 1)) ? 1 : 0;
		nCurrentRotation += (bRotateHold && DoesPieceFit(nCurrentPiece, nCurrentRotation + 1, nCurrentX, nCurrentY)) ? 1 : 0;
		

		if (bForceDown)
		{
			// Update difficulty every 50 pieces
			nSpeedCount = 0;
			nPieceCount++;
			if (nPieceCount % 50 == 0)
				if (nSpeed >= 10) nSpeed--;

			if (DoesPieceFit(nCurrentPiece, nCurrentRotation, nCurrentX, nCurrentY + 1))
				nCurrentY++; // It can, so do it!
			else
			{
				// Lock the current piece in the field
				for (int px = 0; px < 4; px++)
					for (int py = 0; py < 4; py++)
						if (tetromino[nCurrentPiece][Rotate(px, py, nCurrentRotation)] != L'.')
							pField[(nCurrentY + py) * nFieldWidth + (nCurrentX + px)] = nCurrentPiece + 1;
				// Check have we got any lines
				for (int py = 0; py < 4; py++)
					if (nCurrentY + py < nFieldHeight - 1)
					{
						bool bLine = true;
						for (int px = 1; px < nFieldWidth - 1; px++)
							bLine &= (pField[(nCurrentY + py) * nFieldWidth + px]) != 0;

						if (bLine)
						{
							// Remove Line, set to =
							for (int px = 1; px < nFieldWidth - 1; px++)
								pField[(nCurrentY + py) * nFieldWidth + px] = 8;
							vLines.push_back(nCurrentY + py);
						}
					}

				nScore += 25;
				if (!vLines.empty())	nScore += (1 << vLines.size()) * 100;
				// Choose the next piece
				nCurrentX = nFieldWidth / 2;
				nCurrentY = 0;
				nCurrentRotation = 0;
				nCurrentPiece = rand() % 7;


				// If piece does not fit

				bGameOver = !DoesPieceFit(nCurrentPiece, nCurrentRotation, nCurrentX, nCurrentY);
			}


			if (nSpeedCount / nSpeed > 3)
			{
				nSpeedCount = 0;
			}
		}

		// Draw current piece
		for (int px = 0; px < 4; px++)
			for (int py = 0; py < 4; py++)
				if (tetromino[nCurrentPiece][Rotate(px, py, nCurrentRotation)] != L'.')
					Draw(0, y, olc::Pixel(255, 255, 0));
					screen[(nCurrentY + py + 2) * nScreenWidth + (nCurrentX + px + 2)] = nCurrentPiece + 65;

		//Clear(olc::DARK_GREEN);
		return true;
	}
};

int main()
{
	PixelTetris demo;
	if (demo.Construct(150, 300, 4, 4))
		demo.Start();
	return 0; 
}
