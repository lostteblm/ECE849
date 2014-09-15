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
		ImgGray img, threshold_hi, threshold_lo;

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

			threshold_hi.Reset(img.Width(), img.Height());
			threshold_lo.Reset(img.Width(), img.Height());



			int count = 0;
			for (ImgGray::ConstIterator p = img.Begin(); p != img.End(); p++)
			{
				ImgGray::Iterator p_hi = threshold_hi.Begin();
				if (*p<105)
				{
					*(p_hi+count) = 0;
				}
				else
				{
					*(p_hi + count) = 255;
				}
				count = count + 1;
			}
			
			Erode3x3(threshold_hi, &threshold_hi);
/*			for (int y = 0; y < threshold_hi.Height(); y++)
			{
				for (int x = 0; x < threshold_hi.Width(); x++)
				{

				}
			}
	*/		//Erode3x3(threshold_hi, &threshold_hi);


			count = 0;
			for (ImgGray::ConstIterator p = img.Begin(); p != img.End(); p++)
			{
				ImgGray::Iterator p_low = threshold_lo.Begin();
				if (*p<70)
				{
					*(p_low + count) = 0;
				}
				else
				{
					*(p_low + count) = 255;
				}
				count = count + 1;
			}
			ImgGray doublethreshold(0, 0);
			doublethreshold.Reset(img.Width(), img.Height());
			for (ImgGray::Iterator p = doublethreshold.Begin(); p != doublethreshold.End(); p++)
			{
				*p = 0;	
			}
			FloodFill4(threshold_lo, 92, 66, 255, &doublethreshold);

			Figure fig1,fig2,fig3,fig4;
			fig1.Draw(img);
			fig2.Draw(threshold_hi);
			fig3.Draw(threshold_lo);
			fig4.Draw(doublethreshold);







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

