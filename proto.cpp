/* 
   build: g++ proto.cpp -o fenykard -lboost_system -lboost_filesystem 
   run: ./proto <"absolute path">
   */

#include "boost/filesystem/operations.hpp" // includes boost/filesystem/path.hpp
#include "boost/filesystem/fstream.hpp"    // ditto
#include "boost/filesystem.hpp"
#include <iostream>                        // for std::cout
#include <vector>
#include <string>
#include <sstream> 


using namespace boost::filesystem;
using namespace std;

std::ofstream ofs ("merge.txt", std::ofstream::out);



void merge(vector<int>& l2f, vector<int>& f2l ){
	
	for ( int i {0}; i<l2f.size(); ++i ) {
			 ofs<<l2f[i] << ", ";
		if ( i<f2l.size() )
			ofs << f2l[i] << ", ";
	}
	l2f.clear();
	f2l.clear();
	//ofs<<"\n";
}

void extractIntegerWords(string str, vector<int>& out) 
{ 
    stringstream ss;     
  
    /* Storing the whole string into string stream */
    ss << str; 
  
    /* Running loop till the end of the stream */
    string temp; 
    int found; 
    while (!ss.eof()) { 
  
        /* extracting word by word from stream */
        ss >> temp; 
  
        /* Checking the given word is integer or not */
        if (stringstream(temp) >> found){
            out.push_back(found); 

        }
  
        /* To save from space at the end of string */
        temp = ""; 
    } 
}
void getFileContent(boost::filesystem::path fileName, vector<int>& l2f, vector<int>& f2l)
{
 	
	// Open the File
	std::ifstream in(fileName.c_str());
 
	// Check if object is valid
	if(!in)
	{
		std::cerr << "Cannot open the File : "<<fileName<<std::endl;
		
	}

	std::string str;
	// Read the next line from File untill it reaches the end.
	while (std::getline(in, str)){
		
		// Line contains string of length > 0 then save it in vector
		if(str.find("lost2found:") == 0){
			if(str.size() > 13 ){
				extractIntegerWords(str, l2f);
			}
		}

		if (str.find("found2lost:") == 0){
			if( str.size() > 13){
				extractIntegerWords(str, f2l);
				ofs<<"\n";
			}
		}
	}
	merge(l2f, f2l);

	//Close The File
	in.close();
	
}

string getExt(path p){
	if (p.has_extension()){
        	return p.extension().string();
        }
    return "";
}

void dirent(path p, vector<int>& l2f, vector<int>& f2l){


	try {

    if (exists(p)) {
      
      if (is_regular_file(p)){
       // cout << p << " size is " << file_size(p) << '\n';
       
       if (getExt(p) == ".txt"){
       		
			getFileContent(p, l2f, f2l);
       }
    }

      else if (is_directory(p)) {
        //cout << p << " is a directory containing:\n";

        for (directory_entry& x : directory_iterator(p)){
        
         // cout << "    " << x.path() << '\n'; 
          dirent(x.path(), l2f, f2l);
      		
        }
      }
     
    }
    

  }

  catch (const filesystem_error& ex)
  {
    cout << ex.what() << '\n';
  }

}


int main(int argc, char* argv[]){
	path p;
	
	if ( argv[1] != NULL){
		p = argv[1];
	}
	else {
		p =  boost::filesystem::current_path();
	}

	std::vector<int> l2f;
	std::vector<int> f2l;

	dirent(p, l2f, f2l);

	ofs.close();
	
	return 0;
}
