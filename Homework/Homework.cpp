// Heyu Liu- C15622730
//Assignment-3

#include <afxwin.h>  // necessary for MFC to work properly
#include <iostream>
#include "Homework.h"
#include "../../src/blepo.h"
#include <io.h>
#include <math.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#define pi 3.1415926
using namespace blepo;
using namespace std;

//create a Gaussian Kernel
void GaussianKernel(const double& sigma, vector<double>* output)
{
	double f = 2.5;
	int mu = round(f*sigma - 0.5);
	int w = 2 * mu + 1;
	vector<double>g;
	double sum = 0;
	for (int i = 0; i < w; i++)
	{
		g.push_back( exp(-(i - mu)*(i - mu) / (2 * sigma*sigma)));
		sum += g[i];
	}
	for (int i = 0; i < w; i++)
	{
		g[i] = g[i] / sum;
		cout << g[i] << ", ";
	}
	cout << endl;
	*output = g;
}

//double threshold
void GaussianDKernel(const double& sigma, vector<double>* GDKernel)
{
	double f = 2.5;
	int mu = round(f*sigma - 0.5);
	int w = 2 * mu + 1;
	vector<double>gd;
	double sum = 0;
	for (int i = 0; i < w; i++)
	{
		gd.push_back((i-mu)*exp(-(i - mu)*(i - mu) / (2 * sigma*sigma)));
		sum += gd[i]*i;
	}
	for (int i = 0; i < w; i++)
	{
		gd[i] = gd[i] / sum;
		cout << gd[i] << ", ";
	}
	cout << endl;
	*GDKernel = gd;
}

//Separability convolution
void gradient(const ImgGray& origin, const double& sigma, ImgGray* Gx, ImgGray* Gy)
{
	double f = 2.5;
	int mu = round(f*sigma - 0.5);
	int w = 2 * mu + 1;
	vector<double>GKernel, GDKernel;
	GaussianKernel(sigma, &GKernel);
	GaussianDKernel(sigma, &GDKernel);
	Gx->Reset(origin.Width(), origin.Height());
	Gy->Reset(origin.Width(), origin.Height());
	ImgFloat temperary(origin.Width(), origin.Height());

	//Gx gradient
	for (int y = 0; y < origin.Height(); y++)
	{
		for (int x = mu; x < origin.Width() - mu; x++)
		{
			double val = 0;
			for (int i = 0; i < w; i++)
			{
				val += GDKernel[i] * origin(x + mu - i, y);
			}
			temperary(x, y) = val;
		}
	}

	for (int y = mu; y < origin.Height()-mu; y++)
	{
		for (int x = 0; x < origin.Width(); x++)
		{
			double val = 0;
			for (int i = 0; i < w; i++)
			{
				val += GKernel[i] * temperary(x, y + mu - i);
			}
			(*Gx)(x,y) = val;
		}
	}

	//Gy gradient

	for (int y = 0; y < origin.Height(); y++)
	{
		for (int x = mu; x < origin.Width() - mu; x++)
		{
			double val = 0;
			for (int i = 0; i < w; i++)
			{
				val += GKernel[i] * origin(x + mu - i, y);
			}
			temperary(x, y) = val;
		}
	}

	for (int y = mu; y < origin.Height() - mu; y++)
	{
		for (int x = 0; x < origin.Width(); x++)
		{
			double val = 0;
			for (int i = 0; i < w; i++)
			{
				val += GDKernel[i] * temperary(x, y + mu - i);
			}
			(*Gy)(x, y) = val;
		}
	}
}


//Non-maximal suppression
void suppression(const ImgGray& Gx, const ImgGray& Gy, ImgGray* suppressed)
{
	ImgFloat magnitude(Gx.Width(), Gx.Height());
	ImgFloat phase(Gx.Width(), Gx.Height());
	ImgFloat NonMax(Gx.Width(), Gx.Height());

	for (int y = 0; y < Gx.Height(); y++)
	{
		for (int x = 0; x < Gx.Width(); x++)
		{
			//find the max point around the target point
			magnitude(x, y) = max(Gx(x,y),Gy(x,y));
			phase(x, y) = atan2(Gy(x,y), Gx(x,y));
		}
	}


	//Non-maximal suppression
	for (int y = 1; y < Gx.Height()-1; y++)
	{
		for (int x = 1; x < Gx.Width()-1; x++)
		{
			float val_left = 0, val_right = 0;
			if (phase(x, y)> -pi / 8 && phase(x, y) < pi / 8)
			{
				val_left = magnitude(x - 1, y);
				val_right = magnitude(x + 1, y);
			}
			else if (phase(x, y) >= pi / 8 && phase(x, y) < 3 * pi / 8)
			{
				val_left = magnitude(x - 1, y - 1);
				val_right = magnitude(x + 1, y + 1);
			}
			else if (phase(x, y) >= 3 * pi / 8 && phase(x, y) < 5 * pi / 8)
			{
				val_left = magnitude(x, y - 1);
				val_right = magnitude(x, y + 1);
			}
			else
			{
				val_left = magnitude(x + 1, y - 1);
				val_right = magnitude(x - 1, y + 1);
			}
			if (magnitude(x, y) < val_left || magnitude(x, y) < val_right)
				NonMax(x, y) = 0;
			else
				NonMax(x, y) = magnitude(x, y);
		}
	}

	Convert(NonMax, suppressed);
	Figure phaseF("Phase floating image"), magnitudeF("Magnitude floating image") , spre("Suppressed gray image");
	phaseF.Draw(phase);
	magnitudeF.Draw(magnitude);
	spre.Draw(*suppressed);
}

//Double thresholding
void DoubleThreshold(const ImgGray& origin, ImgGray* thresholded)
{

	float High_p = 0.05;
	ImgGray sortImage;
	sortImage = origin;
	ImgBinary doublethresholding(origin.Width(), origin.Height());
	sort(sortImage.Begin(), sortImage.End());
	ImgGray::ConstIterator p = sortImage.Begin();

	int HighThreshold;
	int size = origin.Width() * origin.Height();
	int High_value = size*(1-High_p);
	HighThreshold = *(p+High_value);
	int LowThreshold = HighThreshold / 5;

	ImgGray threshold_hi(origin.Width(), origin.Height()), threshold_low(origin.Width(), origin.Height());
	int count = 0;
	//High threshold 
	for (ImgGray::ConstIterator p = origin.Begin(); p != origin.End(); p++, count++)
	{
		ImgGray::Iterator p_hi = threshold_hi.Begin();
		if (*p<HighThreshold)
		{
			*(p_hi + count) = 0;
		}
		else
		{
			*(p_hi + count) = 255;
		}
	}
	//Low threshold
	count = 0;
	for (ImgGray::ConstIterator p = origin.Begin(); p != origin.End(); p++, count++)
	{

		ImgGray::Iterator p_low = threshold_low.Begin();
		if (*p<LowThreshold)
		{
			*(p_low + count) = 0;
		}
		else
		{
			*(p_low + count) = 255;
		}
	}

	//display both high thresholded and low thresholded pictures
	Figure fig1("High threshold", 0, 400), fig2("Low threshold", 850, 400), spre;
	spre.Draw(origin);
	fig1.Draw(threshold_hi);
	fig2.Draw(threshold_low);

	thresholded->Reset(origin.Width(), origin.Height());
	for (ImgGray::Iterator p = thresholded->Begin(); p != thresholded->End(); p++)
	{
		*p = 0;
	}
	//double-threshold
	for (int y = 0; y < threshold_hi.Height(); y++)
	{
		for (int x = 0; x < threshold_hi.Width(); x++)
		{
			if (255 == threshold_hi(x, y))
			{
				FloodFill4(threshold_low, x, y, 255, thresholded);
				FloodFill4(threshold_hi, x, y, 0, &threshold_hi);
			}
		}
	}
	Figure thresholdedF("thresholded image");
	thresholdedF.Draw(*thresholded);
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


		//check if the file name is correctly inputted
		if (3 == argc)
		{
			//Load the file name
			string filename = argv[1];
			float sigma = stof(argv[2]);
			cout << "The file is: " << argv[1] << endl;

			//Load the file location
			string location1 = location + filename;

			//check if the file exists
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
			ImgGray Gx, Gy, suppressedImage;
			ImgGray doublethresholdedImage;
			Figure OriF("Original", 0, 0);
			OriF.Draw(origin);
			
			gradient(origin, sigma, &Gx, &Gy);
			Figure GradientedF("Gx"), DGradientedF("Gy");
			GradientedF.Draw(Gx);
			DGradientedF.Draw(Gy);

			suppression(Gx, Gy, &suppressedImage);

			DoubleThreshold(suppressedImage, &doublethresholdedImage);

		}
		else
		{
			//Failed to input three file names
			cout << "Please press Ctrl+C to quit program and reinput another file" << endl;
		}
		cout << "Image processing finished, you can press Ctrl+C to quit the program" << endl;
		EventLoop();
	}

	catch (const Exception& e)
	{
		e.Display();
	}
	return 0;
}

