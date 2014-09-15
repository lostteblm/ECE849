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

void DoubleThresholding(const ImgGray& input, ImgGray* output)
{
	ImgGray  threshold_hi, threshold_lo;
	int high = 150;
	int low = 79;
	threshold_hi.Reset(input.Width(), input.Height());
	threshold_lo.Reset(input.Width(), input.Height());

	int count = 0;
	for (ImgGray::ConstIterator p = input.Begin(); p != input.End(); p++)
	{
		ImgGray::Iterator p_hi = threshold_hi.Begin();
		if (*p<high)
		{
			*(p_hi + count) = 0;
		}
		else
		{
			*(p_hi + count) = 255;
		}
		count = count + 1;
	}
	Erode3x3(threshold_hi, &threshold_hi);

	count = 0;
	for (ImgGray::ConstIterator p = input.Begin(); p != input.End(); p++)
	{
		ImgGray::Iterator p_low = threshold_lo.Begin();
		if (*p<low)
		{
			*(p_low + count) = 0;
		}
		else
		{
			*(p_low + count) = 255;
		}
		count = count + 1;
	}

	output->Reset(input.Width(), input.Height());
	for (ImgGray::Iterator p = output->Begin(); p != output->End(); p++)
	{
		*p = 0;
	}

	for (int y = 0; y < threshold_lo.Height(); y++)
	{
		for (int x = 0; x < threshold_lo.Width(); x++)
		{
			if (255 == threshold_hi(x, y))
				FloodFill4(threshold_lo, x, y, 255, output);
		}
	}

	Erode3x3(*output, output);
	Dilate3x3(*output, output);
	Dilate3x3(*output, output);

	Figure fig1("High threshold", 0, 400), fig2("Low threshold", 850, 400);
	fig1.Draw(threshold_hi);
	fig2.Draw(threshold_lo);
}

void ConnectedComponent(const ImgGray& input, ImgInt* output)
{
	output->Reset(input.Width(), input.Height());
	ConnectedComponents4(input, output);
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
		ImgGray origin, doublethreshold;
		ImgInt label;

		//check if the file name is correctly inputted
		if (2 == argc)
		{
			//Load the file name
			string filename = argv[1];
			cout << "The file is: " << argv[1] << endl;


			//Load first two file locations
			string location1 = location + filename;

			//check if the first file exist
			while (true)
			{
				if ((_access(location1.c_str(), 0)) != -1)
				{
					//Load the first image
					Load(location1.c_str(), &origin);
				}
				if (origin.Height() > 0 && origin.Width() > 0)
				{
					break;
				}
				cout << "ERROR: cannot open " << filename << ", please open another file (please just type file name)" << endl;
				filename = "";
				getline(cin, filename);
				location1 = location + filename;
			}

			//Perform double thresholding
			DoubleThresholding(origin, &doublethreshold);

			Figure OriF("Original", 0, 0), DTF("Double threshold", 850, 0);
			OriF.Draw(origin);
			DTF.Draw(doublethreshold);
			
			ConnectedComponent(doublethreshold, &label);
			Figure CCF("Connected Component using ", 0, 900);
			CCF.Draw(label);

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

