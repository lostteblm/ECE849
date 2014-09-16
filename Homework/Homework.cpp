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

#define pi 3.1415926
using namespace blepo;
using namespace std;

void DoubleThresholding(const ImgGray& input, ImgGray* output)
{
	ImgGray  threshold_hi, threshold_lo;
	int high = 150;
	int low = 85;
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
	//Dilate3x3(*output, output);
	Erode3x3(*output, output);
	Dilate3x3(*output, output);
	Dilate3x3(*output, output);

	Dilate3x3(*output, output);
	Dilate3x3(*output, output);
	Erode3x3(*output, output);
	Erode3x3(*output, output);
	Figure fig1("High threshold", 0, 400), fig2("Low threshold", 850, 400);
	fig1.Draw(threshold_hi);
	fig2.Draw(threshold_lo);
}

void RegionPro(const ImgGray&origin, const ImgGray& component, ImgBgr* masked)
{
	ImgInt label;
	
	std::vector< ConnectedComponentProperties<ImgGray::Pixel>> reg;  		// pixel type must match input image type
	ConnectedComponents4(component, &label, &reg);
	Figure CCF("Connected Component", 0, 300);
	CCF.Draw(label);
	
	ImgBinary calculation(component.Width(),component.Height());


	for (int i = 1; i < reg.size(); i++)
	{
		cout << "The region properties of region " << i << " is listed below." << endl;

		for (int y = 0; y < calculation.Height(); y++)
		{
			for (int x = 0; x < calculation.Width(); x++)
			{
				if (i == label(x, y))
				{
					calculation(x, y) = 1;
				}
				else
				{
					calculation(x, y) = 0;
				}
			}
		}

		//calculate premeter 
		ImgBinary PremeterBin(component.Width(), component.Height());
		
		Erode3x3(calculation, &PremeterBin);
		Xor(calculation, PremeterBin,&PremeterBin);
		int premeter=0;
		for (ImgBinary::Iterator p = PremeterBin.Begin(); p != PremeterBin.End(); p++)
		{
			if (1==*p)
			{
				premeter += 1;
			}
		}
		
		RegionProperties props;
		RegionProps(calculation, &props);
		float compactness = 4 * pi / props.area / (premeter ^ 2);
		cout << "Regular moments: \n m00 = " << props.m00 << ", m01 =  " << props.m01 << ", m10 = " 
			<< props.m10 << ", m02 = " << props.m02 << ", m20 = " << props.m20 << endl;
		cout << "Centralized moments: \n mu00 = " << props.mu00 << ", mu01 =  " << props.mu01 << ", mu10 = " << props.mu10 << ", mu02 = " << props.mu02 << ", mu20 = " << props.mu20 << endl;
		cout << "Perimeter = " << premeter << ", and area = " << props.area << ". Compactness = " << compactness << endl;
		cout << "Eccentricity = " << props.eccentricity << endl;
		cout << "Direction (clockwise from horizontal): " << props.direction << endl << endl;

		//Centroid point
		CPoint centroid;
		centroid.x = long(props.xc);
		centroid.y = long(props.yc);

		CPoint major_start, major_end, minor_start,minor_end;
		major_start.x = long(props.xc + props.major_axis_x);
		major_start.y = long(props.yc + props.major_axis_y);
		major_end.x = long(props.xc - props.major_axis_x);
		major_end.y = long(props.yc - props.major_axis_y);

		minor_start.x = long(props.xc + props.minor_axis_x);
		minor_start.y = long(props.yc + props.minor_axis_y);
		minor_end.x = long(props.xc - props.minor_axis_x);
		minor_end.y = long(props.yc - props.minor_axis_y);


		//Apple =1 = Red, Banana =2 =Yellow, Grapefruit =3 =Green
		if (props.eccentricity > 0.7&& props.area>1000)	
		{
			//Banana
			Set(masked, PremeterBin, Bgr(255, 0, 255));


			DrawDot(centroid, masked, Bgr(0, 255, 255));

			DrawLine(major_start, major_end, masked, Bgr(0, 0, 255));
			DrawLine(minor_start, minor_end, masked, Bgr(0, 0, 255));

			Erode3x3(calculation, &calculation);
			Erode3x3(calculation, &calculation);
			Erode3x3(calculation, &calculation);
			Erode3x3(calculation, &calculation);


			Dilate3x3(calculation, &calculation);
			Dilate3x3(calculation, &calculation);
			Dilate3x3(calculation, &calculation);
			Dilate3x3(calculation, &calculation);


			ImgBinary body(calculation.Width(),calculation.Height());
			Erode3x3(calculation, &body);
			Xor(calculation, body, &body);
			And(body, PremeterBin, &body);
			Set(masked, body, Bgr(0, 255, 255));

		}
		else if (props.area > 4500)	
		{
			//Grapefruit
			Set(masked, PremeterBin, Bgr(0, 255, 0));
			DrawDot(centroid, masked, Bgr(0, 255, 255));

			DrawLine(major_start, major_end, masked, Bgr(0, 0, 255));
			DrawLine(minor_start, minor_end, masked, Bgr(0, 0, 255));

		}
		else if (props.area>1000)
		{
			//Apple
			Set(masked, PremeterBin, Bgr(0, 0, 255));
			DrawDot(centroid, masked, Bgr(0, 255, 255));
			DrawLine(major_start, major_end, masked, Bgr(0, 0, 255));
			DrawLine(minor_start, minor_end, masked, Bgr(0, 0, 255));

		}
	}


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
			
			ImgBgr Outlined;
			Convert(origin, &Outlined);
			RegionPro(origin, doublethreshold, &Outlined);

			Figure OutlinedF("With fruits detected");
			OutlinedF.Draw(Outlined);

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

