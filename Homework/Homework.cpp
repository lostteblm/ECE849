// Homework.cpp : Defines the entry point for the console application.
//

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


void SquareRegion(const ImgBgr& Img1,ImgGray& Img2)
{
	int width = Img1.Width();
	int height = Img1.Height();
	Img2.Reset(width, height);
	

	//Modify image that are larger than 100*100
	if (width > 100 && height > 100)
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
	//modify images that are small than 100*100
	else
	{
		for (ImgGray::Iterator p = Img2.Begin(); p != Img2.End(); p++)
		{
			*p = 255;
		}
	}
}

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
		//Initialize filenames
		string location = "../../images/";
		ImgBgr img1(0, 0);
		ImgGray img2(0, 0);
		ImgGray img3(0, 0);


		//check if three file names are correctly inputted
		if (4 == argc)
		{
			//Load three file names
			string filename1 = argv[1];
			string filename2 = argv[2];
			string filename3 = argv[3];
			cout << "Three files are: " << argv[1] << ", " << argv[2] << ", " << argv[3] << endl;

			//check if the second file has a jpg extension
			while (true)
			{
				int startp = filename2.find(".");
				string extension2 = filename2.substr(startp+1);
				if ("jpeg" == extension2 || "jpg" == extension2 || "JPG" == extension2 || "JPEG" == extension2)
				{
					cout << "ERROR: The extension of location2 cannot be JPEG, please type another extension" << endl;
				}
				else
				{
					break;
				}
				getline(cin, filename2);
			}

			//Load first two file locations
			string location1 = location + filename1;
			string location2 = location + filename2;

			//check if the first file exist
			while (true)
			{
				if ((_access(location1.c_str(), 0)) != -1)
				{
					//Load the first image
					Load(location1.c_str(), &img1);
					break;
				}
				cout << "ERROR: filename1 doesn't exist, please open another file" << endl;
				filename1 = "";
				getline(cin, filename1);
				location1 = location + filename1;
			}

			//modify the first image and save the second image
			SquareRegion(img1, img2);
			Figure fig1, fig2;
			fig1.Draw(img1);
			fig2.Draw(img2);
			Save(img2, location2.c_str(), "bmp");

			//check if the second file is successfully saved
			if ((_access(location2.c_str(), 0)) != -1)
			{
				
				//check if the third file exist
				while (true)
				{
					string location3 = location + filename3;
					if ((_access(location3.c_str(), 0)) != -1)
					{
						Load(location3.c_str(), &img3);
						break;
					}
					else
					{
						cout << "ERROR: filename3 doesn't exist, please open another file" << endl;
						filename3 = "";
						getline(cin, filename3);
						location3 = location + filename3;
					}
				}

				//mask the third image
				while (true)
				{ 
					//check if the third image has same size with the first image
					if (img3.Width() == img1.Width() && img3.Height() == img1.Height())
					{
						ImgBgr maskedfigure = maskimage(img3, img1);
						Figure fig3;
						fig3.Draw(img3);
						Figure fig4;
						fig4.Draw(maskedfigure);
						break;
					}
					else
					{
						cout << "The sizes of file 3 and file 1 are different,please select another file" << endl;
						filename3 = "";
						getline(cin, filename3);
						string location3 = location + filename3;
					}
				}

			}
				else
				{
					//Second file failed to save
					cout << "ERROR: location2 failed to save" << endl;
				}

			}
		else
		{
			//Failed to input three file names
			cout << "Please press Ctrl+C to quit program and reinput three files" << endl;
		}
			EventLoop();
		}

	catch (const Exception& e)
	{
		e.Display();
	}
    return 0;
}
