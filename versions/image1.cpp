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


/***************************************************************
 * cette fonction classe permet de retourner le nom du dossier 
 * resultat en fonction du mot clé se trouvant dans le nom de 
 * methode appele
 **************************************************************/
 
 string Image::getFolder(string functionName){

	 string pattern[] = {"Profile", "Line", "Histogram","Contrast"};
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
void Image::imageProfile(int option){
	string resultFolder = this->results+Image::getFolder(__func__)+"/"; 
	string file = this->sources+this->imageName; 
	int typeRead  = this->imageChannel; 
	Mat image = imread(file, typeRead);  
	if (!image.data)
    {
        std::cout << "Aucune image chargée";
		exit(0); 
    }
	switch(option){
		case 0 : { //toute l image 
			const int nb_value= image.rows*image.cols; 
			int histogram[nb_value]={0};
			if(image.channels() == 1){
				for(int i= 0; i< image.cols; i++){
					for(int j = 0; j<image.rows; j++){
						histogram[(int)image.at<uchar>(Point(i,j))]++; 
					}
				}
				// draw the histograms
				int hist_w = image.cols; 
				int hist_h = image.rows;
				int bin_w = cvRound((double) hist_w/256);
				Mat histImage(hist_w, hist_h, CV_8UC1, Scalar(255, 255, 255));
 
				// find the maximum intensity element from histogram
				int max = histogram[0];
				for(int i = 1; i < nb_value; i++){
					if(max < histogram[i]){
							max = histogram[i];
					}
				}
 
				// normalize the histogram between 0 and histImage.rows
 
				for(int i = 0; i < nb_value; i++){
					histogram[i] = ((double)histogram[i]/max)*histImage.rows;
				}
 
				for(int i = 0; i < nb_value; i++)
				{
					if(i>0){
							line(histImage, Point(bin_w*(i-1), hist_h-histogram[i-1]),
										Point(bin_w*(i), hist_h - histogram[i]),
								Scalar(0,0,0), 1, 8, 0);
					}else if(i==0){
							line(histImage, Point(bin_w*(0), hist_h-histogram[0]),
										Point(bin_w*(i), hist_h - histogram[i]),
								Scalar(0,0,0), 1, 8, 0);
					}	
				}
				//Affichage
				namedWindow(__func__, WINDOW_AUTOSIZE );
				imshow(__func__, histImage);
				cout<<"Votre fichier resultat est : "<<__func__+this->dateTime+".jpg"<<endl; 
				imwrite(resultFolder+__func__+this->dateTime+".jpg", histImage); 
				waitKey(0); 
				
			}else if(image.channels() == 3){
				Scalar tab_color[3]={ Scalar(255,0,0), Scalar(0,255,0), Scalar(0,0,255)};
				Mat image_rgv[3] ;
				split(image, image_rgv); 
				for(int rgv=0; rgv<3; rgv++){
						
					for(int i= 0; i< image_rgv[rgv].cols; i++){
						for(int j = 0; j< image_rgv[rgv].rows; j++){
							histogram[(int)image_rgv[rgv].at<uchar>(Point(i,j))]++; 
						}
					}

					// draw the histograms
					int hist_w = 300; 
					int hist_h = 300;
					int bin_w = cvRound((double) hist_w/256);
					Mat histImage(hist_w, hist_h, CV_8UC3, Scalar(255,255,255));
	 
					// find the maximum intensity element from histogram
					int max = histogram[0];
					for(int i = 1; i < nb_value; i++){
						if(max < histogram[i]){
								max = histogram[i];
						}
					}
    				// normalize the histogram between 0 and histImage.rows
 
    		 		for(int i = 0; i < nb_value; i++){
        				histogram[i] = ((double)histogram[i]/max)*histImage.rows;
    				}
 
					for(int i = 0; i < nb_value; i++){
						if(i>0){
								line(histImage, Point(bin_w*(i-1), hist_h-histogram[i-1]),
											Point(bin_w*(i), hist_h - histogram[i]),
									tab_color[rgv], 1, 8, 0);
						}else if(i==0){
								line(histImage, Point(bin_w*(0), hist_h-histogram[0]),
											Point(bin_w*(i), hist_h - histogram[i]),
									tab_color[rgv], 1, 8, 0);
						}	
					}
					namedWindow(__func__+to_string(rgv), WINDOW_AUTOSIZE );
					imshow(__func__+to_string(rgv), histImage); 
					imwrite(resultFolder+__func__+"_"+to_string(rgv)+"_"+this->dateTime+".jpg", histImage); 
					cout<<"Votre fichier "<< "No "+to_string(rgv)<<"resultat est : "<<__func__+this->dateTime+".jpg"<<endl; 
					waitKey(0);
					memset(histogram, 0, sizeof(histogram)); 
				}

			}
			cout<<"Vos resultats se trouvent dans le dossier :"+resultFolder<<endl; 
			break ;
		}
		case 1 : { // une droite de l image
			int* points = new int() ;
			get2Points(points);
			
			if(image.channels() == 1){
				LineIterator it(image, Point(points[0],points[1]),Point(points[2],points[3]), 8);
				int nb_value = it.count ; 
				int histogram[nb_value]={0};
				
				for(int i= 0; i< nb_value; i++,++it){
					histogram[(int)image.at<uchar>(it.pos())]++; 
				}
				// draw the histograms
				int hist_w = 300; 
				int hist_h = 300;
				int bin_w = cvRound((double) hist_w/256);
				Mat histImage(hist_w, hist_h, CV_8UC1, Scalar(255, 255, 255));
 
				// find the maximum intensity element from histogram
				int max = histogram[0];
				for(int i = 1; i < nb_value; i++){
					if(max < histogram[i]){
							max = histogram[i];
					}
				}
 
				// normalize the histogram between 0 and histImage.rows
 
				for(int i = 0; i < nb_value; i++){
					histogram[i] = ((double)histogram[i]/max)*histImage.rows;
				}
 
				for(int i = 0; i < nb_value; i++)
				{
					if(i>0){
							line(histImage, Point(bin_w*(i-1), hist_h-histogram[i-1]),
										Point(bin_w*(i), hist_h - histogram[i]),
								Scalar(0,0,0), 1, 8, 0);
					}else if(i==0){
							line(histImage, Point(bin_w*(0), hist_h-histogram[0]),
										Point(bin_w*(i), hist_h - histogram[i]),
								Scalar(0,0,0), 1, 8, 0);
					}	
				}
				//Affichage
				namedWindow(__func__, WINDOW_AUTOSIZE );
				imshow(__func__, histImage);
				cout<<"Votre fichier resultat est : "<<__func__+this->dateTime+".jpg"<<endl; 
				imwrite(resultFolder+__func__+this->dateTime+".jpg", histImage); 
				waitKey(0); 
				this->imageLine(Point(points[0],points[1]),Point(points[2],points[3])); 
			}else if(image.channels() == 3){
				Scalar tab_color[3]={ Scalar(255,0,0), Scalar(0,255,0), Scalar(0,0,255)};
				Mat image_rgv[3] ;
				split(image, image_rgv); 
				LineIterator it(image, Point(points[0],points[1]),Point(points[2],points[3]), 8);
				int nb_value = it.count ; 
				int histogram[nb_value]={0};
				for(int rgv=0; rgv<3; rgv++){
					LineIterator it(image_rgv[rgv], Point(points[0],points[1]),Point(points[2],points[3]), 8);
					int nb_value = it.count ; 
					int histogram[nb_value]={0};
							
					for(int i= 0; i< nb_value; i++,++it){
						histogram[(int)image_rgv[rgv].at<uchar>(it.pos())]++; 
					}
					// draw the histograms
					int hist_w = 300; 
					int hist_h = 300;
					int bin_w = cvRound((double) hist_w/256);
					Mat histImage(hist_w, hist_h, CV_8UC3, Scalar(255,255,255));
	 
					// find the maximum intensity element from histogram
					int max = histogram[0];
					for(int i = 1; i < nb_value; i++){
						if(max < histogram[i]){
								max = histogram[i];
						}
					}
    				// normalize the histogram between 0 and histImage.rows
 
    		 		for(int i = 0; i < nb_value; i++){
        				histogram[i] = ((double)histogram[i]/max)*histImage.rows;
    				}
 
					for(int i = 0; i < nb_value; i++){
						if(i>0){
								line(histImage, Point(bin_w*(i-1), hist_h-histogram[i-1]),
											Point(bin_w*(i), hist_h - histogram[i]),
									tab_color[rgv], 1, 8, 0);
						}else if(i==0){
								line(histImage, Point(bin_w*(0), hist_h-histogram[0]),
											Point(bin_w*(i), hist_h - histogram[i]),
									tab_color[rgv], 1, 8, 0);
						}	
					}
					namedWindow(__func__+to_string(rgv), WINDOW_AUTOSIZE );
					imshow(__func__+to_string(rgv), histImage); 
					imwrite(resultFolder+__func__+"_"+to_string(rgv)+"_"+this->dateTime+".jpg", histImage); 
					cout<<"Votre fichier "<< "No "+to_string(rgv)<<"resultat est : "<<__func__+this->dateTime+".jpg"<<endl; 
					waitKey(0);
					//memset(histogram, 0, sizeof(histogram)); 
				}
			}
			cout<<"Vos resultats se trouvent dans le dossier :"+resultFolder<<endl; 
			break ;
		}
		default : {
			cout<<"Votre option n est pas parmi la liste"<<endl;
			cout<<"Veillez changer d'option"<<endl;
		}
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
	cout<<"Entrez 3 pour contraste"<<endl;
	cin>>choix ;
	switch(choix){
		case 1: {
        		cout<<"Quel type de profile voulez vous"<<endl;
                cout<<"Entrez 0 pour toute l'image"<<endl;
                cout<<"Entrez 1 pour une droite"<<endl;
                cin>>choix ;
                raw.imageProfile(choix);
                break ; 
		}
		case 2: {
        		cout<<"Quel type de histogramme voulez vous"<<endl;
            	cout<<"Entrez 0 pour toute l'image"<<endl;
            	cout<<"Entrez 1 pour une droite"<<endl;
                cin>>choix ;
				raw.imageHistogram(choix);
                break ;
        	}
		case 3: {
        		cout<<"Pour definir le contraste"<<endl;
        		cout<<"Entrez 0 pour variation MAX-MIX"<<endl;
            	cout<<"Entrez 1 pour ECART-TYPE"<<endl;
                cin>>choix ;
				raw.imageContrast(choix);
                break ;
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
void Image::imageHistogram(int option){
	string resultFolder = this->results+Image::getFolder(__func__)+"/"; 
	string file = this->sources+this->imageName; 
	int typeRead  = this->imageChannel; 
	Mat image = imread(file, typeRead);
	int* points ;
	Scalar tab_color[3]={ Scalar(255,0,0), Scalar(0,255,0), Scalar(0,0,255)};
	if(!image.empty()){
		
		if(option == 1){ // droite 
			get2Points(points); 
			LineIterator it(image, Point(points[0],points[1]),Point(points[2],points[3]), 8);
			int nb_value = it.count ;
			int histogram[nb_value]={0};
			if(image.channels() == 1){ // image niveau de gris - droite 
					
				for(int i= 0; i< it.count; i++,++it){
					histogram[(int)image.at<uchar>(it.pos())]++;
				}
				// Fenetre de histogramme
				int hist_w = 300;
				int hist_h = 300;
				int bin_w = cvRound((double) hist_w/256);
				Mat histImage(hist_w, hist_h, CV_8UC1, Scalar(255, 255, 255));
				// Recherche l intensite max de histogramme
				int max = histogram[0];
				for(int i = 1; i < nb_value; i++){
					if(max < histogram[i]){
							max = histogram[i];
					}
				}
				// Normaliser histogramme entre  0 and histImage.rows
				for(int i = 0; i < nb_value; i++){
					histogram[i] = ((double)histogram[i]/max)*histImage.rows;
				}
				// Desine histogramme
				for(int i = 0; i < nb_value; i++){
					line(histImage, Point(bin_w*(i), hist_h),
								Point(bin_w*(i), hist_h - histogram[i]),
						Scalar(0,0,0), 1, 8, 0);
				}
				
				// Affiche histogram
				namedWindow(__func__, WINDOW_AUTOSIZE );
				imshow(__func__, histImage);
				cout<<resultFolder+__func__+this->dateTime+".jpg"<<endl; 
				imwrite(resultFolder+__func__+this->dateTime+".jpg", histImage); 
				waitKey(0); 
				this->imageLine(Point(points[0],points[1]),Point(points[2],points[3])); 
			}else if(image.channels() == 3){ // image couleur-droite
				Mat image_rgv[3] ;
				split(image, image_rgv); 
		        for(int rgv=0; rgv<3; rgv++){
					for(int i= 0; i< it.count; i++,++it){
						histogram[(int)image_rgv[rgv].at<uchar>(it.pos())]++; 
					}

					// draw the histograms
					int hist_w = 300; 
					int hist_h = 300;
					int bin_w = cvRound((double) hist_w/256);
					Mat histImage(hist_w, hist_h, CV_8UC3, Scalar(255,255,255));
	 
						// find the maximum intensity element from histogram
					int max = histogram[0];
					for(int i = 1; i < nb_value; i++){
						if(max < histogram[i]){
								max = histogram[i];
						}
					}
					// normalize the histogram between 0 and histImage.rows
 
					for(int i = 0; i < nb_value; i++){
						histogram[i] = ((double)histogram[i]/max)*histImage.rows;
					}
 
 
					// draw the intensity line for histogram
					for(int i = 0; i < nb_value; i++)
					{
						line(histImage, Point(bin_w*(i), hist_h),
									Point(bin_w*(i), hist_h - histogram[i]),
							tab_color[rgv], 1, 8, 0);
					}
				
    				namedWindow(__func__+to_string(rgv), WINDOW_AUTOSIZE );
					imshow(__func__+to_string(rgv), histImage); 
					imwrite(resultFolder+__func__+"_"+to_string(rgv)+"_"+this->dateTime+".jpg", histImage); 
					waitKey(0);
					memset(histogram, 0, sizeof(histogram));
				}
				//namedWindow( "Profile_intensite", WINDOW_AUTOSIZE );
				//imshow("Profile_intensite", histImage);  
				//imwrite("Profile_intensite"+this->dateTime+".jpg", histImage);
				//waitKey(0);
				this->imageLine(Point(points[0],points[1]),Point(points[2],points[3])); 
			}
		}else if(option == 0){ // toute image 
			const int totalpixel = image.total(); 
			int histogram[totalpixel]={0};
			if(image.channels() == 1){ // image niveau de gris
				for (int y = 0; y < image.rows; y++){
					for (int x = 0; x < image.cols; x++)
						histogram[(int) image.at<uchar>(y, x)]++;
				}
				int hist_w = 300;
				int hist_h = 300;
				int bin_w = cvRound((double) hist_w/256);
				Mat histImage(hist_w, hist_h, CV_8UC1, Scalar(255, 255, 255));
				// 
    			int max = histogram[0];
    			for(int i = 1; i < totalpixel; i++){
        			if(max < histogram[i]){
            				max = histogram[i];
        			}
    			}
    			//
    			for(int i = 0; i < totalpixel; i++){
        			histogram[i] = ((double)histogram[i]/max)*histImage.rows;
    			}
    			// 
    			for(int i = 0; i < totalpixel; i++)
    			{
        			line(histImage, Point(bin_w*(i), hist_h),
                              	Point(bin_w*(i), hist_h - histogram[i]),
             			Scalar(0,0,0), 1, 8, 0);
    			}
    			// 
    			namedWindow(__func__, WINDOW_AUTOSIZE );
				imshow(__func__, histImage);
				cout<<resultFolder+__func__+this->dateTime+".jpg"<<endl; 
				imwrite(resultFolder+__func__+this->dateTime+".jpg", histImage); 
				waitKey(0); 
			
			}else if(image.channels() == 3){ // image couleur
				
				Mat image_rgv[3] ;
				split(image, image_rgv); 
		        for(int rgv=0; rgv<3; rgv++){
					
					for (int y = 0; y < image.rows; y++){
						for (int x = 0; x < image.cols; x++)
							histogram[(int) image_rgv[rgv].at<uchar>(y, x)]++;
					}
					int hist_w = 300;
					int hist_h = 300;
					int bin_w = cvRound((double) hist_w/256);
					Mat histImage(hist_w, hist_h, CV_8UC1, Scalar(255, 255, 255));
					// 
					int max = histogram[0];
					for(int i = 1; i < totalpixel; i++){
						if(max < histogram[i]){
								max = histogram[i];
						}
					}
					// 
					for(int i = 0; i < totalpixel; i++){
						histogram[i] = ((double)histogram[i]/max)*histImage.rows;
					}
					//
					for(int i = 0; i < totalpixel; i++){
						line(histImage, Point(bin_w*(i), hist_h),
									Point(bin_w*(i), hist_h - histogram[i]),
							tab_color[rgv], 1, 8, 0);
					}
					namedWindow(__func__+to_string(rgv), WINDOW_AUTOSIZE );
					imshow(__func__+to_string(rgv), histImage); 
					imwrite(resultFolder+__func__+"_"+to_string(rgv)+"_"+this->dateTime+".jpg", histImage); 
					waitKey(0);
					memset(histogram, 0, sizeof(histogram));
    			}
			}
		}
	}else{
		cout<<"Image vide"<<endl ;
	}
	
}


/*************************************************************
 * fonction pour le contraste 
 ************************************************************/
void Image::imageContrast(int option){
	string resultFolder = this->results+Image::getFolder(__func__)+"/"; 
	string file = this->sources+this->imageName; 
	int typeRead  = this->imageChannel; 
	Mat image = imread(file, typeRead);
	int nb_value = image.total(); 
	int matImage[nb_value] = {0}; 
	
	switch(option){
		case 0 : { // variation max min
			int k=0; 
			float maxPixel, minPixel ; 
			float contrast ; 
			Mat new_image = Mat::zeros( image.size(), image.type() );
			int nb_elt = sizeof(matImage)/sizeof(int) ;
			if(image.channels() == 1){
				while(k<nb_value){
					for (int y = 0; y < image.rows; y++){
						for (int x = 0; x < image.cols; x++){
							matImage[k]=(int)image.at<uchar>(y, x);
							k=k+1;
						}
					}
				}
				// max 
				maxPixel = matImage[0];
				for(int i=0; i<nb_elt ; i++){
					if(matImage[i] > maxPixel){
						maxPixel = matImage[i]; 
					}
				}
				cout<<"max"<<maxPixel<<endl; 
				// min
				minPixel = matImage[0];
				for(int i=0; i<nb_elt ; i++){
					if(matImage[i] < minPixel){
						minPixel = matImage[i]; 
					}
				}
				cout<<"min"<<minPixel<<endl; 
				
				contrast = (maxPixel - minPixel)/(maxPixel-minPixel); 
				 
				for (int y = 0; y < image.rows; y++){
					for (int x = 0; x < image.cols; x++)
						new_image.at<uchar>(y, x) = contrast*image.at<uchar>(y,x) ;
				}
				
				namedWindow("Image Originale", 1);
				namedWindow(__func__, 1);
				
				imshow("Image Originale", image);
				imshow(__func__, new_image);
				waitKey();
			}else if(image.channels() == 3){
				Mat imageRGB[3] ; 
				split(image, imageRGB);
				int tab_contrast[3]; 
				for(int p=0; p<3; p++){
						for (int y = 0; y < imageRGB[p].rows; y++){
							for (int x = 0; x < imageRGB[p].cols; x++)
								matImage[(int) imageRGB[p].at<uchar>(y, x)]++;
						}
						
						// max 
						maxPixel = matImage[0];
						for(int i=0; i<nb_elt ; i++){
							if(matImage[i] > maxPixel){
								maxPixel = matImage[i]; 
							}
						}
						cout<<"max"<<maxPixel<<endl; 
						// min
						minPixel = matImage[0];
						for(int i=0; i<nb_elt ; i++){
							if(matImage[i] < minPixel){
								minPixel = matImage[i]; 
							}
						}
						cout<<"min"<<minPixel<<endl; 
						tab_contrast[p] = (maxPixel - minPixel)/(maxPixel-minPixel); 
						cout<<"constrast"<<tab_contrast[p]<<endl; 
				}
				
				for( int y = 0; y < image.rows; y++ ) {
					for( int x = 0; x < image.cols; x++ ) {
						for( int i = 0; i < 3; i++ ) {
							new_image.at<Vec3b>(y,x)[i] =
							  saturate_cast<uchar>( tab_contrast[i] *( image.at<Vec3b>(y,x)[i] ) );
						}
					}
				}
				
				namedWindow("Image Originale", 1);
				namedWindow(__func__, 1);
				
				imshow("Image Originale", image);
				imshow(__func__, new_image); 
				waitKey();
				
			}
			break ; 
			} 
		case 1 : { // Ecart type
			int k=0,somme=0; 
			float moy=0; 
			float contrast=0; 
			float S=0; 
			Mat new_image = Mat::zeros( image.size(), image.type() );; 
			int nb_elt = sizeof(matImage)/sizeof(int) ;
			if(image.channels() == 1){
				while(k<nb_value){
					for (int y = 0; y < image.rows; y++){
						for (int x = 0; x < image.cols; x++){
							matImage[k]=(int)image.at<uchar>(y, x);
							k=k+1;
						}
					}
				}
				
				for(int i=0; i<nb_value ; i++){
					cout<<matImage[i]<<endl;
					somme = somme+matImage[i]; 
				}
				cout<<"Somme"<<somme<<endl; 
				
				moy = somme/nb_value; 
				cout<<"Moyenne"<<moy<<endl; 
				S=0; 
				for (int y = 0; y < image.rows; y++){
					for (int x = 0; x < image.cols; x++)
						S = S+pow((int)image.at<uchar>(y, x)-moy,2);
				}
				cout<<"N"<<image.cols<<endl; 
				cout<<"M"<<image.rows<<endl; 
				contrast = sqrt(S/(image.rows*image.cols));
				cout<<"contrast"<<contrast<<endl; 
				for (int y = 0; y < image.rows; y++){
					for (int x = 0; x < image.cols; x++)
						new_image.at<uchar>(y, x) = contrast*image.at<uchar>(y,x) ;
				}
				
				namedWindow("Image Originale", 1);
				namedWindow(__func__, 1);
				
				imshow("Image Originale", image);
				imshow(__func__, new_image);
				waitKey();
			}else if(image.channels() == 3){
				Mat imageRGB[3] ; 
				split(image, imageRGB);
				for(int i=0; i<3; i++){
					
				}
			}
			break; 
			}  
		default:{
			
		}
	}	
}
