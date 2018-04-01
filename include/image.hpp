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

class Image{
		
	// Attributs
	public :
		string imageName;
		int imageChannel ;
		string dateTime ; 
		const string results = "../src/results/" ; 
		const string sources = "../src/sources/"; 

	//functions
	public:
		// constructeur
		// typeImage 0 pour niveau de gris 
		// typeImage 3 pour couleur  
		Image(string imageFile); 
		Image(Mat image); 

		// la fonction qui affiche le menu des different traitement a apporter sur l'image
		static void menu(string filename, int typeread);

		// recupere deux points
		static void get2Points(int* p) ;
		
		// la fonction permet de classer  les resultats dans les dossiers en fonction des fonction appelée 
		static string getFolder(string functionName); 
		
		// la fonction retourne le mininum et le maximum d'intensité d'une image 
		static void minmax(Mat image, int tab[2]); 

		// la fonction qui trace le profile d'intensite
		void imageProfile();

		// la fonction qui trace la ligne sur un image
		void imageLine();
		void imageLine(Point p1, Point p2);

		// la fonction qui trace l'histogramme
		void imageHistogram();
		
		// la fonction de transformation lineaire simple 
		void imageTranSimple(); 

		// la fonction de transformation multi-points 
		void imageTranMultiPoints(int smin, int smax);
		
		// correction gamma de l image 
		void imageGammaCorrection(float fgamma); 
};
