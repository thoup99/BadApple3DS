//Bad Apple by Thoup
#include <citro2d.h>

#include <3ds.h>
#include <string.h>
#include <unistd.h>
#include <stdio.h>
#include <stdio.h> 

#define SCREEN_WIDTH  400
#define SCREEN_HEIGHT 240


int main(int argc, char **argv)
{
	// Init libs
	romfsInit();
	gfxInitDefault();
	C3D_Init(C3D_DEFAULT_CMDBUF_SIZE);
	C2D_Init(C2D_DEFAULT_MAX_OBJECTS);
	C2D_Prepare();
	consoleInit(GFX_BOTTOM, NULL);
	
	// Create screens
	C3D_RenderTarget* top = C2D_CreateScreenTarget(GFX_TOP, GFX_LEFT);
	
	// Create colors
	u32 clrWhite = C2D_Color32(0xFF, 0xFF, 0xFF, 0xFF);
	u32 clrBlack   = C2D_Color32(0x00, 0x00, 0x00, 0xFF);
	
	//Load Files
    FILE *file = fopen("romfs:/bad_apple.tbn", "rb");
	
	//Get file Size
	fseek(file,0,SEEK_END);
	int size = ftell(file);
	fseek(file,0,SEEK_SET);
	
	//Load file into heap memory
	u8* video = new u8[size];
    memset(video, 0, size);
    size_t bytesread = fread(video, sizeof(u8), size, file);
    fclose(file);

	
	int frame_num = 0;
	float target = 33.33f;
	float enter_time = svcGetSystemTick() / (SYSCLOCK_ARM11 / 1000.0f);
	float time_last_frame = 0;
	float time_since_last_frame = 0.0f;
	
	int bytes_read_so_far = 0;
	int bytes_read_last_it = 0;

	// Main loop
	while (aptMainLoop())
	{
		//Scan all the inputs. This should be done once for each frame
		hidScanInput();

		//hidKeysDown returns information about which buttons have been just pressed (and they weren't in the previous frame)
		u32 kDown = hidKeysDown();

		if (kDown & KEY_START) break; // break in order to return to hbmenu
		
		float ms = (svcGetSystemTick() / (SYSCLOCK_ARM11 / 1000.0f)) - enter_time;
		time_since_last_frame += ms - time_last_frame;
		time_last_frame = ms;
		
		if (time_since_last_frame >= target) 
		{
			frame_num++;
			time_since_last_frame -= target;
			bytes_read_so_far += bytes_read_last_it;
		}
		if (frame_num == 6570) break; //Close the program after the video ends
		
		
		printf("\x1b[1;1HBad Apple by Thoup");
		printf("\x1b[2;1HCPU:     %6.2f%%\x1b[K", C3D_GetProcessingTime()*6.0f);
		printf("\x1b[3;1HGPU:     %6.2f%%\x1b[K", C3D_GetDrawingTime()*6.0f);
		printf("\x1b[4;1HMilisec:     %6.2f%\x1b[K", ms);
		printf("\x1b[5;1HFrame:    %6.2f%\x1b[K", frame_num * 1.0f);
		

		// Render the scene
		C3D_FrameBegin(C3D_FRAME_SYNCDRAW);
		C2D_TargetClear(top, clrBlack); // Can be used to set the background color for the whole screen. Clears previous screen with this color
		C2D_SceneBegin(top);
		
		//Draw Frame
		int x = 0;
		int y = 0;
		bool color = 0;
		bytes_read_last_it = 50;
		//"i" starts at 50 since the elements before it are for starting colors
		for (int i = 50; x < SCREEN_WIDTH; i++)
		{
			bytes_read_last_it += 1;
			int adjusted_i = i + bytes_read_so_far;
			if (y == 0){
				//Get starting color for col
				u8 color_byte = video[bytes_read_so_far + (x / 8)];
				int offset = 7 - x % 8;
				color = (color_byte >> offset) & 0x1;
			}
			
			if (color)
				C2D_DrawRectSolid(x, y, 0, 1, video[adjusted_i], clrWhite);
			y += video[adjusted_i];
			color = !color;
			
			
			if (y == SCREEN_HEIGHT){
				x += 1;
				y = 0;
			}	
		}
		//C2D_DrawRectSolid(SCREEN_WIDTH-1, 0, 0, 1, 1, clrWhite);
		gfxFlushBuffers();
        gfxSwapBuffers();

		C3D_FrameEnd(0);
	}
	
	delete[] video;

	// Deinit libs
	C2D_Fini();
	C3D_Fini();
	gfxExit();
	return 0;
}
