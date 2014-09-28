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
void GaussianKernel(const float& sigma, vector<double>* output)
{
	float f = 2.5;
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
void GaussianDKernel(const float& sigma, vector<double>* GDKernel)
{
	float f = 2.5;
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
void gradient(const ImgGray& origin, const float& sigma, ImgGray* GaussianGradiented, ImgGray* DerivativeGaussian)
{
	float f = 2.5;
	int mu = round(f*sigma - 0.5);
	int w = 2 * mu + 1;
	vector<double>GKernel, GDKernel;
	GaussianKernel(sigma, &GKernel);
	GaussianDKernel(sigma, &GDKernel);

	ImgGray temperary(origin.Width(), origin.Height());
	ImgGray Gradiented(origin.Width(), origin.Height());
	ImgGray DGradiented(origin.Width(), origin.Height());

	//Gaussian gradient
	for (int y = 0; y < origin.Height(); y++)
	{
		for (int x = mu; x < origin.Width() - mu; x++)
		{
			float val = 0;
			for (int i = 0; i < w; i++)
			{
				val += GKernel[i] * origin(x + mu - i, y);
			}
			temperary(x, y) = val;
		}
	}

	for (int y = mu; y < origin.Height()-mu; y++)
	{
		for (int x = 0; x < origin.Width(); x++)
		{
			float val = 0;
			for (int i = 0; i < w; i++)
			{
				val += GKernel[i] * temperary(x, y + mu - i);
			}
			Gradiented(x, y) = val;
		}
	}
	*GaussianGradiented = Gradiented;

	//Deriviative Gaussian gradient

	for (int y = 0; y < origin.Height(); y++)
	{
		for (int x = mu; x < origin.Width() - mu; x++)
		{
			float val = 0;
			for (int i = 0; i < w; i++)
			{
				val += GDKernel[i] * origin(x + mu - i, y);
			}
			temperary(x, y) = val;
		}
	}

	for (int y = mu; y < origin.Height() - mu; y++)
	{
		for (int x = 0; x < origin.Width(); x++)
		{
			float val = 0;
			for (int i = 0; i < w; i++)
			{
				val += GDKernel[i] * temperary(x, y + mu - i);
			}
			DGradiented(x, y) = val;
		}
	}

	*DerivativeGaussian = DGradiented;
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
			ImgGray gradiented, derivativegradiented;
			
			Figure OriF("Original", 0, 0);
			OriF.Draw(origin);
			
			gradient(origin, sigma, &gradiented, &derivativegradiented);
			Figure GradientedF("Gaussian gradiented"), DGradientedF("Derivative Gaussian gradiented");
			GradientedF.Draw(gradiented);
			DGradientedF.Draw(derivativegradiented);


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

