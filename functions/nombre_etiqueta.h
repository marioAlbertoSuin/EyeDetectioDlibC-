#include <fstream>
#include <sstream> 
#include <iostream>

int nombre_ubiris(const char* nombre_img)
{

std::stringstream stream(nombre_img);
std::string dato;
std::getline(stream,dato,'_');
std::getline(stream,dato,'_');
return stoi(dato);

}

 
int nombre_casia(const char* nombre_img)
{
std::string name = nombre_img;
std::string label_user = name.substr(2,3);
if (name.substr(5,1) == "R")   label_user = label_user+"1";
if (name.substr(5,1) == "L")   label_user = label_user+"2";
return stoi(label_user);
}



int nombre_personal(const char* nombre_img)
{
std::string name = nombre_img;
std::string label_user = name.substr(0,2);
if (name.substr(2,1) == "R")   label_user = label_user+"1";
if (name.substr(2,1) == "L")   label_user = label_user+"2"; 
return stoi(label_user);

}