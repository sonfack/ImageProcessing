
#include "image.hpp"

int main(){

	string fileimage ; 
	int typeread ; 
	cout<<"Entrez le nom du fichier : " ; 
	cin>>fileimage; 
	cout<<endl<<"Nom du fichier image : "<< fileimage<<endl; 
	if(!fileimage.empty()){   
		Image raw = Image(fileimage); 
		Image::menu(raw.imageName, raw.imageChannel); 
	}else{
		cout<<"Probleme nom de fichier"<<endl; 
	}	
	return 1; 
}
