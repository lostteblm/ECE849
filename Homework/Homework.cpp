// Heyu Liu- C15622730
//Assignment-1

#include <afxwin.h>  // necessary for MFC to work properly
#include <iostream>
#include "Homework.h"
#include "../../src/blepo.h"
#include <io.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

using namespace blepo;
using namespace std;


//Square the image function
void SquareRegion(const ImgBgr& Img1,ImgGray& Img2)
{
	int width = Img1.Width();
	int height = Img1.Height();
	Img2.Reset(width, height);
	

	//Modify images that are larger than 100*100
	if (width >= 100 && height >= 100)
	{
		for (ImgGray::Iterator p = Img2.Begin(); p != Img2.End(); p++)
		{
			*p = 0;
		}
		for (int y = height / 2 - 50; y < height / 2 + 50; y++)
		{
			for (int x = width / 2 - 50; x < width / 2 + 50; x++)
			{
				Img2(x, y) = 255;
			}
		}
	}
	//modify images that are smaller than 100*100
	else
	{
		for (ImgGray::Iterator p = Img2.Begin(); p != Img2.End(); p++)
		{
			*p = 255;
		}
	}
}


//Mask image function
ImgBgr maskimage(const ImgGray& source,ImgBgr target)
{
	int count = 0;
	ImgBgr::Iterator pmask = target.Begin();

	//mask target image
	for (ImgGray::ConstIterator p = source.Begin(); p != source.End(); p++)
	{

		if (0 == *p)
		{
			*(pmask+count) = Bgr(0, 0, 0);
		}
		count = count + 1;
	}
	return target;
}


//Main function
int main(int argc, const char* argv[], const char* envp[])
{
    // Initialize MFC and return if failure
    HMODULE hModule = ::GetModuleHandle(NULL);
    if (hModule == NULL || !AfxWinInit(hModule, NULL, ::GetCommandLine(), 0))
    {
        printf("Fatal Error: MFC initialization failed (hModule = %x)\n", hModule);
        return 1;
    }

	try
	{
		//Initialization filenames
		string location = "../../images/";
		ImgBgr img(0, 0);

		//check if three file names are correctly inputted
		if (2 == argc)
		{
			//Load three file names
			string filename = argv[1];
			cout << "Three files are: " << argv[1] << endl;


			//Load first two file locations
			string location1 = location + filename;

			//check if the first file exist
			while (true)
			{
				if ((_access(location1.c_str(), 0)) != -1)
				{
					//Load the first image
					Load(location1.c_str(), &img);
				}
				if (img.Height() > 0 && img.Width() > 0)
				{
					break;
				}
				cout << "ERROR: cannot open " << filename << ", please open another file" << endl;
				filename = "";
				getline(cin, filename);
				location1 = location + filename;
			}
		}
		else
		{
			//Failed to input three file names
			cout << "Please press Ctrl+C to quit program and reinput three files" << endl;
		}
		cout << "Mask process finished, you can press Ctrl+C to quit the program" << endl;
			EventLoop();
		}

	catch (const Exception& e)
	{
		e.Display();
	}
    return 0;
}
