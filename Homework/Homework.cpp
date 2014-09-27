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

//Calculate outline perimeter
int calculatePerimeter(const ImgBinary& input)
{
	int perimeter = 0;
	int Nd = 0;	//number of Nd
	int No = 0;	//number of No
	for (int y = 0; y < input.Height(); y++)
	{
		for (int x = 0; x < input.Width(); x++)
		{
			if (1 == input(x, y))
			{	//if a pixel has 4-neighbors
				if (1 == input(x - 1, y) || 1 == input(x + 1, y) || 1 == input(x, y - 1) || 1 == input(x, y + 1))
				{
					No = No + 1;
				}	
					//if a pixel only have diagonal neighbors
				else if (1 == input(x - 1, y + 1) || 1 == input(x + 1, y + 1) || 1 == input(x - 1, y - 1) || 1 == input(x + 1, y - 1))
				{
					Nd = Nd + 1;
				}
			}
		}
	}
	//Kimura's method
	perimeter = sqrt(pow(Nd,2) + pow((Nd + No / 2),2)) + No / 2;
	return perimeter;
}

//double threshold
void DoubleThresholding(const ImgGray& input, ImgGray* output)
{
	ImgGray  threshold_hi, threshold_lo;
	int high = 160;
	int low = 85;
	threshold_hi.Reset(input.Width(), input.Height());
	threshold_lo.Reset(input.Width(), input.Height());
	int count = 0;
	
	//High threshold 
	for (ImgGray::ConstIterator p = input.Begin(); p != input.End(); p++, count++)
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

	}
	Erode3x3(threshold_hi, &threshold_hi);	//remove some noises


	//Low threshold
	count = 0;
	for (ImgGray::ConstIterator p = input.Begin(); p != input.End(); p++, count++)
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
	}

	//display both high thresholded and low thresholded pictures
	Figure fig1("High threshold", 0, 400), fig2("Low threshold", 850, 400);
	fig1.Draw(threshold_hi);
	fig2.Draw(threshold_lo);

	//initialize double-thresholded image
	output->Reset(input.Width(), input.Height());
	for (ImgGray::Iterator p = output->Begin(); p != output->End(); p++)
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
				FloodFill4(threshold_lo, x, y, 255, output);
				FloodFill4(threshold_hi, x, y, 0, &threshold_hi);
			}				
		}
	}

	//remove image noises
	Erode3x3(*output, output);
	Erode3x3(*output, output);
	Dilate3x3(*output, output);
	Dilate3x3(*output, output);

	Dilate3x3(*output, output);
	Dilate3x3Cross(*output, output);
	Erode3x3Cross(*output, output);
	Erode3x3(*output, output);

}

//Region properties and fruit classificiation
void RegionPro(const ImgGray&origin, const ImgGray& component, ImgBgr* masked)
{
	//get different region label
	ImgInt label;
	std::vector< ConnectedComponentProperties<ImgGray::Pixel>> reg;  	
	ConnectedComponents4(component, &label, &reg);
	Figure CCF("Connected Component", 0, 300);
	CCF.Draw(label);
	
	//Calculate region properties
	ImgBinary calculation(component.Width(),component.Height());
	for (int i = 1; i < reg.size(); i++)
	{
		//Find out each region
		int index = 0;
		cout << "The region properties of region " << i << " is listed below." << endl;
		for (ImgInt::ConstIterator p = label.Begin(); p != label.End(); p++)
		{
			if (i == *p)
			{
				calculation(index)=1;
			}
			else
			{
				calculation(index) = 0;
			}
			index++;
		}
	
		//calculate moments
		int m00, m01, m10, m11, m02,m20;	//regular moment
		m00 = m01 = m10 = m11 = m02=m20=0;
		for (int y = 0; y < calculation.Height(); y++)
		{
			for (int x = 0; x < calculation.Width(); x++)
			{
				m00 = m00 + calculation(x,y);
				m01 = m01 + y*calculation(x, y);
				m10 = m10 + x*calculation(x, y);
				m11 = m11 + x*y*calculation(x, y);
				m02 = m02 + pow(y, 2)*calculation(x, y);
				m20 = m20 + pow(x, 2)*calculation(x, y);
			}
		}

		int Xc = m10 / m00;
		int Yc = m01 / m00;

		//calculate centric moment and other properties, referenced DEMO
		RegionProperties props;
		RegionProps(calculation, &props);


		//calculate premeter 
		ImgBinary PerimeterBin(component.Width(), component.Height());
		Erode3x3Cross(calculation, &PerimeterBin);		//I choose Erode3x3Cross because it is more accurate than Erode3x3
		Xor(calculation, PerimeterBin, &PerimeterBin);
		int perimeter = calculatePerimeter(PerimeterBin);	//Call calculatePerimeter function
		

		//calculate compactness
		int compactness = 4 * pi / m00 / (perimeter ^ 2);

		//display all properties required
		cout << "Regular moments: \n m00 = " << props.m00 << ", m01 =  " << props.m01 << ", m10 = " << props.m10 
			<< ", m11 = " << props.m11 << ", m02 = " << props.m02 << ", m20 = " << props.m20 << endl;
		cout << "Centralized moments: \n mu00 = " << props.mu00 << ", mu01 =  " << props.mu01 << ", mu10 = " << props.mu10 
			<< ", mu11 = " << props.mu11 << ", mu02 = " << props.mu02 << ", mu20 = " << props.mu20 << endl;
		cout << "Perimeter = " << perimeter << ", and area = " << m00 << ". Compactness = " << compactness << endl;
		cout << "Eccentricity = " << props.eccentricity << endl;
		cout << "Direction (clockwise from horizontal): " << props.direction << endl << endl;


		//Draw perpendicular lines from the centroid point
		CPoint centroid(Xc,Yc);
		DrawDot(centroid, masked, Bgr(0, 255, 255));

		//Major axis
		CPoint major_start, major_end, minor_start,minor_end;
		major_start.x = long(Xc + props.major_axis_x);
		major_start.y = long(Yc + props.major_axis_y);
		major_end.x = long(Xc - props.major_axis_x);
		major_end.y = long(Yc - props.major_axis_y);

		//minor axis
		minor_start.x = long(Xc + props.minor_axis_x);
		minor_start.y = long(Yc + props.minor_axis_y);
		minor_end.x = long(Xc - props.minor_axis_x);
		minor_end.y = long(Yc - props.minor_axis_y);

		//draw two lines
		DrawLine(major_start, major_end, masked, Bgr(0, 0, 255));
		DrawLine(minor_start, minor_end, masked, Bgr(0, 0, 255));


		//classify each shape to fruits
		//Apple =1 = Red, Banana =2 =Yellow, Grapefruit =3 =Green
		if (props.eccentricity > 0.7&& props.area>1000)	//Banana is very long
		{
			Set(masked, PerimeterBin, Bgr(255, 0, 255));	//paint outline to yellow

			//find banana stem and paint the outline to magneta
			Erode3x3(calculation, &calculation);
			Erode3x3(calculation, &calculation);
			Erode3x3(calculation, &calculation);
			Erode3x3(calculation, &calculation);
			Erode3x3Cross(calculation, &calculation);
			Dilate3x3Cross(calculation, &calculation);
			Dilate3x3(calculation, &calculation);
			Dilate3x3(calculation, &calculation);
			Dilate3x3(calculation, &calculation);
			Dilate3x3(calculation, &calculation);

			//highlight the stem
			ImgBinary body(calculation.Width(),calculation.Height());
			Erode3x3(calculation, &body);
			Xor(calculation, body, &body);
			And(body, PerimeterBin, &body);
			Set(masked, body, Bgr(0, 255, 255));
		}
		else if (props.area > 4500)	// Grapefruit is bigger than apple
		{
			//Grapefruit
			Set(masked, PerimeterBin, Bgr(0, 255, 0));	//paint outline to green
		}
		else if (props.area>1000)	//Apple is smaller 
		{
			//Apple
			Set(masked, PerimeterBin, Bgr(0, 0, 255));	//paint outline to red
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

			
			Figure OriF("Original", 0, 0);
			OriF.Draw(origin);
			//Perform double thresholding by call DoubleThresholding function
			DoubleThresholding(origin, &doublethreshold);

			Figure DTF("Double threshold", 850, 0);
			DTF.Draw(doublethreshold);
			
			//Calculate image and find fruits by call RegionPro function
			ImgBgr Outlined;
			Convert(origin, &Outlined);
			RegionPro(origin, doublethreshold, &Outlined);
			Figure OutlinedF("Fruits classified");
			OutlinedF.Draw(Outlined);

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

