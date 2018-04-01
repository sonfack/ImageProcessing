#include "opencv2/opencv.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/core/core.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include <iostream>
#include <string>
#include <stdio.h>
#include <time.h>

using namespace std;
using namespace cv;

class Visualizer{
		
	// Attributs
	public :


	//functions
	public:

		// la fonction qui affiche le menu des different traitement a apporter sur l'image
		static void menu(string filename, int typeread);
		
		// la fonction qui trace le profile d'intensite
		void imageProfile(int option);

		// la fonction qui trace la ligne sur un image
		void imageLine();
		void imageLine(Point p1, Point p2);

		// la fonction qui trace l'histogramme
		void imageHistogram();
};
