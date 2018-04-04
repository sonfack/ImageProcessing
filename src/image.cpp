#include <algorithm>   // transform()
#include <cctype>      // toupper(), tolower()
#include <functional>  // ptr_fun()
#include <algorithm>    // std::min_element, std::max_element
#include <math.h>   // pow
#include "../include/image.hpp"


/***************************************************************
 constructeur
****************************************************************/
Image::Image(string fileName){
	
	if(!fileName.empty()){	
		this->imageName = fileName ;
		this->imageChannel = CV_LOAD_IMAGE_UNCHANGED; 
		time_t now = time(0);   // currente date/time
		this->dateTime = ctime(&now);   // convertion en string
	}else{
		cout<<"Probleme lors de la lecture de l'image"<<endl ;
		cout<<"Merci ce verifier le chemin ou le nom de votre image"<<endl;
	}
}

Image::Image(Mat image){
	if(!image.data){
		this->imageChannel = image.channels(); 
		time_t now = time(0);   // currente date/time
		this->dateTime = ctime(&now);   // convertion en string
		this->imageName = __func__+this->dateTime+".jpg" ;
		string file = this->sources+this->imageName; 
		imwrite(file, image); 
	}else{
		cout<<"Image  ici est vide vide"<<endl; 
	}
}


/***************************************************************
 * Cette fonction retourne le min et le max d'intensité 
 * d'une image
 ***************************************************************/
 void Image::minmax(Mat image, int tab[2]){
    int min = 0, max=0;
    for(int c=0;c<image.channels();c++){
        for(int y = 0; y<image.rows;y++){
            for(int x=0; x<image.cols; x++){
                int b=0;
                if(image.channels()==1){
                    Scalar b1 = image.at<uchar>(y, x);
                    b=b1.val[0];
                }else{
                    Scalar b1 = image.at<Vec3b>(y, x);
                    b=b1.val[c];
                }
                if(b<min) min=b;
                if(b>max) max=b;
            }
        }

    }
    tab[0]=min;
    tab[1]=max;
}



/***************************************************************
 * cette fonction classe permet de retourner le nom du dossier 
 * resultat en fonction du mot clé se trouvant dans le nom de 
 * methode appele
 **************************************************************/
 
 string Image::getFolder(string functionName){

	 string pattern[] = {"Profile", "Line", "Histogram","Contrast","Gamma", "TranMultiPoint", "TranSimple"};
	 int patternSize;
	 string folder;  
	 patternSize = sizeof(pattern)/sizeof(string) ; 
	 int i=0;	 
	 size_t found = functionName.find(pattern[i]);
	 while(i<patternSize and !(found != std::string::npos)){
		 i=i+1; 
		 found = functionName.find(pattern[i]);
	 }
	 folder = pattern[i]+"s"; 
	 transform( folder.begin(), folder.end(), folder.begin(), ptr_fun <int, int> ( tolower ) ); 
	 return folder ; 
 } 


/****************************************************************
 * la fonction qui trace le profile d'intensite
 * option 0 pour pour toute l'image
 * option 1 pour porfile d'une droite connaissant deux points
****************************************************************/
void Image::imageProfile(){
	string resultFolder = this->results+Image::getFolder(__func__)+"/"; 
	string file = this->sources+this->imageName; 
	int typeRead  = this->imageChannel; 
	Mat image = imread(file, typeRead);  
	if (!image.data)
    {
      cout << "Image vide "<<endl; 
    }else{
		Scalar tab_color[3]={ Scalar(255,0,0), Scalar(0,255,0), Scalar(0,0,255)};
		int hist_w = 300;
		int hist_h = 300;
		Mat profilImage(hist_w, hist_h, CV_8UC3, Scalar(255, 255, 255));
		double a = (double)profilImage.rows/256;
        double b = (double)profilImage.cols/image.cols;
        int y = (int)image.rows/2;
        Scalar b1,b2;
        for(int j = 0;j<image.channels();j++){
            Mat output(300,300, CV_8UC3, Scalar(255, 255, 255));
            for(int i=0;i<image.cols;i++){
                if(image.channels()==1){
                   b1 = image.at<uchar>(y, i);
                   b2 = image.at<uchar>(y, i+1);
                }else{ 
                   b1 = image.at<Vec3b>(y, i);
                   b2 = image.at<Vec3b>(y, i+1);
                }
                Point p1(i*b,output.rows - b1.val[j]*a);
                Point p2(b*(i+1),output.rows - b2.val[j]*a);
                line(profilImage,p1,p2,tab_color[j],1,8,0);
            }
        }
        namedWindow(__func__, WINDOW_AUTOSIZE );
		imshow(__func__, profilImage); 
		imwrite(resultFolder+__func__+"_"+this->dateTime+".jpg", profilImage); 
		waitKey(0);	
		this->imageLine(Point(0, y), Point(image.cols, y));
		cout<<"Vos resultats se trouvent dans le dossier :"+resultFolder<<endl; 
	}
	
}



/**********************************************************************************
 la fonction qui affiche le menu des different traitement a apporter sur l'image
**********************************************************************************/
void Image::menu(string filename, int typeread){
	Image raw = Image(filename); 
	int choix ;
	cout<<endl<<"####################### MENU ##################### "<<endl<<endl;
	cout<<"Entrez 1 pour le profile d'intensite"<<endl;
	cout<<"Entrez 2 pour histogramme"<<endl;
	cout<<"Entrez 3 pour correction multi-point "<<endl;
	cout<<"Entrez 4 pour correction gamma"<<endl;
	cout<<"Entrez 5 pour correction simple"<<endl;
	cin>>choix ;
	switch(choix){
		case 1: {
                raw.imageProfile();
                break ; 
		}
		case 2: {
				raw.imageHistogram();
                break ;
        	}
		case 3: {
				int smin, smax; 
        		cout<<"Entrez la saturation minimale"<<endl;
        		 cin>>smin ;
            	cout<<"Entrez la saturation maximale"<<endl;
                cin>>smax ;
				raw.imageTranMultiPoints(smin, smax);
                break ;
		}
		case 4:{
				float fgamma; 
				cout<<"Entrez la valeur de votre gamma"<<endl;
                cin>>fgamma ;
				raw.imageGammaCorrection(fgamma); 
		}
		case 5:{
				raw.imageTranSimple(); 
		}
		default:{
			cout<<"Votre choix n est pas bon";
		}
	}
}



/****************************************************************
 la fonction qui trace la ligne sur un image
****************************************************************/
void Image::get2Points(int* points){
	int coordinate ; 
	cout<<"Entrez coordonnees de votre premier point"<<endl;
	cout<<"X : ";
	cin>>coordinate; 
	points[0] = coordinate;
	cout<<"\nY : ";
	cin>>coordinate; 
	points[1] = coordinate;
	cout<<"Entrez coordonnees de votre deuxieme  point"<<endl;
	cout<<"X : ";
	cin>>coordinate ; 
	points[2] = coordinate;
	cout<<"\nY : ";
	cin>>coordinate; 
	points[3] = coordinate ;
}


/****************************************************************
Cette fonction permet de tracer une droite sur une image
***************************************************************/
void Image::imageLine(){
	string resultFolder = this->results+Image::getFolder(__func__)+"/"; 
	string file = this->sources+this->imageName; 
	int typeRead  = this->imageChannel; 
	Mat image = imread(file, typeRead);  
	int points[4] ;
	int coordinate ; 
	const Scalar lcolor(0,0,0);
	cout<<"Entrez coordonnees de votre premier point"<<endl;
	cout<<"X : ";
	cin>>coordinate; 
	points[0] = coordinate;
	cout<<"\nY : ";
	cin>>coordinate; 
	points[1] = coordinate;
	cout<<"Entrez coordonnees de votre deuxieme  point"<<endl;
	cout<<"X : ";
	cin>>coordinate ; 
	points[2] = coordinate;
	cout<<"\nY : ";
	cin>>coordinate; 
	points[3] = coordinate ;
    line(image, Point(points[0],points[1]),Point(points[2],points[3]), lcolor);
    namedWindow( "Droite_points", WINDOW_AUTOSIZE );
	imshow("Droite_points", image);
	imwrite("Droite_points"+this->dateTime+".jpg", image); 
	waitKey(0);
}

void Image::imageLine(Point p1, Point p2){
	string resultFolder = this->results+Image::getFolder(__func__)+"/"; 
	string file = this->sources+this->imageName; 
	int typeRead  = this->imageChannel; 
	Mat image = imread(file, typeRead);  
	if(image.channels() == 1){
		const Scalar lcolor(255,0,0);
		line(image,p1,p2, lcolor);
		namedWindow( "Droite_points", WINDOW_AUTOSIZE );
		imshow("Droite_points", image);
		//imwrite("Droite_points"+this->dateTime+".jpg", image); 
		cout<<"Votre fichier tracée de la droite est : "<<__func__+this->dateTime+".jpg"<<endl; 
		imwrite(resultFolder+__func__+this->dateTime+".jpg", image); 
		waitKey(0);
	}else{
		const Scalar lcolor(0,0,0);
		line(image,p1,p2, lcolor);
		namedWindow( "Droite_points", WINDOW_AUTOSIZE );
		imshow("Droite_points", image);
		//imwrite("Droite_points"+this->dateTime+".jpg", image); 
		cout<<"Votre fichier tracée de la droite est : "<<__func__+this->dateTime+".jpg"<<endl; 
		imwrite(resultFolder+__func__+this->dateTime+".jpg", image); 
		waitKey(0);
	}
    
}


/****************************************************************
 la fonction qui trace l'histogramme
 option 0 histogramme toute l image
 option 1 histogramme droite
****************************************************************/
void Image::imageHistogram(){
	string resultFolder = this->results+Image::getFolder(__func__)+"/"; 
	string file = this->sources+this->imageName; 
	int typeRead  = this->imageChannel; 
	Mat image = imread(file, typeRead);
	int* points ;
	Scalar tab_color[3]={ Scalar(255,0,0), Scalar(0,255,0), Scalar(0,0,255)};
	if(!image.empty()){ // toute image 
		int histogram[256]={0};
		if(image.channels() == 1){ // image niveau de gris
			for (int y = 0; y< image.rows; y++){
				for(int x = 0; x<image.cols;x++){
					Scalar color = image.at<uchar>(y,x);
					histogram[(int)color.val[0]]++;
				}
			}
			int hist_w = 300;
			int hist_h = 300;
			Mat histImage(hist_w, hist_h, CV_8UC1, Scalar(255, 255, 255));
			// 
			int max = histogram[0];
			for(int i = 1; i < 256; i++){
				if(max < histogram[i]){
						max = histogram[i];
				}
			}
			// 
			double a = (double)histImage.rows/max;
			double b = (double)histImage.cols/256;
			for (int j = 0; j < 256; j++){
				Point p1(b*j,histImage.rows-(a*histogram[j]));
				Point p2(b*j,histImage.rows);
				line(histImage,p1,p2,Scalar(0,0,0),1,8,0);
			}
			// 
			namedWindow(__func__, WINDOW_AUTOSIZE );
			imshow(__func__, histImage);
			cout<<resultFolder+__func__+this->dateTime+".jpg"<<endl; 
			imwrite(resultFolder+__func__+this->dateTime+".jpg", histImage); 
			waitKey(0); 
		
		}else if(image.channels() == 3){ // image couleur
			int rgb [3][256] = {0};
			for (int c = 0; c < 3; c++)
			{
				for(int y=0;y<image.rows;y++){
					for(int x=0; x<image.cols;x++){
							Scalar color = image.at<Vec3b>(y,x);
							rgb[c][(int)color.val[c]]++;
						}
					}
			}
			for (int i = 0; i < 3; ++i)
			{
				Mat histImage(300,300, CV_8UC3, Scalar(255, 255, 255));
				int max = rgb[i][0];
				for (int j = 1; j < 256; j++) {
					if (max < rgb[i][j]) {
						max = rgb[i][j];
					}
				}
				double a = (double)histImage.rows/max;
				double b = (double)histImage.cols/256;
				for (int j = 0; j < 256; j++)
				{
					Point p1((b*j)+1,histImage.rows-(a*rgb[i][j]));
					Point p2((b*j)+1,histImage.rows);
					line(histImage,p1,p2,tab_color[i],1,8,0);
				}
				namedWindow(__func__+to_string(i), WINDOW_AUTOSIZE );
				imshow(__func__+to_string(i), histImage); 
				imwrite(resultFolder+__func__+"_"+to_string(i)+"_"+this->dateTime+".jpg", histImage); 
				waitKey(0);
				memset(histogram, 0, sizeof(histogram));
			}
		}
		
	}else{
		cout<<"Image vide"<<endl ;
	}
	
}




/*************************************************************
 * Gamma correction 
 * ***********************************************************/
void Image::imageGammaCorrection(float gamma){
	string resultFolder = this->results+Image::getFolder(__func__)+"/"; 
	string file = this->sources+this->imageName; 
	int typeRead  = this->imageChannel;  
	Mat image = imread(file, typeRead);
	int tab [2];
    Mat new_image = Mat::zeros( image.size(), image.type());
    minmax(image,tab);
    for(int c=0;c<image.channels();c++){
        for(int y = 0; y<image.rows;y++){
            for(int x=0; x<image.cols; x++){
                int b=0;
                if(image.channels()==1){
                    Scalar b1 = image.at<uchar>(y, x);
                    b=b1.val[0];
                    float u = (255*((float)pow(b/255,1/gamma)));
                    if(u>255) u=255;
                    new_image.at<uchar>(y,x)=saturate_cast<uchar>(u);
                }else{
                    int b = image.at<Vec3b>(y,x)[c];
                    float u = (float)255.0*(pow(b/255.0,1.0/gamma));
                    if(u>255) u=255;
                    new_image.at<Vec3b>(y,x)[c]=saturate_cast<uchar>(u);
                }
            }
        }
    }
	Image newRaw = Image(new_image); 
	newRaw.imageHistogram(); 
	namedWindow("Image Originale", 1);
	namedWindow(__func__, 1);
	imshow("Image Originale", image);
	imshow(__func__, new_image);
	imwrite(resultFolder+__func__+"_"+this->dateTime+".jpg", new_image); 
	waitKey(); 
}


void Image :: imageTranMultiPoints(int smin, int smax){
	string resultFolder = this->results+Image::getFolder(__func__)+"/"; 
	string file = this->sources+this->imageName; 
	int typeRead  = this->imageChannel;  
	Mat image = imread(file, typeRead);
    int tab [2];
    Mat new_image = Mat::zeros( image.size(), image.type());
    minmax(image,tab);
    for(int c=0;c<image.channels();c++){
        for(int y = 0; y<image.rows;y++){
            for(int x=0; x<image.cols; x++){
                int b=0;
                if(image.channels()==1){
                    Scalar b1 = image.at<uchar>(y, x);
                    b=b1.val[0];
                    int u = (255*(b-smin))/(smax-smin);
                    if(u<=0) u=0;
                    if(u>=255) u=255;
                    new_image.at<uchar>(y,x)=saturate_cast<uchar>(u);
                }else{
                    int b = image.at<Vec3b>(y,x)[c];
                    int u = (255*(b-smin))/(smax-smin);
                    if(u<=0) u=0;
                    if(u>=255) u=255;
                    new_image.at<Vec3b>(y,x)[c]=saturate_cast<uchar>(u);
                }
            }
        }
    }
    namedWindow("Image Originale", 1);
	namedWindow(__func__, 1);
	imshow("Image Originale", image);
	imshow(__func__, new_image);
	imwrite("multipoint.jpg", new_image); 
	imwrite(resultFolder+__func__+"_"+this->dateTime+".jpg", new_image); 
	waitKey(0); 
}

void Image::imageTranSimple(){
	string resultFolder = this->results+Image::getFolder(__func__)+"/"; 
	string file = this->sources+this->imageName; 
	int typeRead  = this->imageChannel;  
	Mat image = imread(file, typeRead);
    int tab [2];
    Mat new_image = Mat::zeros( image.size(), image.type());
    minmax(image,tab);
    for(int c=0;c<image.channels();c++){
        for(int y = 0; y<image.rows;y++){
            for(int x=0; x<image.cols; x++){
                int b=0;
                if(image.channels()==1){
                    Scalar b1 = image.at<uchar>(y, x);
                    b=b1.val[0];
                    new_image.at<uchar>(y,x)=saturate_cast<uchar>(((b-tab[0])*255)/(tab[1]-tab[0]));
                }else{
                    new_image.at<Vec3b>(y,x)[c]=saturate_cast<uchar>((((image.at<Vec3b>(y,x)[c])-tab[0])*255)/(tab[1]-tab[0]));
                    
                }
            }
        }

    }
    namedWindow("Image Originale", 1);
	namedWindow(__func__, 1);
	imshow("Image Originale", image);
	imshow(__func__, new_image);
	imwrite("transimple.jpg", new_image); 
	imwrite(resultFolder+__func__+"_"+this->dateTime+".jpg", new_image); 
	waitKey(0); 
}
